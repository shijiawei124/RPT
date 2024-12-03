import csv
import os.path
import time
import base64
import cv2
import numpy as np
import onnxruntime
import paddle
from PIL import Image
from PyQt5 import QtWidgets
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QImage, QIcon, QPixmap, QFont
from PyQt5.QtWidgets import QMainWindow, QFileDialog, QTreeWidgetItem, QMessageBox
from icon import icon_png
from component.base_info import trained_models, models
from component.mychartview import myChartView
from paddleseg.utils import get_sys_env
from threads.calculate_thread import CalculateThread
from threads.predict_thread import PredictThread
from threads.process_thread import ProcessThread
from threads.train_thread import TrainThread
from threads.warp_thread import WarpThread
from ui.main_UI import MainWindow_UI


# paddle.set_flags({'FLAGS_verbosity': 0})


class MainWindow(QMainWindow, MainWindow_UI):

    def __init__(self):
        super(MainWindow, self).__init__()
        # self.setWindowFlag(QtCore.Qt.FramelessWindowHint)  # 隐藏边框
        self.setup(self)
        self.setuplogit()
        self.setupDefaultValue()
        env_info = get_sys_env()
        self.place = 'gpu' if env_info['Paddle compiled with cuda'] and env_info['GPUs used'] else 'cpu'
        if self.place == 'cpu':
            self.update_textBrowser('Warning: CPU is used, which is slow and not recommended.')
            self.tab_Train.setEnabled(False)
        paddle.set_device(self.place)

    def setuplogit(self):
        self.file_dict = {}  # {'image_path': {'binary_path': None, 'processed_path': None, 'traits': None, 'visualization': None}
        self.trained_model = trained_models
        self.trainable_models = models
        self.dataset_rootpath = None
        self.predict_save_dir = None
        self.warp_save_dir = None
        self.postprocess_save_dir = None
        self.traits_save_dir = None
        self.current_image = {'image_path': None, 'img': None, 'gray': None, 'binary': None, 'thresholdseg': None,
                              'processed': None, 'traits': None, 'visualization': None}
        self.inpaintingMode = False

        # area_Preview
        self.pushButton_DataDir.clicked.connect(self.select_dir_path)
        self.treeWidget_Files.itemDoubleClicked.connect(self.update_current_image)
        self.pushButton_FirstImage.clicked.connect(self.update_current_image)
        self.pushButton_PreviousImage.clicked.connect(self.update_current_image)
        self.pushButton_NextImage.clicked.connect(self.update_current_image)
        self.pushButton_LastImage.clicked.connect(self.update_current_image)
        self.pushButton_FitAllImageView.clicked.connect(self.graphicsView_Main.fitInView)
        self.pushButton_FitAllImageView.clicked.connect(self.graphicsView_Main2.fitInView)
        self.pushButton_FitAllImageView.clicked.connect(self.graphicsView_Main3.fitInView)
        # self.graphicsView_Main.signal.connect(self.update_current_image)

        # self.pushButton_FirstImage.clicked.connect(lambda: self.comboBox_FileList.setCurrentIndex(0))
        # self.pushButton_PreviousImage.clicked.connect(lambda: self.comboBox_FileList.setCurrentIndex(
        #     self.comboBox_FileList.currentIndex() - 1 if self.comboBox_FileList.currentIndex() > 0 else 0))
        # self.pushButton_NextImage.clicked.connect(lambda: self.comboBox_FileList.setCurrentIndex(
        #     self.comboBox_FileList.currentIndex() + 1 if self.comboBox_FileList.currentIndex() < len(
        #         self.file_list) - 1 else self.comboBox_FileList.currentIndex()))
        # self.pushButton_LastImage.clicked.connect(
        #     lambda: self.comboBox_FileList.setCurrentIndex(len(self.file_list) - 1))
        # self.pushButton_ActivateCurrentImage.clicked.connect(self.activate_current_image)

        # tab_Segmentation
        self.pushButton_WeightPath.clicked.connect(self.select_file_path)
        self.pushButton_SaveDir_predict.clicked.connect(self.select_dir_path)
        self.lineEdit_SaveDir_predict.textChanged.connect(self.update_by_lineEdit)
        self.checkBox_IsSlide.stateChanged.connect(self.update_IsSlide)
        self.pushButton_PredictOne.clicked.connect(self.start_segmentation)
        self.pushButton_PredictAll.clicked.connect(self.start_segmentation)
        self.pushButton_StopPredict.clicked.connect(self.stop_thread)
        # warp
        self.pushButton_SaveDir_warp.clicked.connect(self.select_dir_path)
        self.lineEdit_SaveDir_warp.textChanged.connect(self.update_by_lineEdit)
        self.pushButton_ManualWarp.clicked.connect(self.inpainting_mode)
        self.pushButton_Mask.clicked.connect(self.paint_current_image)
        self.pushButton_Paint.clicked.connect(self.paint_current_image)
        self.pushButton_Erase.clicked.connect(self.paint_current_image)
        self.pushButton_Warp.clicked.connect(self.start_warp)
        self.pushButton_WarpAll.clicked.connect(self.start_warp)
        self.pushButton_StopWarp.clicked.connect(self.stop_thread)

        # tab_Postprocess
        self.comboBox_ColorPlane.currentTextChanged.connect(self.update_threshold_segmentation)
        self.comboBox_Method.currentTextChanged.connect(self.update_threshold_segmentation)
        self.doubleSpinBox_Threshold.valueChanged.connect(self.update_threshold_segmentation)
        self.treeWidget_ProcessingConfig.itemChanged.connect(self.update_treeWidget_ProcessingConfig)
        self.treeWidget_ProcessingConfig.itemChanged.connect(self.update_processing)
        self.spinBox_Dilation.valueChanged.connect(self.update_processing)
        self.spinBox_AreaThreshold.valueChanged.connect(self.update_processing)
        self.spinBox_rbo_Left.valueChanged.connect(self.update_processing)
        self.spinBox_rbo_Right.valueChanged.connect(self.update_processing)
        self.spinBox_rbo_Top.valueChanged.connect(self.update_processing)
        self.spinBox_rbo_Bottom.valueChanged.connect(self.update_processing)
        self.pushButton_Inpainting.clicked.connect(self.inpainting_mode)
        # TODO: add auto inpainting
        self.spinBox_AutoInpainting.valueChanged.connect(self.update_processing)
        self.pushButton_SaveDir_postporcess.clicked.connect(self.select_dir_path)
        self.lineEdit_SaveDir_postporcess.textChanged.connect(self.update_by_lineEdit)
        self.pushButton_SaveThisImage.clicked.connect(self.save_processed_image)
        self.pushButton_ProcessingAllImage.clicked.connect(self.start_process)
        self.pushButton_StopProcess.clicked.connect(self.stop_thread)

        # tab_Calculate
        # self.spinBox_LayerHeight.valueChanged.connect(self.pix_ratio_balance)
        # self.spinBox_LayerWidth.valueChanged.connect(self.pix_ratio_balance)
        # self.doubleSpinBox_LayerHeight.valueChanged.connect(self.pix_ratio_balance)
        # self.doubleSpinBox_LayerWidth.valueChanged.connect(self.pix_ratio_balance)
        self.pushButton_SaveDir_cacuate.clicked.connect(self.select_dir_path)
        self.lineEdit_SaveDir_calculate.textChanged.connect(self.update_by_lineEdit)
        self.pushButton_Calculate.clicked.connect(self.start_calculate)
        self.pushButton_CalculateAll.clicked.connect(self.start_calculate)
        self.pushButton_StopCalculate.clicked.connect(self.stop_thread)

        # tab_Train
        self.pushButton_DatasetPath.clicked.connect(self.select_dir_path)
        self.pushButton_SaveDir_train.clicked.connect(self.select_dir_path)
        self.doubleSpinBox_DatasetSplit_train.valueChanged.connect(self.dataset_split_balance)
        self.doubleSpinBox_DatasetSplit_val.valueChanged.connect(self.dataset_split_balance)
        self.pushButton_ResumeModel.clicked.connect(self.select_dir_path)
        self.pushButton_StartTrain.clicked.connect(self.start_train)
        self.pushButton_StopTrain.clicked.connect(self.stop_thread)

    def setupDefaultValue(self):
        self.comboBox_SegmentationModel.addItems(self.trained_model)
        for trainable_model in self.trainable_models:
            self.comboBox_TrainableModel.addItem(trainable_model)

    # def fit_all_image_view(self):
    #     self.graphicsView_Main.fitInView(self.graphicsView_Main.sceneRect(), Qt.KeepAspectRatio)

    # def pix_ratio_balance(self):
    #     sender = self.sender()
    #     if self.current_image['img'] is None:
    #         return
    #     if sender == self.spinBox_LayerHeight:
    #         self.doubleSpinBox_LayerHeight.setValue(
    #             self.spinBox_LayerHeight.value() / self.current_image['img'].shape[0])
    #     elif sender == self.doubleSpinBox_LayerHeight:
    #         self.spinBox_LayerHeight.setValue(
    #             int(self.doubleSpinBox_LayerHeight.value() * self.current_image['img'].shape[0]))
    #     elif sender == self.spinBox_LayerWidth:
    #         self.doubleSpinBox_LayerWidth.setValue(self.spinBox_LayerWidth.value() / self.current_image['img'].shape[1])
    #     elif sender == self.doubleSpinBox_LayerWidth:
    #         self.spinBox_LayerWidth.setValue(
    #             int(self.doubleSpinBox_LayerWidth.value() * self.current_image['img'].shape[1]))

    def update_textBrowser(self, text):
        if self.splitter_Right.sizes()[1] == 0:
            self.splitter_Right.setSizes([1, 1])
        if self.splitter_Middle.sizes()[1] == 0:
            self.splitter_Middle.setSizes([1, 1])
        current_time = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(time.time()))
        self.textBrowser_MessagePrinter.append(current_time + ' >>' + str(text))
        self.textBrowser_MessagePrinter.moveCursor(self.textBrowser_MessagePrinter.textCursor().End)
        self.textBrowser_MessagePrinter.repaint()

    def load_treeWidget_from_dirpath(self, dir_path, treeWidget):
        image_extensions = ('.jpg', '.jpeg', '.png', '.bmp', '.gif')
        for file_name in os.listdir(dir_path):
            file_path = os.path.join(dir_path, file_name)
            if os.path.isdir(file_path):
                dir_item = QTreeWidgetItem(treeWidget)
                dir_item.setText(0, file_name)
                dir_item.setText(1, file_path)
                self.load_treeWidget_from_dirpath(file_path, dir_item)
                if dir_item.childCount() == 0:
                    if type(treeWidget) == QtWidgets.QTreeWidget:
                        treeWidget.takeTopLevelItem(treeWidget.indexOfTopLevelItem(dir_item))
                    else:
                        treeWidget.removeChild(dir_item)
            elif os.path.isfile(file_path) and file_path.endswith(image_extensions):
                # self.file_list.append(file_path)
                self.file_dict[file_path] = {'binary_path': None, 'processed_path': None, 'traits': None,
                                             'visualization': None}
                item = QTreeWidgetItem(treeWidget)
                item.setText(0, os.path.basename(file_path))
                item.setText(1, file_path)

    def select_dir_path(self):
        dir_path = QFileDialog.getExistingDirectory(self, 'Open dir')
        if dir_path == '':
            return
        sender = self.sender()
        if sender == self.pushButton_SaveDir_train:
            self.lineEdit_SaveDir_train.setText(dir_path)
        elif sender == self.pushButton_SaveDir_predict:
            self.lineEdit_SaveDir_predict.setText(dir_path)
        elif sender == self.pushButton_DatasetPath:
            self.lineEdit_DatasetPath.setText(dir_path)
        elif sender == self.pushButton_ResumeModel:
            self.lineEdit_ResumeModel.setText(dir_path)
        elif sender == self.pushButton_DataDir:
            self.dataset_rootpath = dir_path
            self.label_DataDirShow.setText(dir_path)
            # self.file_list = []
            self.file_dict = {}
            self.treeWidget_Files.clear()
            self.load_treeWidget_from_dirpath(dir_path, self.treeWidget_Files)
            self.treeWidget_Files.expandAll()
            self.current_image = {'image_path': None, 'img': None, 'thresholdseg': None, 'gray': None,
                                  'processed': None, 'binary': None, 'traits': None, 'visualization': None}
            self.update_file_dict(self.lineEdit_SaveDir_predict)
            self.update_file_dict(self.lineEdit_SaveDir_postporcess)
            self.update_file_dict(self.lineEdit_SaveDir_calculate)
            self.update_current_image()
            self.inpainting_mode('quit')
        elif sender == self.pushButton_SaveDir_postporcess:
            self.lineEdit_SaveDir_postporcess.setText(dir_path)
        elif sender == self.pushButton_SaveDir_cacuate:
            self.lineEdit_SaveDir_calculate.setText(dir_path)
        elif sender == self.pushButton_SaveDir_warp:
            self.lineEdit_SaveDir_warp.setText(dir_path)

    def select_file_path(self):
        file_path = QFileDialog.getOpenFileName(self, 'Open file', './')
        if file_path[0] == '':
            return
        sender = self.sender()
        if sender == self.pushButton_WeightPath:
            self.lineEdit_WeighPath.setText(file_path[0])

    def update_by_lineEdit(self):
        sender = self.sender()
        if sender == self.lineEdit_SaveDir_predict:
            self.predict_save_dir = self.lineEdit_SaveDir_predict.text()
        elif sender == self.lineEdit_SaveDir_postporcess:
            self.postprocess_save_dir = self.lineEdit_SaveDir_postporcess.text()
        elif sender == self.lineEdit_SaveDir_calculate:
            self.traits_save_dir = self.lineEdit_SaveDir_calculate.text()
        elif sender == self.lineEdit_SaveDir_warp:
            self.warp_save_dir = self.lineEdit_SaveDir_warp.text()
        self.update_file_dict(sender)

    def update_file_dict(self, sender=None):
        if self.dataset_rootpath == None:
            return
        if sender == self.lineEdit_SaveDir_calculate:
            num = 0
            if self.traits_save_dir == None:
                return
            self.update_textBrowser('try to update traits')
            try:
                with open(os.path.join(self.traits_save_dir, 'traits.csv'), 'r') as f:
                    csvreader = csv.DictReader(f)
                    for row in csvreader:
                        if row['image_path'] in self.file_dict.keys():
                            self.file_dict[row['image_path']]['traits'] = row
                            num += 1
                            if row['image_path'] == self.current_image['image_path']:
                                self.current_image['traits'] = row
                                self.update_traits_table()
                self.update_textBrowser(f'update traits success: {num}, total: {len(self.file_dict)}')
            except Exception as e:
                if Exception == FileNotFoundError:
                    self.update_textBrowser(f"can't find traits.csv in {self.traits_save_dir}")
                else:
                    self.update_textBrowser(f'update traits failed: {e}')
            self.update_textBrowser('try to update traits visualization')
            try:
                num = 0
                for root, dirs, files in os.walk(os.path.join(self.traits_save_dir, 'traits_visualization')):
                    for file in files:
                        if file.endswith('.png'):
                            file_path = os.path.join(root, file)
                            image_path = file_path.replace(os.path.join(self.traits_save_dir, 'traits_visualization'),
                                                           self.dataset_rootpath)
                            if image_path in self.file_dict.keys():
                                self.file_dict[image_path]['visualization'] = file_path
                                num += 1
                                if image_path == self.current_image['image_path']:
                                    self.current_image['visualization'] = file_path
                                    self.update_current_image()
                self.update_textBrowser(f'update visualization success: {num}, total: {len(self.file_dict)}')
            except Exception as e:
                self.update_textBrowser(f'update visualization failed: {e}')
            return

        if sender == self.lineEdit_SaveDir_predict:
            dir_path = self.predict_save_dir
        elif sender == self.lineEdit_SaveDir_postporcess:
            dir_path = self.postprocess_save_dir
        else:
            dir_path = None
        if dir_path == None or dir_path == '':
            return
        path_type = 'processed_path' if sender == self.lineEdit_SaveDir_postporcess else 'binary_path'
        self.update_textBrowser(f'try to update {path_type.split("_")[0]} images')
        try:
            num = 0
            for root, dirs, files in os.walk(dir_path):
                for file in files:
                    if file.endswith('.png'):
                        file_path = os.path.join(root, file)
                        image_path = file_path.replace(dir_path, self.dataset_rootpath)
                        if image_path in self.file_dict.keys():
                            self.file_dict[image_path][path_type] = file_path
                            num += 1
            self.update_textBrowser(f'update {path_type} success: {num}, total: {len(self.file_dict)}')
            if self.current_image['thresholdseg'] is None and self.current_image['gray'] is None and self.current_image[
                'processed'] is None:
                self.update_current_image()
        except Exception as e:
            if e == TypeError:
                self.update_textBrowser(f'update {path_type} error: please select save dir')
            else:
                self.update_textBrowser(f'update {path_type} error: {e}')

    def inpainting_mode(self, mode=None):
        sender = self.sender()
        if self.dataset_rootpath is None:
            self.update_textBrowser('please select dataset rootpath')
            self.inpaintingMode = True
        if mode == 'quit' or 'Quit' in sender.text():
            self.inpaintingMode = True
        if self.inpaintingMode:
            self.graphicsView_Main3.setDrawMode(False)
            self.graphicsView_Main2.setDrawMode(False)
            self.graphicsView_Main.setDrawMode(False)
            self.pushButton_Mask.setVisible(False)
            self.pushButton_Paint.setVisible(False)
            self.pushButton_Erase.setVisible(False)
            self.pushButton_Inpainting.setText('Inpainting')
            self.pushButton_ManualWarp.setText('manual')
            self.inpaintingMode = False
            return
        if sender == self.pushButton_Inpainting:
            if self.current_image['binary'] is None and mode is None:
                QMessageBox.warning(self, 'Warning', 'Please segment first!')
                return
            if self.current_image['processed'] is None:
                self.current_image['processed'] = self.current_image['binary'] if self.current_image['binary'] else \
                    self.current_image['thresholdseg']
                self.update_graphicsview(self.current_image['processed'], self.graphicsView_Main3)
            self.splitter_Right.setSizes([1, 0])
            self.graphicsView_Main3.setDrawMode(True)
            self.pushButton_Inpainting.setText('Quit Inpainting')
            self.pushButton_Paint.setVisible(True)
            self.pushButton_Erase.setVisible(True)
            self.inpaintingMode = True
        elif sender == self.pushButton_ManualWarp:
            if self.current_image['image_path'] is None:
                QMessageBox.warning(self, 'Warning', 'Please select one image!')
                return
            if self.current_image['binary'] is None:
                self.current_image['binary'] = np.zeros(self.current_image['img'].shape[:2], dtype=np.uint8)
                self.update_graphicsview(self.current_image['binary'], self.graphicsView_Main2)
            self.graphicsView_Main.setDrawMode(True)
            self.graphicsView_Main2.setDrawMode(True)
            self.pushButton_ManualWarp.setText('Quit')
            self.pushButton_Mask.setVisible(True)
            self.pushButton_Paint.setVisible(True)
            self.pushButton_Erase.setVisible(True)

    def paint_current_image(self):
        sender = self.sender()
        if self.pushButton_Inpainting.text() == 'Quit Inpainting':
            save_dir = self.postprocess_save_dir
            if save_dir is None:
                QMessageBox.warning(self, 'Warning', 'Please select a save dir!')
                return
            graphicsView = self.graphicsView_Main3
            img = self.current_image['processed'] if self.current_image['processed'] is not None else \
                self.current_image['binary']
        elif self.pushButton_ManualWarp.text() == 'Quit':
            if self.predict_save_dir is None:
                self.update_textBrowser('please select a predict save dir')
                return
            save_dir = self.predict_save_dir
            if sender == self.pushButton_Mask:
                graphicsView = self.graphicsView_Main
                if self.current_image['binary']:
                    img = self.current_image['binary']
                else:
                    img = np.zeros(self.current_image['img'].shape[:2], dtype=np.uint8)
            else:
                graphicsView = self.graphicsView_Main2
                img = self.current_image['binary']
        save_path = self.current_image['image_path'].replace(self.dataset_rootpath, save_dir)
        polygons = graphicsView.get_polygons()
        if len(polygons) == 0:
            return
        color = 0 if sender == self.pushButton_Erase else 255
        for polygon in polygons:
            cv2.fillPoly(img, [np.array(polygon)], color)
        if self.pushButton_ManualWarp.text() == 'Quit':
            self.update_graphicsview(img, self.graphicsView_Main2, fitInView=False)
            self.current_image['binary'] = img
        if self.pushButton_Inpainting.text() == 'Quit Inpainting':
            self.update_graphicsview(img, self.graphicsView_Main3, fitInView=False)
            self.current_image['processed'] = img
        if not os.path.exists(os.path.dirname(save_path)):
            os.makedirs(os.path.dirname(save_path))
        Image.fromarray(img).save(save_path)
        self.update_textBrowser(f'saved at {save_path}')

    def autoinpainting(self, img):
        if self.spinBox_AutoInpainting.value() == 0:
            return
        times = self.spinBox_AutoInpainting.value()
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

        for i in range(times):
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

    def update_current_image(self):
        if hasattr(self, 'myChartView_loss'):
            self.horizontalLayout_imageview.removeWidget(self.chartview_loss)
            self.horizontalLayout_imageview.removeWidget(self.chartview_acc)
            self.horizontalLayout_imageview.removeWidget(self.chartview_iou)
            self.chartview_loss.deleteLater()
            self.chartview_acc.deleteLater()
            self.chartview_iou.deleteLater()
            self.graphicsView_Main.setVisible(True)
            # self.horizontalLayout_imageview.setStretch(0, 0)
            # self.horizontalLayout_imageview.setStretch(1, 0)
            # self.horizontalLayout_imageview.setStretch(2, 0)
        sender = self.sender()
        current_image = self.current_image['image_path']
        if current_image is None and sender != self.treeWidget_Files:
            return
        if sender == self.pushButton_FirstImage:
            current_image = list(self.file_dict.keys())[0]
        elif sender == self.pushButton_LastImage:
            current_image = list(self.file_dict.keys())[-1]
        elif sender == self.pushButton_NextImage:
            if self.current_image['image_path']:
                index = list(self.file_dict.keys()).index(self.current_image['image_path'])
                if -1 < index < len(self.file_dict) - 1:
                    current_image = list(self.file_dict.keys())[index + 1]
        elif sender == self.pushButton_PreviousImage:
            if self.current_image['image_path']:
                index = list(self.file_dict.keys()).index(self.current_image['image_path'])
                if index > 0:
                    current_image = list(self.file_dict)[index - 1]
        elif sender == self.treeWidget_Files:
            if self.treeWidget_Files.currentItem().childCount() == 0:
                current_image = self.treeWidget_Files.currentItem().text(1)
        if current_image:
            self.current_image = {'image_path': None, 'img': None, 'gray': None, 'binary': None, 'thresholdseg': None,
                                  'processed': None, 'traits': None, 'visualization': None}
            self.current_image['image_path'] = current_image
            self.current_image['img'] = np.array(Image.open(current_image))
            if self.file_dict[current_image]['binary_path']:
                self.current_image['binary'] = np.array(
                    Image.open(self.file_dict[current_image]['binary_path']).convert('L'))
            if self.file_dict[current_image]['processed_path']:
                self.current_image['processed'] = np.array(
                    Image.open(self.file_dict[current_image]['processed_path']).convert('L'))
            if self.file_dict[current_image]['traits']:
                self.current_image['traits'] = self.file_dict[current_image]['traits']
            if self.file_dict[current_image]['visualization']:
                self.current_image['visualization'] = self.file_dict[current_image]['visualization']
            self.label_ImageSize.setText(
                f'image size: {self.current_image["img"].shape[0]}x{self.current_image["img"].shape[1]}')
            self.label_CurrentImage.setText(current_image)
            self.update_graphicsview(current_image)
            self.update_traits_table()

    def update_graphicsview(self, image_path, graphicsView=None, fitInView=True):
        if graphicsView:
            graphicsView.setVisible(True)
            images = [image_path]
        else:
            images = [image_path, self.file_dict[image_path]['binary_path'],
                      self.file_dict[image_path]['processed_path']]
            graphicsViews = [self.graphicsView_Main, self.graphicsView_Main2, self.graphicsView_Main3]
            for i, image in enumerate(images):
                if image is not None:
                    graphicsViews[i].clean_items()
                    graphicsViews[i].setVisible(True)
                else:
                    graphicsViews[i].setVisible(False)
        for i, image in enumerate(images):
            if i == 0:
                graphicsView = self.graphicsView_Main if not graphicsView else graphicsView
            elif i == 2:
                graphicsView = self.graphicsView_Main3
                if self.current_image['visualization'] is not None:
                    image = self.current_image['visualization']
            else:
                graphicsView = self.graphicsView_Main2
            if image is not None:
                if isinstance(image, str):
                    image = QImage(image)
                elif isinstance(image, np.ndarray):
                    if len(image.shape) == 3:
                        image = QImage(image.data, image.shape[1], image.shape[0], QImage.Format_RGB888)
                    else:
                        image = QImage(image.data, image.shape[1], image.shape[0], QImage.Format_Grayscale8)
                else:
                    raise TypeError('image type not supported')
                # if not graphicsView.isVisible():
                #     graphicsView.setVisible(True)
                # graph_scene = QGraphicsScene()
                # image = image.scaled(graphicsView.width() - 3, graphicsView.height() - 3, Qt.KeepAspectRatio)
                # graph_scene.addPixmap(image)
                # graph_scene.update()
                # graphicsView.setScene(graph_scene)
                graphicsView.setPixmap(image, fitInView=fitInView)
                if graphicsView == self.graphicsView_Main:
                    self.label_ImageSize.setText(f'image size: {image.width()}x{image.height()}')
            # else:
            #     if graphicsView.isVisible():
            #         graphicsView.setVisible(False)

    def dataset_split_balance(self):
        if self.doubleSpinBox_DatasetSplit_train.value() + self.doubleSpinBox_DatasetSplit_val.value() == 1:
            pass
        if self.sender() == self.doubleSpinBox_DatasetSplit_train:
            self.doubleSpinBox_DatasetSplit_val.setValue(1 - self.doubleSpinBox_DatasetSplit_train.value())
        elif self.sender() == self.doubleSpinBox_DatasetSplit_val:
            self.doubleSpinBox_DatasetSplit_train.setValue(1 - self.doubleSpinBox_DatasetSplit_val.value())

    def update_IsSlide(self):
        if self.checkBox_IsSlide.isChecked():
            self.spinBox_CropSize_predict.setEnabled(True)
            self.spinBox_Stride.setEnabled(True)
        else:
            self.spinBox_CropSize_predict.setEnabled(False)
            self.spinBox_Stride.setEnabled(False)

    '''
    def update_process(self):
        if self.current_image['image_path'] is None or os.path.exists(self.current_image['image_path']) is False:
            if self.comboBox_ColorPlane.currentIndex() != 0:
                self.update_textBrowser('please select an existing image!')
            self.comboBox_ColorPlane.setCurrentIndex(0)
            return
        if self.current_image['img'] is None:
            self.current_image['img'] = Image.open(self.current_image['image_path'])
        sender = self.sender()
        if sender == self.comboBox_ColorPlane:
            colorplane = self.comboBox_ColorPlane.currentText()[0]
            if colorplane == 'N':
                self.current_image['gray'] = None
                self.current_image['thresholdseg'] = None
                self.comboBox_Method.setCurrentIndex(0)
                self.comboBox_Method.setEnabled(False)
                self.doubleSpinBox_Threshold.setEnabled(False)
                self.update_graphicsview(self.current_image['image_path'])
            else:
                self.comboBox_Method.setEnabled(True)
            if colorplane in 'RGB':
                self.current_image['gray'] = self.current_image['img'].convert('RGB')['RGB'.index(colorplane)]
            elif colorplane in 'HSV':
                self.current_image['gray'] = self.current_image['img'].convert('HSV')['HSV'.index(colorplane)]
        if sender == self.comboBox_Method:
            method = self.comboBox_Method.currentText()
            if method == 'None':
                self.doubleSpinBox_Threshold.setEnabled(False)
                img = None
                self.update_graphicsview(self.current_image['gray'], self.graphicsView_Main2)
            else:
                img = np.array(self.current_image['gray'])
            if method == 'manual threshold':
                self.doubleSpinBox_Threshold.setEnabled(True)
                threshold = self.doubleSpinBox_Threshold.value()
                img[img <= threshold] = 0
                img[img > threshold] = 255
            elif method == 'Adaptive threshold(OTSU)':
                self.doubleSpinBox_Threshold.setEnabled(False)
                threshold, img = cv2.threshold(img, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
                self.doubleSpinBox_Threshold.setValue(threshold)
    '''

    def update_threshold_segmentation(self):
        if self.current_image['image_path'] is None or os.path.exists(self.current_image['image_path']) is False:
            if self.comboBox_ColorPlane.currentIndex() != 0:
                self.update_textBrowser('please select an existing image!')
            self.comboBox_ColorPlane.setCurrentIndex(0)
            return
        colorplane = self.comboBox_ColorPlane.currentText()[0]
        if self.sender() == self.comboBox_ColorPlane:
            if colorplane == 'N':
                self.comboBox_Method.setCurrentIndex(0)
                self.comboBox_Method.setEnabled(False)
                self.doubleSpinBox_Threshold.setEnabled(False)
                self.update_graphicsview(self.current_image['image_path'])
                self.current_image['img'] = None
                self.current_image['gray'] = None
                self.current_image['thresholdseg'] = None
                self.current_image['porcessed'] = None
                return
            else:
                if self.current_image['img'] is None:
                    self.current_image['img'] = Image.open(self.current_image['image_path'])
                elif isinstance(self.current_image['img'], np.ndarray):
                    self.current_image['img'] = Image.fromarray(self.current_image['img'])
            self.comboBox_Method.setEnabled(True)
            if colorplane in ['R', 'G', 'B']:
                img = self.current_image['img'].convert('RGB')
                self.current_image['gray'] = img.split()[['R', 'G', 'B'].index(colorplane)]
            elif colorplane in ['H', 'S', 'V']:
                img = self.current_image['img'].convert('HSV')
                self.current_image['gray'] = img.split()[['H', 'S', 'V'].index(colorplane)]

        method = self.comboBox_Method.currentText()
        img = np.array(self.current_image['gray'])
        if method == 'manual threshold':
            self.doubleSpinBox_Threshold.setEnabled(True)
            threshold = self.doubleSpinBox_Threshold.value()
            img[img <= threshold] = 0
            img[img > threshold] = 255
            img = cv2.medianBlur(img, 5)
            img = cv2.medianBlur(img, 5)
            img = cv2.medianBlur(img, 5)
            self.current_image['thresholdseg'] = img
        elif method == 'Adaptive threshold(OTSU)':
            self.doubleSpinBox_Threshold.setEnabled(False)
            threshold, img = cv2.threshold(img, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
            img = cv2.medianBlur(img, 5)
            img = cv2.medianBlur(img, 5)
            img = cv2.medianBlur(img, 5)
            self.doubleSpinBox_Threshold.setValue(threshold)
            self.current_image['thresholdseg'] = img
        # elif method == 'None':
        #     self.doubleSpinBox_Threshold.setValue(0.00)
        #     self.doubleSpinBox_Threshold.setEnabled(False)
        #     self.current_image['thresholdseg'] = None
        self.graphicsView_Main2.setVisible(True)
        self.graphicsView_Main2.setPixmap(QImage(img.data, img.shape[1], img.shape[0], QImage.Format_Grayscale8))

    def update_treeWidget_ProcessingConfig(self):
        if self.trWItem_rso.checkState(1) == Qt.Checked:
            self.trWItem_rso.setExpanded(True)
        else:
            self.trWItem_rso.setExpanded(False)
        if self.trWItem_rbo.checkState(1) == Qt.Checked:
            self.trWItem_rbo.setExpanded(True)
        else:
            self.trWItem_rbo.setExpanded(False)

    def update_traits_table(self):
        traits = self.current_image['traits']
        if traits is None:
            self.traits_init()
            return
        self.trWItem_area.setText(1, str(traits['area']))
        self.trWItem_convex_area.setText(1, str(traits['convex_area']))
        self.trWItem_length.setText(1, str(traits['length']))
        self.trWItem_diameter.setText(1, str(traits['diameter']))
        self.trWItem_depth.setText(1, str(traits['depth']))
        self.trWItem_width.setText(1, str(traits['width']))
        self.trWItem_wdRatio.setText(1, str(traits['wd_ratio']))
        self.trWItem_sturdiness.setText(1, str(traits['sturdiness']))
        self.trWItem_initial_x.setText(1, str(traits['initial_x']))
        self.trWItem_initial_y.setText(1, str(traits['initial_y']))
        self.trWItem_centroid_x.setText(1, str(traits["centroid_x"]))
        self.trWItem_centroid_y.setText(1, str(traits['centroid_y']))
        self.trWItem_angle_apex_left.setText(1, str(traits['apex_angle_left']))
        self.trWItem_angle_apex_right.setText(1, str(traits['apex_angle_right']))
        self.trWItem_angle_apex_all.setText(1, str(traits['apex_angle']))
        self.trWItem_angle_entire_left.setText(1, str(traits['entire_angle_left']))
        self.trWItem_angle_entire_right.setText(1, str(traits['entire_angle_right']))
        self.trWItem_angle_entire_all.setText(1, str(traits['entire_angle']))
        self.trWItem_lmchild_Area.setText(1, str(traits['layer_mass_A']))
        self.trWItem_lmchild_Length.setText(1, str(traits['layer_mass_L']))
        self.trWItem_lmchild_Convex_hull.setText(1, str(traits['layer_mass_C']))
        self.trWItem_lmchild_A_C.setText(1, str(traits['layer_mass_A_C']))
        self.trWItem_lmchild_A_L.setText(1, str(traits['layer_mass_A_L']))
        self.trWItem_lmchild_L_C.setText(1, str(traits['layer_mass_L_C']))

    def update_processing(self):
        self.treeWidget_ProcessingConfig.repaint()
        sender = self.sender()
        if sender == self.spinBox_AreaThreshold:
            self.spinBox_rbo_Left.setEnabled(False)
            self.spinBox_rbo_Top.setEnabled(False)
            self.spinBox_rbo_Right.setEnabled(False)
            self.spinBox_rbo_Bottom.setEnabled(False)
            self.spinBox_Dilation.setEnabled(False)
            self.trWItem_rso.child(1).setText(1, str(self.spinBox_AreaThreshold.value()))
            return
        elif sender == self.spinBox_Dilation:
            self.spinBox_rbo_Left.setEnabled(False)
            self.spinBox_rbo_Top.setEnabled(False)
            self.spinBox_rbo_Right.setEnabled(False)
            self.spinBox_rbo_Bottom.setEnabled(False)
            self.spinBox_AreaThreshold.setEnabled(False)
            self.trWItem_rso.child(0).setText(1, str(self.spinBox_Dilation.value()))
            return
        elif sender == self.spinBox_rbo_Left:
            self.spinBox_Dilation.setEnabled(False)
            self.spinBox_rbo_Top.setEnabled(False)
            self.spinBox_rbo_Right.setEnabled(False)
            self.spinBox_rbo_Bottom.setEnabled(False)
            self.spinBox_AreaThreshold.setEnabled(False)
            self.trWItem_rbo.child(0).setText(1, str(self.spinBox_rbo_Left.value()))
            return
        elif sender == self.spinBox_rbo_Top:
            self.spinBox_rbo_Left.setEnabled(False)
            self.spinBox_Dilation.setEnabled(False)
            self.spinBox_rbo_Right.setEnabled(False)
            self.spinBox_rbo_Bottom.setEnabled(False)
            self.spinBox_AreaThreshold.setEnabled(False)
            self.trWItem_rbo.child(1).setText(1, str(self.spinBox_rbo_Top.value()))
            return
        elif sender == self.spinBox_rbo_Right:
            self.spinBox_rbo_Left.setEnabled(False)
            self.spinBox_rbo_Top.setEnabled(False)
            self.spinBox_Dilation.setEnabled(False)
            self.spinBox_rbo_Bottom.setEnabled(False)
            self.spinBox_AreaThreshold.setEnabled(False)
            self.trWItem_rbo.child(2).setText(1, str(self.spinBox_rbo_Right.value()))
            return
        elif sender == self.spinBox_rbo_Bottom:
            self.spinBox_rbo_Left.setEnabled(False)
            self.spinBox_rbo_Top.setEnabled(False)
            self.spinBox_Dilation.setEnabled(False)
            self.spinBox_rbo_Right.setEnabled(False)
            self.spinBox_AreaThreshold.setEnabled(False)
        dilate_times = self.spinBox_Dilation.value()
        areathreshold = self.spinBox_AreaThreshold.value()
        left = self.spinBox_rbo_Left.value()
        right = self.spinBox_rbo_Right.value()
        top = self.spinBox_rbo_Top.value()
        bottom = self.spinBox_rbo_Bottom.value()
        try:
            binary_path = self.file_dict[self.current_image['image_path']]['binary_path']
            img = self.current_image['thresholdseg'].copy() if binary_path is None else binary_path
        except:
            img = None
        if isinstance(img, str):
            img = np.array(Image.open(img), np.uint8)
        elif img is None:
            self.spinBox_Dilation.setEnabled(True)
            self.spinBox_AreaThreshold.setEnabled(True)
            self.spinBox_rbo_Left.setEnabled(True)
            self.spinBox_rbo_Top.setEnabled(True)
            self.spinBox_rbo_Right.setEnabled(True)
            self.spinBox_rbo_Bottom.setEnabled(True)
            return
        if self.trWItem_rso.checkState(1) == Qt.Checked:
            dilate = cv2.dilate(img, np.ones((3, 3), np.uint8), iterations=dilate_times)
            num_labels, labels, stats, centroids = cv2.connectedComponentsWithStats(dilate, connectivity=8)
            a_array = stats[:, 4].reshape(-1)
            if areathreshold == -1:
                area_m = np.mean(np.sort(a_array)[:-1]) * 0.5
            else:
                area_m = areathreshold
            for index, a in enumerate(a_array):
                if a < area_m:
                    img[labels == index] = 0
        if self.trWItem_rbo.checkState(1) == Qt.Checked:
            num_labels, labels, stats, centroids = cv2.connectedComponentsWithStats(img, connectivity=8)
            h, w = img.shape
            for i, [x, y] in enumerate(centroids):
                if x < left or x > w - right or y < top or y > h - bottom:
                    img[labels == i] = 0
        img_inpainting = self.autoinpainting(img.copy())
        img_inpainting_show = cv2.merge([img_inpainting, img, img])
        self.current_image['processed'] = img_inpainting
        self.graphicsView_Main3.setVisible(True)
        qimg = QImage(img_inpainting_show.data, img.shape[1], img.shape[0], QImage.Format_RGB888)
        self.graphicsView_Main3.setPixmap(qimg)
        self.spinBox_Dilation.setEnabled(True)
        self.spinBox_AreaThreshold.setEnabled(True)
        self.spinBox_rbo_Left.setEnabled(True)
        self.spinBox_rbo_Top.setEnabled(True)
        self.spinBox_rbo_Right.setEnabled(True)
        self.spinBox_rbo_Bottom.setEnabled(True)

    def save_processed_image(self):
        if self.current_image['processed'] is not None:
            save_img = self.current_image['processed']
            if self.postprocess_save_dir is None or self.postprocess_save_dir == '':
                self.update_textBrowser('Please select the save directory!')
                return
            save_path = self.current_image['image_path'].replace(self.dataset_rootpath, self.postprocess_save_dir)
        else:
            save_img = self.current_image['thresholdseg']
            if self.postprocess_save_dir is None or self.postprocess_save_dir == '':
                self.update_textBrowser('Please select the save directory on the Predict tab!')
                return
            save_path = self.current_image['image_path'].replace(self.dataset_rootpath, self.postprocess_save_dir)
        if save_img is None:
            self.update_textBrowser('Please process the image first!')
            return
        if not os.path.exists(os.path.dirname(save_path)):
            os.makedirs(os.path.dirname(save_path))
        img = Image.fromarray(save_img)
        img.save(save_path)
        self.update_textBrowser('Save image to {}'.format(save_path))
        if self.current_image['processed'] is not None:
            self.file_dict[self.current_image['image_path']]['processed_path'] = save_path
        else:
            self.file_dict[self.current_image['image_path']]['binary_path'] = save_path
        self.update_current_image()

    def setabled(self, mode, value):
        if mode == 'Predict':
            self.widget_Predict.setEnabled(value)
            self.pushButton_PredictAll.setEnabled(value)
            self.pushButton_PredictOne.setEnabled(value)
            self.pushButton_Inpainting.setEnabled(value)
            self.tab_Train.setEnabled(value)
            self.tab_PostProcessing.setEnabled(value)
            self.tab_Analysis.setEnabled(value)
            self.groupBox_Warp.setEnabled(value)
        elif mode == 'Warp':
            self.spinBox_TargetWidth.setEnabled(value)
            self.spinBox_TargetHeight.setEnabled(value)
            self.lineEdit_SaveDir_warp.setEnabled(value)
            self.pushButton_SaveDir_warp.setEnabled(value)
            self.pushButton_ManualWarp.setEnabled(value)
            self.pushButton_Warp.setEnabled(value)
            self.pushButton_WarpAll.setEnabled(value)
            self.pushButton_PredictAll.setEnabled(value)
            self.pushButton_PredictOne.setEnabled(value)
            self.pushButton_StopPredict.setEnabled(value)
            self.tab_PostProcessing.setEnabled(value)
            self.tab_Analysis.setEnabled(value)
            self.tab_Train.setEnabled(value)
        elif mode == 'Train':
            self.treeWidget_Train.setEnabled(value)
            self.pushButton_StartTrain.setEnabled(value)
            self.pushButton_Inpainting.setEnabled(value)
            self.tab_Predict.setEnabled(value)
            self.tab_PostProcessing.setEnabled(value)
            self.tab_Analysis.setEnabled(value)
        elif mode == 'Process':
            self.groupBox_ImageProcessing.setEnabled(value)
            self.groupBox_ThresholdSegmentation.setEnabled(value)
            self.pushButton_SaveThisImage.setEnabled(value)
            self.pushButton_ProcessingAllImage.setEnabled(value)
            self.tab_Predict.setEnabled(value)
            self.tab_Train.setEnabled(value)
            self.tab_Predict.setEnabled(value)
            self.tab_Analysis.setEnabled(value)
        elif mode == 'Calculate':
            self.gridLayout_LayerSize.setEnabled(value)
            self.treeWidget_Traits.setEnabled(value)
            self.horizontalLayout_SacveDir_calculate.setEnabled(value)
            self.pushButton_Calculate.setEnabled(value)
            self.pushButton_CalculateAll.setEnabled(value)
            self.pushButton_Inpainting.setEnabled(value)
            self.tab_Predict.setEnabled(value)
            self.tab_Train.setEnabled(value)
            self.tab_PostProcessing.setEnabled(value)

    def show_chart(self, data, type):
        if type == 'init':
            self.graphicsView_Main.setVisible(False)
            self.graphicsView_Main2.setVisible(False)
            self.graphicsView_Main3.setVisible(False)
            if hasattr(self, 'myChartView_loss'):
                self.myChartView_loss.clearchart()
            else:
                self.myChartView_loss = myChartView()
                self.myChartView_loss.chart().setTitle('Loss')
                self.horizontalLayout_imageview.addWidget(self.myChartView_loss)
            if hasattr(self, 'myChartView_acc'):
                self.myChartView_acc.clearchart()
            else:
                self.myChartView_acc = myChartView()
                self.myChartView_acc.chart().setTitle('mAcc')
                self.horizontalLayout_imageview.addWidget(self.myChartView_acc)
            if hasattr(self, 'myChartView_iou'):
                self.myChartView_iou.clearchart()
            else:
                self.myChartView_iou = myChartView()
                self.myChartView_iou.chart().setTitle('mIoU')
                self.horizontalLayout_imageview.addWidget(self.myChartView_iou)
            self.horizontalLayout_imageview.setStretch(3, 1)
            self.horizontalLayout_imageview.setStretch(4, 1)
            self.horizontalLayout_imageview.setStretch(5, 1)
            self.label_CurrentImage.setText('Training')
            self.label_ImageSize.setText('')
        elif type == 'loss':
            self.myChartView_loss.append(data[0], data[1])
        elif type == 'acc':
            self.myChartView_acc.append(data[0], data[1])
        elif type == 'iou':
            self.myChartView_iou.append(data[0], data[1])
        elif type == 'delete':
            self.update_current_image()

    def start_train(self):
        if self.place != 'gpu':
            self.update_textBrowser('Training only supports GPU mode')
            return
        args = {}
        args['model_type'] = self.comboBox_TrainableModel.currentText()
        args['iters'] = self.spinBox_Iters.value()
        args['batch_size'] = self.spinBox_BatchSize.value()
        args['learning_rate'] = self.doubleSpinBox_Lr.value()
        args['dataset_path'] = self.lineEdit_DatasetPath.text()
        args['dataset_split'] = [self.doubleSpinBox_DatasetSplit_train.value(),
                                 self.doubleSpinBox_DatasetSplit_val.value(), 0]
        args['save_dir'] = self.lineEdit_SaveDir_train.text()
        args['resume_model'] = self.lineEdit_ResumeModel.text() if self.lineEdit_ResumeModel.text() else None
        args['log_iters'] = self.spinBox_LogIters.value()
        args['save_interval'] = self.spinBox_SaveInterval.value()
        args['keep_checkpoint_max'] = self.spinBox_KeepCheckpointMax.value()
        args['crop_size'] = self.spinBox_CropSize_train.value()
        for key, value in args.items():
            if str(value) == '' or value == 0:
                QMessageBox.warning(self, 'Warning', 'Please fill in {}!'.format(key))
                return
            elif sum(args['dataset_split']) != 1:
                QMessageBox.warning(self, 'Warning', 'Please check dataset split!')
                return
        self.label_statubar.setText('Training...')
        self.update_textBrowser('Start training...')
        self.setabled('Train', False)
        self.show_chart(None, 'init')
        # train_thread
        self.train_thread = TrainThread()
        self.pushButton_StopTrain.clicked.connect(self.train_thread.stop)
        self.train_thread.signal[str].connect(self.update_textBrowser)
        self.train_thread.signal[str, float, int].connect(self.update_train_log)
        self.train_thread.signal[bool].connect(self.setabled_stop)
        self.train_thread.finished.connect(self.thread_finished)
        self.train_thread.args = args
        self.train_thread.start()

    def start_segmentation(self):
        args = {}
        args['model_type'] = self.comboBox_SegmentationModel.currentText()
        args['model_path'] = self.lineEdit_WeighPath.text()
        if self.sender() == self.pushButton_PredictOne:
            args['image_path'] = self.current_image['image_path']
            if args['image_path'] == '' or args['image_path'] is None:
                QMessageBox.warning(self, 'Warning', 'Please select one image!')
                return
            self.progressBar_statubar.setMaximum(1)

        else:
            args['image_path'] = self.dataset_rootpath
            self.progressBar_statubar.setMaximum(len(self.file_dict))
        args['save_dir'] = self.lineEdit_SaveDir_predict.text()
        args['is_slide'] = self.checkBox_IsSlide.isChecked()
        args['crop_size'] = self.spinBox_CropSize_predict.value()
        args['stride'] = self.spinBox_Stride.value()
        args['image_rootpath'] = self.dataset_rootpath
        for key, value in args.items():
            if str(value) == '' or value == 0:
                QMessageBox.warning(self, 'Warning', 'Please fill in {}!'.format(key))
                return
        self.label_statubar.setText('Predicting...')
        self.setabled('Predict', False)
        self.graphicsView_Main2.setVisible(True)
        self.progressBar_statubar.setValue(0)
        self.predict_thread = PredictThread()
        self.pushButton_StopPredict.clicked.connect(self.predict_thread.stop)
        self.predict_thread.signal[str].connect(self.update_textBrowser)
        self.predict_thread.signal[str, str, str].connect(self.finish_one_image)
        self.predict_thread.signal[bool].connect(self.setabled_stop)
        self.predict_thread.finished.connect(self.thread_finished)
        self.predict_thread.args = args
        self.predict_thread.start()

    def start_warp(self):
        args = {}
        if self.sender() == self.pushButton_Warp:
            if self.current_image['image_path'] is None:
                QMessageBox.warning(self, 'Warning', 'Please select one image!')
                return
            args['file_dict'] = {self.current_image['image_path']: self.file_dict[self.current_image['image_path']]}
            num = 1
        else:
            args['file_dict'] = self.file_dict
            # 统计file_dict中每个值字典中'binary_path'不为None的个数
            num = 0
            for key, value in self.file_dict.items():
                if value['binary_path'] is not None:
                    num += 1
        args['root_path'] = self.dataset_rootpath
        args['save_dir'] = self.lineEdit_SaveDir_warp.text()
        args['target_width'] = self.spinBox_TargetWidth.value()
        args['target_height'] = self.spinBox_TargetHeight.value()
        if self.lineEdit_SaveDir_warp.text() == '':
            QMessageBox.warning(self, 'Warning', 'Please fill in save dir!')
            return
        if self.spinBox_TargetWidth.value() == 0 or self.spinBox_TargetHeight.value() == 0:
            QMessageBox.warning(self, 'Warning', 'Please fill in target size!')
            return
        self.label_statubar.setText('Warping...')
        self.setabled('Warp', False)
        self.graphicsView_Main3.setVisible(True)
        self.progressBar_statubar.setMaximum(num)
        self.progressBar_statubar.setValue(0)
        self.warp_thread = WarpThread()
        self.pushButton_StopWarp.clicked.connect(self.warp_thread.stop)
        self.warp_thread.signal[str, np.ndarray].connect(self.update_warp_img)
        self.warp_thread.signal[str].connect(self.update_textBrowser)
        self.warp_thread.signal[str, str, str].connect(self.finish_one_image)
        self.warp_thread.finished.connect(self.thread_finished)
        self.warp_thread.args = args
        self.warp_thread.num = num
        self.warp_thread.start()

    def start_process(self):
        args = {}
        if self.sender() == self.pushButton_SaveThisImage:
            if self.current_image['image_path'] is None:
                QMessageBox.warning(self, 'Warning', 'Please select one image!')
                return
            args['file_dict'] = {self.current_image['image_path']: self.file_dict[self.current_image['image_path']]}
            num = 1
        else:
            args['file_dict'] = self.file_dict
            # 统计file_dict中每个值字典中'binary_path'不为None的个数
            num = 0
            for key, value in self.file_dict.items():
                if value['binary_path'] is not None:
                    num += 1
        args['root_path'] = self.dataset_rootpath
        args['save_dir'] = self.lineEdit_SaveDir_postporcess.text()
        args['color_plane'] = self.comboBox_ColorPlane.currentText()[0]
        args['segmethod'] = self.comboBox_Method.currentText()
        args['threshold'] = self.doubleSpinBox_Threshold.value()
        args['rso'] = False if self.trWItem_rso.checkState(1) == Qt.Unchecked else True
        args['dilation'] = self.spinBox_Dilation.value()
        args['areathreshold'] = self.spinBox_AreaThreshold.value()
        args['rbo'] = False if self.trWItem_rbo.checkState(1) == Qt.Unchecked else True
        args['left'] = self.spinBox_rbo_Left.value()
        args['right'] = self.spinBox_rbo_Right.value()
        args['top'] = self.spinBox_rbo_Top.value()
        args['bottom'] = self.spinBox_rbo_Bottom.value()
        args['auto_iters'] = self.spinBox_AutoInpainting.value()
        if args['save_dir'] == '':
            QMessageBox.warning(self, 'Warning', 'Please fill in save dir!')
            return
        self.label_statubar.setText('Processing...')
        self.setabled('Process', False)
        self.graphicsView_Main3.setVisible(True)
        self.progressBar_statubar.setValue(0)
        self.progressBar_statubar.setMaximum(num)
        self.process_thread = ProcessThread()
        self.pushButton_StopProcess.clicked.connect(self.process_thread.stop)
        self.process_thread.signal[str].connect(self.update_textBrowser)
        self.process_thread.signal[str, str, str].connect(self.finish_one_image)
        self.process_thread.finished.connect(self.thread_finished)
        self.process_thread.args = args
        self.process_thread.num = num
        self.process_thread.start()

    def start_calculate(self):
        args = {}
        if self.sender() == self.pushButton_Calculate:
            if self.current_image['image_path'] is None:
                QMessageBox.warning(self, 'Warning', 'Please select one image!')
                return
            if self.current_image['processed'] is None and self.current_image['binary'] is None:
                QMessageBox.warning(self, 'Warning', 'Please process the image first!')
                return
            args['file_dict'] = {self.current_image['image_path']: self.file_dict[self.current_image['image_path']]}

            num = 1
        else:
            args['file_dict'] = self.file_dict
            # 统计file_dict中每个值字典中'binary_path'不为None的个数
            num = 0
            for key, value in self.file_dict.items():
                if value['processed_path'] is not None or value['binary_path'] is not None:
                    num += 1
        args['save_path'] = self.lineEdit_SaveDir_calculate.text()
        args['dataset_rootpath'] = self.postprocess_save_dir
        args['Layer_height'] = self.spinBox_LayerHeight.value() if self.spinBox_LayerHeight.value() else None
        args['Layer_width'] = self.spinBox_LayerWidth.value() if self.spinBox_LayerWidth.value() else None
        if len(args['file_dict']) == 0:
            QMessageBox.warning(self, 'Warning', 'Please load image!')
            return
        if args['save_path'] == '':
            QMessageBox.warning(self, 'Warning', 'Please fill in save dir!')
            return
        self.label_statubar.setText('Calculating...')
        self.setabled('Calculate', False)
        self.progressBar_statubar.setValue(0)
        self.progressBar_statubar.setMaximum(num)
        self.calculate_thread = CalculateThread()
        self.pushButton_StopCalculate.clicked.connect(self.calculate_thread.stop)
        self.calculate_thread.signal[str].connect(self.update_textBrowser)
        self.calculate_thread.signal[dict].connect(self.calculate_one_image)
        self.calculate_thread.signal[str, str, str, np.ndarray].connect(self.finish_one_image)
        self.calculate_thread.finished.connect(self.thread_finished)
        self.calculate_thread.args = args
        self.calculate_thread.num = num
        self.calculate_thread.start()

    def setabled_stop(self, flag):
        sender = self.sender()
        if sender.task == 'Train':
            self.pushButton_StopTrain.setEnabled(flag)
        elif sender.task == 'Predict':
            self.pushButton_StopPredict.setEnabled(flag)

    def update_progress(self, progress, maximum):
        if self.progressBar_statubar.maximum() != maximum:
            self.progressBar_statubar.setMaximum(maximum)
        self.progressBar_statubar.setValue(progress)

    def update_train_log(self, key, value, iter_num):
        if key == 'Train/loss':
            self.update_progress(iter_num, self.spinBox_Iters.value())
            self.show_chart([iter_num, value], 'loss')
        elif key == 'Evaluate/mAcc':
            self.show_chart([iter_num, value], 'acc')
        elif key == 'Evaluate/mIoU':
            self.show_chart([iter_num, value], 'iou')

    def update_warp_img(self, image_path, array):
        if image_path == 'warped':
            self.update_graphicsview(array, self.graphicsView_Main3)
        else:
            self.current_image['image_path'] = image_path
            self.update_current_image()
            self.graphicsView_Main.set_a_polygon(array)
            self.graphicsView_Main2.set_a_polygon(array)

    def finish_one_image(self, image_path, new_path, type, img=None):
        self.update_progress(self.progressBar_statubar.value() + 1, self.progressBar_statubar.maximum())
        if type == 'warp':
            return
        self.file_dict[image_path][type] = new_path
        if type == 'binary_path':
            self.file_dict[image_path]['processed_path'] = None
            self.file_dict[image_path]['traits'] = None
        elif type == 'processed_path':
            self.file_dict[image_path]['traits'] = None
        self.current_image['image_path'] = image_path
        self.current_image['visualization'] = img
        self.label_CurrentImage.setText(image_path)
        self.update_graphicsview(image_path)

    def calculate_one_image(self, traits):
        self.update_progress(self.progressBar_statubar.value() + 1, self.progressBar_statubar.maximum())
        self.current_image['image_path'] = traits['image_path']
        self.current_image['traits'] = traits
        self.file_dict[self.current_image['image_path']]['traits'] = traits
        self.update_graphicsview(self.current_image['image_path'])
        self.update_traits_table()

    def stop_thread(self):
        if self.label_statubar.text() in ['Done', 'Ready']:
            return
        self.label_statubar.setText('Stop')

    def thread_finished(self):
        sender = self.sender()  # type TrainThread or PredictThread
        sender.terminate()
        sender.wait()
        sender.deleteLater()
        self.setabled(sender.task, True)

        sender.finished.disconnect()
        if self.label_statubar.text() != 'Stop':
            self.label_statubar.setText('Done')
        self.update_textBrowser(sender.task + ' ' + self.label_statubar.text())


if __name__ == '__main__':
    import qdarkstyle
    import sys
    from PyQt5.QtWidgets import QApplication

    # sys.stdout重定向，指向文件对象
    locol_time = time.strftime("%Y-%m-%d %H-%M-%S", time.localtime())
    log_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'log', locol_time + '.txt')
    if not os.path.exists(os.path.dirname(log_path)):
        os.makedirs(os.path.dirname(log_path))
    logfile = open(log_path, 'w')
    sys.stdout = logfile
    sys.stderr = logfile

    app = QApplication(sys.argv)
    app.setStyleSheet(qdarkstyle.load_stylesheet(qt_api='pyqt5'))

    font = QFont("Arial", 12)
    app.setFont(font)

    mainwindow = MainWindow()
    Logo = QPixmap()
    Logo.loadFromData(base64.b64decode(icon_png))
    icon_img = QIcon()
    icon_img.addPixmap(Logo, QIcon.Normal, QIcon.Off)
    mainwindow.setWindowIcon(icon_img)
    mainwindow.show()
    sys.exit(app.exec_())
