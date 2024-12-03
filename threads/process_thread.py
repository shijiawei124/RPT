import os.path
import onnxruntime
import cv2
import numpy as np
from PIL import Image
from PyQt5.QtCore import QThread, pyqtSignal


class ProcessThread(QThread):
    # 定义信号
    signal = pyqtSignal([str, str, str], [str])

    def __init__(self):
        super(ProcessThread, self).__init__()
        self.args = None
        self.num = 0
        self.isOn = False
        self.task = 'Process'

    def run(self):
        self.isOn = True
        self.process(**self.args)
        self.isOn = False

    def stop(self):
        self.isOn = False
        # self.terminate()
        # self.wait()
        # self.deleteLater()

    def process(self, file_dict, root_path, save_dir, color_plane='N', segmethod=None, threshold=None, rso=False,
                dilation=None, areathreshold=None, rbo=False, left=None, right=None, top=None, bottom=None,
                auto_iters=None):

        self.signal[str].emit(f'Processing {self.num} images...')
        processid = 0
        for image_path, images_path in file_dict.items():
            if not self.isOn:
                break
            if color_plane == 'N':
                if images_path['binary_path'] is None:
                    continue
                processid += 1
                img = Image.open(images_path['binary_path']).convert('L')
                img = np.array(img)
            else:
                processid += 1
                img = Image.open(image_path)
                if color_plane in 'RGB':
                    img = img.convert('RGB').split()['RGB'.index(color_plane)]
                elif color_plane in 'HSV':
                    img = img.convert('HSV').split()['HSV'.index(color_plane)]
                img = np.array(img)
                if 'OTSU' not in segmethod:
                    ret, img = cv2.threshold(img, threshold, 255, cv2.THRESH_BINARY)
                else:
                    ret, img = cv2.threshold(img, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
            if rso:
                if dilation > 0:
                    kernel = np.ones((3, 3), np.uint8)
                    dilate = cv2.dilate(img, kernel, iterations=dilation)
                else:
                    dilate = img.copy()
                if areathreshold > 0:
                    num_labels, labels, stats, centroids = cv2.connectedComponentsWithStats(dilate)
                    for j in range(1, num_labels):
                        if not self.isOn:
                            break
                        if stats[j][4] < areathreshold:
                            img[labels == j] = 0
            if rbo:
                num_labels, labels, stats, centroids = cv2.connectedComponentsWithStats(img)
                h, w = img.shape
                for j in range(1, num_labels):
                    if not self.isOn:
                        break
                    x, y = centroids[j]
                    if x < left or x > w - right or y < top or y > h - bottom:
                        img[labels == j] = 0
            if auto_iters:
                img = self.autoinpainting(img, auto_iters)
            image_save_path = image_path.replace(root_path, save_dir)
            if not os.path.exists(os.path.dirname(image_save_path)):
                os.makedirs(os.path.dirname(image_save_path))
            img = Image.fromarray(img)
            img.save(image_save_path)
            self.signal[str].emit(f'{processid}/{self.num}: {image_save_path} saved.')
            self.signal[str, str, str].emit(image_path, image_save_path, 'processed_path')

    def autoinpainting(self, img, iters):
        if iters == 0:
            return img
        # 找到roi
        arr = np.where(img == 255)
        x_min, x_max = np.min(arr[1]), np.max(arr[1])
        y_min, y_max = np.min(arr[0]), np.max(arr[0])
        roi = img[y_min:y_max, x_min:x_max]
        roi = cv2.resize(roi, (384, 640))
        roi = cv2.threshold(roi, 127, 1, cv2.THRESH_BINARY)[1]
        roi.resize((1, 1, 640, 384))
        roi = roi.astype(np.float32)

        onnxmodel = onnxruntime.InferenceSession('EUGAN.onnx')

        for i in range(iters):
            roi = onnxmodel.run(None, {'input': roi})[0]
        roi = roi.squeeze().copy()
        roi = cv2.resize(roi, (x_max - x_min, y_max - y_min))
        roi = cv2.threshold(roi, 0.75, 255, cv2.THRESH_BINARY)[1].astype(np.uint8)
        output = np.pad(roi, ((y_min, img.shape[0] - y_max), (x_min, img.shape[1] - x_max)), 'constant')
        output = output * (1 - cv2.dilate(img / 255, np.ones((3, 3), np.uint8), iterations=2))
        output = output.astype(np.uint8)
        output_mask = cv2.dilate(output, np.ones((3, 3), np.uint8), iterations=3)
        output = cv2.add(output, img)
        output = cv2.morphologyEx(output, cv2.MORPH_CLOSE, np.ones((7, 7)), iterations=1)
        output = cv2.dilate(output, np.ones((3, 3)), iterations=1)
        output = cv2.blur(output, (3, 3))
        output = cv2.erode(output, np.ones((3, 3)), iterations=1)
        output = output * (output_mask // 255)
        output = cv2.dilate(output, np.ones((3, 3)), iterations=3)
        output = cv2.blur(output, (9, 9))
        output = cv2.erode(output, np.ones((3, 3)), iterations=2)
        output = cv2.threshold(output, 127, 255, cv2.THRESH_BINARY)[1]
        output = cv2.add(output, img)
        return output


# if __name__ == '__main__':
#     process = ProcessThread()
#     args = {
#         'file_dict': {r'E:\wxrice\org_warp2\0926\10_10_1_1.png': {
#             'binary_path': r'D:\file\qtapp\dataset\test\root\10_10_1_1.png'},
#             r'E:\wxrice\org_warp2\0926\10_10_1_2.png': {
#                 'binary_path': r'D:\file\qtapp\dataset\test\root\10_10_1_2.png'}, },
#         'root_path': r'E:\wxrice\org_warp2\0926',
#         'save_dir': r'D:\file\qtapp\dataset\test\postprocess',
#         'color_plane': 'N',
#         'segmethod': 'OTSU',
#         'threshold': 0,
#         'rso': True,
#         'dilation': 0,
#         'areathreshold': 100,
#         'rbo': False,
#         'left': 0,
#         'right': 0,
#         'top': 0,
#         'bottom': 0,
#     }
#     process.args = args
#     process.run()
