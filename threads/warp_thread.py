import os.path

import cv2
import numpy as np
from PIL import Image
from PyQt5 import QtCore
# from matplotlib import pyplot as plt
from skimage.morphology import skeletonize


# def imshow(img, cmap='gray'):
#     plt.imshow(img, cmap=cmap)
#     plt.show()


class WarpThread(QtCore.QThread):
    # [str, str, str]: image_path, binary_path, 'binary_path'
    # [bool]: switch of Stop button
    # [str]: log message
    # [np.ndarray]: polygons points
    signal = QtCore.pyqtSignal([str, str, str], [str], [str, np.ndarray])

    def __init__(self):
        super(WarpThread, self).__init__()
        self.args = None
        self.num = 0
        self.task = 'Warp'
        self.isOn = False

    def run(self) -> None:
        self.isOn = True
        try:
            self.warp(**self.args)
        except Exception as e:
            self.signal[str].emit(str(e))

    def stop(self):
        self.isOn = False
        # self.terminate()
        # self.wait()
        # self.deleteLater()

    def warp(self, file_dict, root_path, save_dir, target_width, target_height):
        processid = 0
        for image_path, images_path in file_dict.items():
            if images_path['binary_path'] is None:
                continue
            if not self.isOn:
                break
            processid += 1
            self.signal[str].emit(f'[WARP]\t[{processid}/{self.num}] Warping {image_path}...')
            image = np.array(Image.open(image_path))
            # image = cv2.imread(image_path)
            # imshow(image, cmap=None)
            binary = np.array(Image.open(images_path['binary_path']).convert('L'))
            # binary = cv2.imread(images_path['binary_path'], 0)
            # imshow(binary, cmap='gray')
            canvas_list = self.get_canvas(binary)
            for i, c in enumerate(canvas_list):
                if not self.isOn:
                    break
                crosspoints = np.array(self.get_crosspoints(c), int)
                t_w = target_width
                t_h = target_height
                target_points = np.array(
                    [[100, 100], [t_w - 100, 100], [t_w - 100, t_h - 100], [100, t_h - 100]], np.float32)
                M = cv2.getPerspectiveTransform(crosspoints.astype(np.float32), target_points)
                output = cv2.warpPerspective(image, M, (t_w, t_h))
                # 在图片文件名加后缀'_i'
                save_path = image_path.replace(root_path, save_dir)
                save_path = save_path.split('.')[0] + f'_{i + 1}' + '.png'
                if not os.path.exists(os.path.dirname(save_path)):
                    os.makedirs(os.path.dirname(save_path))
                Image.fromarray(output).save(save_path)
                self.signal[str, np.ndarray].emit(image_path, crosspoints)
                self.signal[str, np.ndarray].emit('warped', output)
                self.signal[str].emit(f'[WARP]\tWarping {save_path}...Seved')
            self.signal[str, str, str].emit(image_path, None, 'warp')

    def get_canvas(self, binary):
        # image: np.ndarray, binary: np.ndarray
        # return: [np.ndarray,...]
        canvas = binary.copy()
        hasNoise = True
        while hasNoise:
            retval, labels, stats, centroids = cv2.connectedComponentsWithStats(canvas)
            areas = stats[1:, cv2.CC_STAT_AREA]
            threshold = np.mean(areas) * 0.75
            hasNoise = False
            for i in range(1, retval):
                if areas[i - 1] < threshold:
                    canvas[labels == i] = 0
                    hasNoise = True
        pad = np.zeros((binary.shape[0] + 600, binary.shape[1] + 600), np.uint8)
        pad[300:300 + binary.shape[0], 300:300 + binary.shape[1]] = canvas
        # 闭运算
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))
        iterations = 1
        canvas = cv2.morphologyEx(pad, cv2.MORPH_CLOSE, kernel, iterations=iterations)
        # 泛洪填充
        h, w = canvas.shape[:2]
        mask = np.zeros((h + 2, w + 2), np.uint8)
        # 第一个黑色点为seed
        seed = None
        for i in range(h):
            for j in range(w):
                if canvas[i, j] == 0:
                    seed = (j, i)
                    break
            if seed is not None:
                break
        cv2.floodFill(canvas, mask, seed, 125)
        canvas[canvas == 0] = 255
        canvas[canvas == 125] = 0
        # 开运算
        canvases = cv2.morphologyEx(canvas, cv2.MORPH_OPEN, kernel, iterations=iterations)
        retval, labels, stats, centroids = cv2.connectedComponentsWithStats(canvases)
        canvas_list = []
        centroids_y_list = centroids[1:, 0]
        # 按照y坐标从小到大排序后的索引
        centroids_y_list = np.argsort(centroids_y_list)
        for i in centroids_y_list:
            canvas = np.zeros(canvases.shape, np.uint8)
            canvas[labels == i + 1] = 255
            canvas_list.append(canvas)
        return canvas_list

    def get_crosspoints(self, img):
        cornerHarris = cv2.cornerHarris(img, 70, 9, 0.06)
        cornerHarris_bin = cv2.threshold(cornerHarris, -100, 255, cv2.THRESH_BINARY_INV)[1]
        cornerHarris_bin = np.uint8(cornerHarris_bin)
        retval, labels, stats, centroids = cv2.connectedComponentsWithStats(cornerHarris_bin)
        WHr = stats[:, 2] / stats[:, 3]
        stats = np.insert(stats.astype(np.float64), 4, WHr, 1)
        area_threshold = np.mean(stats[1:, 4]) * 0.5
        for i in range(len(stats)):
            if 0.2 < stats[i, 4] < 5 or stats[i, -1] < area_threshold:
                cornerHarris_bin[labels == i] = 0
        # pad_up = np.zeros((50, cornerHarris_bin.shape[1]), np.uint8)
        # pad_left = np.zeros((cornerHarris_bin.shape[0] + 50, 50), np.uint8)
        # cornerHarris_bin_padup = np.vstack((pad_up, cornerHarris_bin))
        # cornerHarris_bin_pad = np.hstack((pad_left, cornerHarris_bin_padup))
        ski = cornerHarris_bin.copy()
        ski[ski == 255] = 1
        skeleton = skeletonize(ski).astype(np.uint8)
        skeleton[skeleton == 1] = 255
        retval, labels, stats, centroids = cv2.connectedComponentsWithStats(skeleton)
        for i in range(1, retval):
            rate = stats[i, 2] / stats[i, 3]
            x, y, w, h, a = stats[i]
            if rate > 1:
                skeleton[y:y + h, x:x + 50] = 0
                skeleton[y:y + h, x + w - 50:x + w] = 0
            elif rate < 1:
                skeleton[y:y + 50, x:x + w] = 0
                skeleton[y + h - 50:y + h, x:x + w] = 0
        # skeleton = myskeletonize(cornerHarris_bin_pad)
        skeleton = skeleton[300:-300, 300:-300]
        arr = np.argwhere(skeleton == 255)
        meany, meanx = np.mean(arr, 0)
        Rangey, Rangex = np.max(arr, 0) - np.min(arr, 0)
        up1, down1 = self.get_points(arr, 1, meanx - Rangex / 5, meany, 1)
        up2, down2 = self.get_points(arr, 1, meanx + Rangex / 5, meany, 0)
        left1, right1 = self.get_points(arr, 0, meany - Rangey / 5, meanx, 1)
        left2, right2 = self.get_points(arr, 0, meany + Rangey / 5, meanx, 0)

        # cv2.line(skeleton, up1, up2, 255, 10)
        # cv2.line(skeleton, down1, down2, 255, 10)
        # cv2.line(skeleton, left1, left2, 255, 10)
        # cv2.line(skeleton, right1, right2, 255, 10)

        upK = (up2[1] - up1[1]) / (up2[0] - up1[0])
        upB = up2[1] - upK * up2[0]
        downK = (down2[1] - down1[1]) / (down2[0] - down1[0])
        downB = down2[1] - downK * down2[0]
        leftK = (left2[1] - left1[1]) / (left2[0] - left1[0])
        leftB = left1[1] - leftK * left1[0]
        rightK = (right2[1] - right1[1]) / (right2[0] - right1[0])
        rightB = right1[1] - rightK * right1[0]
        if left1[0] == left2[0]:
            x_dl = x_ul = left1[0]
        else:
            x_ul = (upB - leftB) / (leftK - upK)
            x_dl = (downB - leftB) / (leftK - downK)
        if right1[0] == right2[0]:
            x_dr = x_ur = right1[0]
        else:
            x_ur = (upB - rightB) / (rightK - upK)
            x_dr = (downB - rightB) / (rightK - downK)
        y_ul = upK * x_ul + upB
        y_ur = upK * x_ur + upB
        y_dl = downK * x_dl + downB
        y_dr = downK * x_dr + downB
        return (x_ul, y_ul), (x_ur, y_ur), (x_dr, y_dr), (x_dl, y_dl)

    def get_points(self, arr, dir, index, meanY, flag):
        index = int(index)
        if dir:
            if index < np.min(arr, 0)[1] or index > np.max(arr, 0)[1]:
                return (0, 0), (0, 0)
        else:
            if index < np.min(arr, 0)[0] or index > np.max(arr, 0)[0]:
                return (0, 0), (0, 0)
        points_index = np.argwhere(arr[:, dir] == index)
        points = []
        for i in points_index[:, 0]:
            points.append(arr[i])
        point1, point2 = [], []
        for i in range(len(points)):
            y = points[i][0 if dir else 1]
            if y < meanY:
                point1.append([index, y] if dir else [y, index])
            else:
                point2.append([index, y] if dir else [y, index])
        point1, point2 = np.array(point1), np.array(point2)
        if len(point1) == 0 or len(point2) == 0:
            if flag:
                return self.get_points(arr, dir, index + 50, meanY, 1)
            else:
                return self.get_points(arr, dir, index - 50, meanY, 0)
        return np.mean(point1, 0, int), np.mean(point2, 0, int)


# if __name__ == '__main__':
#     warper = WarpThread()
#     warper.isOn = True
#     file_dict = {r'E:\wxrice\org\0923\2_13_4.png': {'binary_path': r'E:\wxrice\soil5\0923\2_13_4.png'}}
#     warper.warp(file_dict, r'E:\wxrice\org\0923', r'E:\wxrice\warped\0923', 2024, 3400)
