import csv
import math
import os
import time

import cv2
# import matplotlib.pyplot as plt
import numpy as np
from PIL import Image
from PyQt5.QtCore import QThread, pyqtSignal, QMutex, QMutexLocker
from skimage import morphology
import base64
import pickle


class CalculateThread(QThread):
    signal = pyqtSignal([str], [dict], [str, str, str])  # 注意: 将np.ndarray改为object以避免导入问题

    def __init__(self):
        super(CalculateThread, self).__init__()
        self.args = None
        self.num = 0
        self.task = 'Calculate'
        self.isOn = False

    def run(self):
        self.isOn = True
        print('start calculate thread')
        self.calculate_traits(**self.args)
        print('end calculate thread')
        self.isOn = False

    def stop(self):
        self.isOn = False

    def calculate_traits(self, file_dict, save_path, dataset_rootpath, Layer_height, Layer_width):
        calculater = Calculater(dataset_rootpath)
        print('start calculate traits')
        self.signal[str].emit(f'[CALC]\tNumber of predict images = {self.num}')
        start_time = time.time()

        if os.path.exists(os.path.join(save_path, 'traits.csv')):
            os.remove(os.path.join(save_path, 'traits.csv'))

        processid = 0
        for image_path, images_path in file_dict.items():
            if not self.isOn:
                break

            try:
                img_path = images_path.get('processed_path') or images_path.get('binary_path') or image_path
                if not os.path.exists(img_path):
                    continue

                processid += 1
                print(f'processid:{processid}/{self.num}')

                show_img = calculater.loadimage(img_path)
                traits = calculater.get_traits(Layer_height=Layer_height, Layer_width=Layer_width)
                traits['image_path'] = image_path
                self.signal[dict].emit(traits)
                new_path = calculater.save_traits(save_path)

                # 问题所在 20250324
                self.signal[str, str, str].emit(image_path, new_path, 'visualization')

                cost = time.time() - start_time
                total_cost = time.time() - start_time
                eta = (total_cost) / (processid) * (self.num - processid)
                self.signal[str].emit(
                    f'[CALC]\t{processid}/{self.num} {image_path}: {cost:.2f}s\ttotal_cost:{total_cost:.2f}s\teta:{eta:.2f}s')

            except Exception as e:
                print(f"Error processing image {image_path}: {e}")


class Calculater(object):
    def __init__(self, dataset_rootpath=None):
        self.dataset_rootpath = dataset_rootpath
        pass

    def loadimage(self, image_path):
        self.Empty = False
        self.image_path = image_path

        # 读取并二值化图像
        self.img = np.array(Image.open(image_path).convert('L'))
        _, self.img = cv2.threshold(self.img, 10, 255, cv2.THRESH_BINARY)
        self.show_img = cv2.cvtColor(self.img, cv2.COLOR_GRAY2BGR)

        # 获取所有前景点坐标 (x, y)
        self.coordinates = np.argwhere(self.img == 255)[:, ::-1]  # shape: (N, 2)

        if self.coordinates.size == 0:
            self.Empty = True
        else:
            self.centroid = np.mean(self.coordinates, axis=0)
            # 计算凸包
            hull = cv2.convexHull(self.coordinates)
            if hull is None or len(hull) < 3:
                self.Empty = True
            else:
                self.convexhull = np.squeeze(hull, axis=1)  # shape: (M, 2)
                w_min, d_min = np.min(self.convexhull, axis=0)
                w_max, d_max = np.max(self.convexhull, axis=0)
                self.width = w_max - w_min
                self.depth = d_max - d_min

        # 初始化凸包图像
        self.convexhull_img = np.zeros_like(self.img, dtype=np.uint8)
        if not self.Empty:
            cv2.fillConvexPoly(self.convexhull_img, self.convexhull, 255)
            self.show_img = cv2.addWeighted(
                self.show_img, 1,
                cv2.cvtColor(self.convexhull_img, cv2.COLOR_GRAY2BGR), 0.5, 0
            )

        # 凸包轮廓点
        convexhull_contour_img = np.zeros_like(self.img, dtype=np.uint8)
        if not self.Empty:
            cv2.polylines(convexhull_contour_img, [self.convexhull], isClosed=True, color=255, thickness=1)
        self.convexhull_Contours = np.argwhere(convexhull_contour_img == 255)[:, ::-1]  # (x, y)

        # 骨架
        if not self.Empty:
            binary = (self.img > 0).astype(np.uint8)
            self.skeleton_img = morphology.skeletonize(binary).astype(np.uint8) * 255
            self.show_img[self.skeleton_img == 255] = [0, 0, 255]
        else:
            self.skeleton_img = self.img

        # 如果为空，直接返回默认值
        if self.Empty:
            zero_pt = np.array([0, 0], dtype=np.int64)
            self.start = self.left_ = self.right_ = self.left = self.right = zero_pt
            return self.show_img

        # === 计算 start 点（顶部均值）===
        sorted_by_y = self.convexhull[np.lexsort(self.convexhull[:, 1:2].T)]  # 按 y 排序
        top_y = sorted_by_y[0, 1]
        top_xs = sorted_by_y[sorted_by_y[:, 1] == top_y, 0]
        start_x = int(np.round(np.mean(top_xs)))
        self.start = np.array([start_x, int(top_y)], dtype=np.int64)

        # === 计算 left 和 right（最左/右均值）===
        sorted_by_x = self.convexhull[np.lexsort(self.convexhull[:, :1].T)]  # 按 x 排序
        # 最左点
        left_x = sorted_by_x[0, 0]
        left_mask = (sorted_by_x[:, 0] == left_x)
        left_ys = sorted_by_x[left_mask, 1]

        if left_ys.size > 0:
            # 确保找到的点的 x 小于 start 的 x
            if left_x < self.start[0]:
                left_y = int(np.round(np.mean(left_ys)))
                self.left = np.array([int(left_x), left_y], dtype=np.int64)
            else:
                # 如果没有找到合适的点，则回退到起始点
                self.left = self.start.copy()
        else:
            # 如果没有找到任何点，则直接回退到起始点
            self.left = self.start.copy()

        # 最右点
        right_x = sorted_by_x[-1, 0]
        right_mask = (sorted_by_x[:, 0] == right_x)
        right_ys = sorted_by_x[right_mask, 1]
        if right_ys.size > 0:
            # 确保找到的点的 x 大于 start 的 x
            if right_x > self.start[0]:
                right_y = int(np.round(np.mean(right_ys)))
                self.right = np.array([int(right_x), right_y], dtype=np.int64)
            else:
                # 如果没有找到合适的点，则回退到起始点
                self.right = self.start.copy()
        else:
            # 如果没有找到任何点，则直接回退到起始点
            self.right = self.start.copy()

        # === 安全计算 left_ 和 right_ ===
        def _get_mean_point_near_start(contour_points, is_left_side=True):
            """从 contour_points 中，在 start 下方若干行找点，返回均值"""
            offsets = [10, 30, 50, 70, 90]
            collected = []
            for dy in offsets:
                target_y = self.start[1] + dy
                if is_left_side:
                    candidates = contour_points[
                        (contour_points[:, 0] < self.start[0]) & (contour_points[:, 1] == target_y)
                        ]
                else:
                    candidates = contour_points[
                        (contour_points[:, 0] > self.start[0]) & (contour_points[:, 1] == target_y)
                        ]
                if candidates.size > 0:
                    collected.append(candidates)
            if not collected:
                return self.start.copy()
            all_pts = np.vstack(collected)
            mean_pt = np.mean(all_pts, axis=0)
            return np.round(mean_pt).astype(np.int64)

        self.left_ = _get_mean_point_near_start(self.convexhull_Contours, is_left_side=True)
        self.right_ = _get_mean_point_near_start(self.convexhull_Contours, is_left_side=False)

        # 计算外接椭圆
        ellipse = cv2.fitEllipse(self.convexhull_Contours)
        (ellipse_center, ellipse_radius, ellipse_angle) = ellipse
        ellipse_x, ellipse_y = ellipse_center
        self.ellipse_center = np.array([int(ellipse_x), int(ellipse_y)], dtype=np.int64)  # 中心
        self.ellipse_radius = ellipse_radius  # 长短轴
        self.ellipse_angle = ellipse_angle  # 偏转角

        # === 绘图（确保所有坐标为整数元组）===
        w_min, d_min = np.min(self.convexhull, axis=0).astype(int)
        w_max, d_max = np.max(self.convexhull, axis=0).astype(int)

        # 外接矩形
        cv2.line(self.show_img, (w_min, d_min), (w_max, d_min), (70, 200, 70), 2)
        cv2.line(self.show_img, (w_min, d_min), (w_min, d_max), (70, 200, 70), 2)
        cv2.line(self.show_img, (w_max, d_min), (w_max, d_max), (70, 200, 70), 2)
        cv2.line(self.show_img, (w_min, d_max), (w_max, d_max), (70, 200, 70), 2)

        # 连接线
        def _to_tuple(pt):
            return tuple(map(int, pt))

        cv2.line(self.show_img, _to_tuple(self.start), _to_tuple(self.left_), (0, 255, 255), 2)
        cv2.line(self.show_img, _to_tuple(self.start), _to_tuple(self.right_), (0, 255, 255), 2)
        cv2.line(self.show_img, _to_tuple(self.start), _to_tuple(self.left), (255, 255, 0), 2)
        cv2.line(self.show_img, _to_tuple(self.start), _to_tuple(self.right), (255, 255, 0), 2)

        # 圆点
        cv2.circle(self.show_img, _to_tuple(self.start), 7, (0, 0, 255), -1)
        cv2.circle(self.show_img, _to_tuple(self.left_), 7, (0, 255, 255), -1)
        cv2.circle(self.show_img, _to_tuple(self.right_), 7, (0, 255, 255), -1)
        cv2.circle(self.show_img, _to_tuple(self.left), 7, (255, 255, 0), -1)
        cv2.circle(self.show_img, _to_tuple(self.right), 7, (255, 255, 0), -1)
        cv2.circle(self.show_img, _to_tuple(self.centroid), 7, (255, 0, 0), -1)
        cv2.circle(self.show_img, _to_tuple(self.ellipse_center), 7, (255, 0, 255), -1)

        return self.show_img

    # def loadimage(self, image_path):
    #     self.Empty = False
    #     self.image_path = image_path
    #     # 向右为x轴正方向,向下为y轴正方向
    #     self.img = np.array(Image.open(image_path).convert('L'))
    #     self.img = cv2.threshold(self.img, 10, 255, cv2.THRESH_BINARY)[1]
    #     self.show_img = cv2.cvtColor(self.img, cv2.COLOR_GRAY2BGR)
    #     # 二值图所有点坐标（x, y）
    #     self.coordinates = np.argwhere(self.img == 255)[:, ::-1]
    #     self.centroid = np.mean(self.coordinates, axis=0)
    #     # 二值图凸包的顶点坐标（x, y）
    #     self.convexhull = np.squeeze(cv2.convexHull(self.coordinates), axis=1)
    #     if self.convexhull.shape[0] == 0:
    #         self.Empty = True
    #     else:
    #         w_min, d_min = np.min(self.convexhull, axis=0)
    #         w_max, d_max = np.max(self.convexhull, axis=0)
    #         self.width = w_max - w_min
    #         self.depth = d_max - d_min
    #     # 凸包图
    #     self.convexhull_img = np.zeros_like(self.img, dtype=np.uint8)
    #     if not self.Empty:
    #         cv2.fillConvexPoly(self.convexhull_img, self.convexhull, 255)
    #         self.show_img = cv2.addWeighted(self.show_img, 1, cv2.cvtColor(self.convexhull_img, cv2.COLOR_GRAY2BGR),
    #                                         0.5, 0)
    #     # 凸包轮廓所有点坐标
    #     convexhull_img = np.zeros_like(self.img, dtype=np.uint8)
    #     if not self.Empty:
    #         cv2.polylines(convexhull_img, [self.convexhull], True, 255, 1)
    #     self.convexhull_Contours = np.argwhere(convexhull_img == 255)[:, ::-1]
    #     # 骨架图
    #     if not self.Empty:
    #         binary = self.img.copy()
    #         binary[binary > 0] = 1
    #         self.skeleton_img = morphology.skeletonize(binary).astype(np.uint8) * 255
    #         self.show_img[self.skeleton_img == 255] = [0, 0, 255]
    #     else:
    #         self.skeleton_img = self.img
    #     # 五个点
    #     if self.Empty:
    #         self.start = self.left_ = self.right_ = self.left = self.right = np.array([0, 0])
    #         return
    #     # 从上到下排序，顶层取均值为起始点
    #     self.convexhull = self.convexhull.copy()[np.lexsort(self.convexhull.copy().T)]
    #     starty = self.convexhull[0][1]
    #     startx = np.mean(self.convexhull[np.argwhere(self.convexhull[:, 1] == starty), 0], dtype=np.int64)
    #     self.start = np.array([startx, starty])
    #     # 从左到右排序，分别取最左侧和最右侧均值
    #     convexhull = self.convexhull[np.lexsort(self.convexhull[:, ::-1].T)]
    #     leftx = convexhull[0][0]
    #     lefty = np.mean(convexhull[np.argwhere(convexhull[:, 0] == leftx), 1], dtype=np.int64)
    #     self.left = np.array([leftx, lefty])
    #     rightx = convexhull[-1][0]
    #     righty = np.mean(convexhull[np.argwhere(convexhull[:, 0] == rightx), 1], dtype=np.int64)
    #     self.right = np.array([rightx, righty])
    #     # 起始点附近起始点两侧凸包上的点的均值
    #     left = self.convexhull_Contours[np.argwhere(self.convexhull_Contours[:, 0] < self.start[0]).squeeze()]
    #     left_ = left[np.argwhere(left[:, 1] == self.start[1] + 10)]
    #     left_ = np.concatenate((left_, left[np.argwhere(left[:, 1] == self.start[1] + 30)]), axis=0)
    #     left_ = np.concatenate((left_, left[np.argwhere(left[:, 1] == self.start[1] + 50)]), axis=0)
    #     left_ = np.concatenate((left_, left[np.argwhere(left[:, 1] == self.start[1] + 70)]), axis=0)
    #     left_ = np.concatenate((left_, left[np.argwhere(left[:, 1] == self.start[1] + 90)]), axis=0).squeeze(axis=1)
    #     self.left_ = np.mean(left_, axis=0, dtype=np.int64)
    #     right = self.convexhull_Contours[np.argwhere(self.convexhull_Contours[:, 0] > self.start[0]).squeeze()]
    #     right_ = right[np.argwhere(right[:, 1] == self.start[1] + 10)]
    #     right_ = np.concatenate((right_, right[np.argwhere(right[:, 1] == self.start[1] + 30)]), axis=0)
    #     right_ = np.concatenate((right_, right[np.argwhere(right[:, 1] == self.start[1] + 50)]), axis=0)
    #     right_ = np.concatenate((right_, right[np.argwhere(right[:, 1] == self.start[1] + 70)]), axis=0)
    #     right_ = np.concatenate((right_, right[np.argwhere(right[:, 1] == self.start[1] + 90)]), axis=0).squeeze(
    #         axis=1)
    #     self.right_ = np.mean(right_, axis=0, dtype=np.int64)
    #     if not self.Empty:
    #         if not self.Empty:
    #             # 对角点 (w_min, d_min), (w_max, d_min) 等等，也应确保它们是整数
    #             cv2.line(self.show_img, (int(w_min), int(d_min)), (int(w_max), int(d_min)), (70, 200, 70), 2)
    #             cv2.line(self.show_img, (int(w_min), int(d_min)), (int(w_min), int(d_max)), (70, 200, 70), 2)
    #             cv2.line(self.show_img, (int(w_max), int(d_min)), (int(w_max), int(d_max)), (70, 200, 70), 2)
    #             cv2.line(self.show_img, (int(w_min), int(d_max)), (int(w_max), int(d_max)), (70, 200, 70), 2)
    #
    #             # 使用 tuple(map(int, ...)) 来确保每个点都被转换为整数元组
    #             cv2.line(self.show_img, tuple(map(int, self.start)), tuple(map(int, self.left_)), (0, 255, 255), 2)
    #             cv2.line(self.show_img, tuple(map(int, self.start)), tuple(map(int, self.right_)), (0, 255, 255), 2)
    #             cv2.line(self.show_img, tuple(map(int, self.start)), tuple(map(int, self.left)), (255, 255, 0), 2)
    #             cv2.line(self.show_img, tuple(map(int, self.start)), tuple(map(int, self.right)), (255, 255, 0), 2)
    #
    #             # 同样地，对于 cv2.circle 调用也需要保证圆心位置为整数元组
    #             cv2.circle(self.show_img, tuple(map(int, self.start)), 7, (0, 0, 255), -1)
    #             cv2.circle(self.show_img, tuple(map(int, self.left_)), 7, (0, 255, 255), -1)
    #             cv2.circle(self.show_img, tuple(map(int, self.right_)), 7, (0, 255, 255), -1)
    #             cv2.circle(self.show_img, tuple(map(int, self.left)), 7, (255, 255, 0), -1)
    #             cv2.circle(self.show_img, tuple(map(int, self.right)), 7, (255, 255, 0), -1)
    #             cv2.circle(self.show_img, tuple(map(int, self.centroid)), 7, (255, 0, 0), -1)
    #         # plt.imshow(self.show_img)
    #     return self.show_img

    def pad_img(self, img, row_pad, col_pad, row_num, col_num):
        # 将图像填充为Layer_height*Layer_width的整数倍
        img_pad = np.pad(img, ((0, row_pad), (0, col_pad)), 'constant', constant_values=0)
        self.row_num = row_num if row_pad == 0 else row_num + 1
        self.col_num = col_num if col_pad == 0 else col_num + 1
        # 将填充后的图像切片为(row_num, col_num, Layer_height, Layer_width)的形状
        img_pad = img_pad.reshape((self.row_num, self.Layer_height_, self.col_num, self.Layer_width_)).transpose(
            (0, 2, 1, 3))
        return img_pad

    def get_area(self):
        return self.coordinates.shape[0]

    def get_convex_area(self):
        return np.count_nonzero(self.convexhull_img)

    def get_length(self):
        return np.count_nonzero(self.skeleton_img)

    def get_wdr(self):
        w_min, d_min = np.min(self.convexhull, axis=0)
        w_max, d_max = np.max(self.convexhull, axis=0)
        width = w_max - w_min
        depth = d_max - d_min
        return width, depth, width / depth if depth != 0 else 0

    def get_sturdiness(self):
        return self.get_area() / self.get_convex_area() if self.get_convex_area() != 0 else 0

    def get_centroid(self):
        return list(
            np.mean(self.coordinates, axis=0) - np.array([self.start[0], self.start[1]])) if not self.Empty else [0, 0]

    def get_ellipse_distance(self):
        """
        计算质心与根节点的距离与角度
        Returns:
        与根节点距离，与根节点角度
        """
        if self.Empty:
            return 0
        (x_dist_start, t_dist_start) = self.ellipse_center - self.start
        dist_to_start = (x_dist_start ** 2 + t_dist_start ** 2) ** 0.5
        angle_to_start = math.atan2(x_dist_start, t_dist_start)
        return dist_to_start, angle_to_start

    def get_angles(self):
        if self.Empty:
            return 0
        self.angle_left = math.atan2(- self.left[0] + self.start[0], self.left[1] - self.start[1]) * 180 / math.pi
        self.angle_right = math.atan2(self.right[0] - self.start[0], self.right[1] - self.start[1]) * 180 / math.pi

        if self.left_[0] > self.start[0] or self.left_[1] < self.start[1]:
            self.angle_left_ = 0
        else:
            self.angle_left_ = math.atan2(- self.left_[0] + self.start[0],
                                          self.left_[1] - self.start[1]) * 180 / math.pi
        if self.right_[0] < self.start[0] or self.right_[1] < self.start[1]:
            self.angle_right_ = 0
        else:
            self.angle_right_ = math.atan2(self.right_[0] - self.start[0],
                                           self.right_[1] - self.start[1]) * 180 / math.pi

        return self.angle_left + self.angle_right, self.angle_left, self.angle_right, self.angle_left_ + self.angle_right_, self.angle_left_, self.angle_right_

    def get_mass(self, Layer_height=None, Layer_width=None):
        if self.Empty:
            return 0
        # 以Layer_height*Layer_width为单位，对图像进行切片，计算每个切片的面积、凸包面积、长度和三个比例
        # Layer_height 和 Layer_width 为None或大于图像大小时，以图像的高和宽为单位
        self.Layer_height_ = min(self.img.shape[0], Layer_height) if Layer_height else self.img.shape[0]
        self.Layer_width_ = min(self.img.shape[1], Layer_width) if Layer_width else self.img.shape[1]
        # 为了保证切片后的图像能够完整的覆盖原图像，对图像进行填充
        row_num, row_pad = np.divmod(self.img.shape[0], self.Layer_height_)
        col_num, col_pad = np.divmod(self.img.shape[1], self.Layer_width_)
        row_pad = self.Layer_height_ - row_pad if row_pad else 0
        col_pad = self.Layer_width_ - col_pad if col_pad else 0
        img_pad = self.pad_img(self.img.copy(), row_pad, col_pad, row_num, col_num)
        skeleton_img_pad = self.pad_img(self.skeleton_img.copy(), row_pad, col_pad, row_num, col_num)
        convexhull_img_pad = self.pad_img(self.convexhull_img.copy(), row_pad, col_pad, row_num, col_num)

        mass = {'Area': [], 'Convex_area': [], 'Length': [], 'Area_Convex': [], 'Area_Length': [], 'Length_Convex': []}
        for i in range(row_num):
            for j in range(col_num):
                mass['Area'].append(np.count_nonzero(img_pad[i, j]))
                mass['Convex_area'].append(np.count_nonzero(convexhull_img_pad[i, j]))
                mass['Length'].append(np.count_nonzero(skeleton_img_pad[i, j]))
                mass['Area_Convex'].append(
                    np.count_nonzero(img_pad[i, j]) / np.count_nonzero(convexhull_img_pad[i, j]) if
                    np.count_nonzero(convexhull_img_pad[i, j]) else 0)
                mass['Area_Length'].append(np.count_nonzero(img_pad[i, j]) / np.count_nonzero(skeleton_img_pad[i, j]) if
                                           np.count_nonzero(skeleton_img_pad[i, j]) else 0)
                mass['Length_Convex'].append(np.count_nonzero(skeleton_img_pad[i, j]) / np.count_nonzero(
                    convexhull_img_pad[i, j]) if np.count_nonzero(convexhull_img_pad[i, j]) else 0)
        return mass

    def get_eccentricity(self):
        # 计算偏心率
        if self.Empty:
            return 0
        major_axis = max(self.ellipse_radius)
        minor_axis = min(self.ellipse_radius)
        return math.sqrt(1 - (minor_axis / major_axis) ** 2) if major_axis > 0 else 0

    def get_traits(self, Layer_height=None, Layer_width=None):
        if self.Empty:
            return 0
        image_height, image_width = self.img.shape[:2]
        image_area = image_width * image_height
        self.traits = {'image_path': self.image_path}
        self.traits['area'] = self.get_area()
        self.traits['norm_area'] = self.traits['area'] / image_area  # 归一化面积
        self.traits['convex_area'] = self.get_convex_area()
        self.traits['norm_convex_area'] = self.traits['convex_area'] / image_area  # 归一化凸包面积
        self.traits['length'] = self.get_length()
        self.traits['norm_length'] = self.traits['length'] / image_area
        self.traits['initial_x'], self.traits['initial_y'] = self.start
        self.traits['norm_initial_x'] = self.traits['initial_x'] / image_width  # 归一化根起点坐标
        self.traits['norm_initial_y'] = self.traits['initial_y'] / image_height  # 归一化根起点坐标
        self.traits['diameter'] = self.traits['area'] / self.traits['length']
        self.traits['width'], self.traits['depth'], self.traits['wd_ratio'] = self.get_wdr()
        self.traits['norm_width'] = self.traits['width'] / image_width  # 归一化根宽
        self.traits['norm_depth'] = self.traits['depth'] / image_height  # 归一化根深
        self.traits['sturdiness'] = self.get_sturdiness()
        self.traits['eccentricity'] = self.get_eccentricity()  # 偏心率
        self.traits['apex_angle'], self.traits['apex_angle_left'], self.traits['apex_angle_right'], self.traits[
            'entire_angle'], self.traits['entire_angle_left'], self.traits['entire_angle_right'] = self.get_angles()
        self.traits['centroid_x'], self.traits['centroid_y'] = self.get_centroid()
        mass = self.get_mass(Layer_height=Layer_height, Layer_width=Layer_width)
        self.traits['layer_mass_A'] = mass['Area']
        self.traits['layer_mass_L'] = mass['Length']
        self.traits['layer_mass_C'] = mass['Convex_area']
        self.traits['layer_mass_A_C'] = mass['Area_Convex']
        self.traits['layer_mass_L_C'] = mass['Length_Convex']
        self.traits['layer_mass_A_L'] = mass['Area_Length']
        return self.traits

    def save_traits(self, save_path):
        image_save_path = self.image_path.replace(self.dataset_rootpath,
                                                  os.path.join(save_path, 'traits_visualization'))
        if not os.path.exists(os.path.dirname(image_save_path)):
            os.makedirs(os.path.dirname(image_save_path))
            print('create dir: ', os.path.dirname(image_save_path))
        self.show_img = Image.fromarray(self.show_img)
        self.show_img.save(image_save_path)
        hashead = False
        try:
            with open(os.path.join(save_path, 'traits.csv'), 'r', newline="") as f:
                reader = csv.DictReader(f)
                if reader.fieldnames:
                    hashead = True
        except:
            pass
        with open(os.path.join(save_path, 'traits.csv'), 'a+', newline="") as f:
            if self.Empty:
                return 0
            writer = csv.DictWriter(f, fieldnames=self.traits.keys())
            if not hashead:
                writer.writeheader()
            writer.writerow(self.traits)
        return image_save_path


if __name__ == '__main__':
    img_path = r'C:\Users\w1863\Desktop\sample\src\20251212_064.png'
    save_path = r'C:\Users\w1863\Desktop\sample\cal'
    traits = Calculater(r'C:\Users\w1863\Desktop\sample')
    traits.loadimage(img_path)
    print(traits.get_traits())
    traits.save_traits(save_path)
