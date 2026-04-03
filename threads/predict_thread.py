import os
import math
from PIL import Image
import numpy as np
import paddle
import copy
from PyQt5 import QtCore
import cv2
from mpmath.identification import transforms

import paddleseg
from paddleseg import transforms as T
from paddleseg import utils
from paddleseg.core import infer
from paddleseg.utils import logger, progbar, get_image_list
from component.base_info import base, modeltype
from paddleseg.cvlibs import Config


class PredictThread(QtCore.QThread):
    # [str, str, str]: image_path, binary_path, 'binary_path'
    # [bool]: switch of Stop button
    # [str]: log message
    signal = QtCore.pyqtSignal([str, str, str], [bool], [str])

    def __init__(self):
        super(PredictThread, self).__init__()
        self.args = None
        self.task = 'Predict'
        self.isOn = False

    def run(self) -> None:
        self.signal[bool].emit(False)
        self.isOn = True
        try:
            device = 'cpu'

            if 'gpu_id' in self.args:
                preferred_gpu_id = self.args['gpu_id']

                if preferred_gpu_id >= 0 and paddle.is_compiled_with_cuda():
                    test_device = f'gpu:{preferred_gpu_id}'
                    try:
                        paddle.set_device(test_device)
                        device = test_device
                        self.signal[str].emit(f"[INFO] Successfully using {device} for inference.")
                    except Exception as e1:
                        # 如果设置失败，则尝试 fallback 到 cpu
                        self.signal[str].emit(f"[WARNING] Failed to use GPU {preferred_gpu_id}: {e1}. Falling back to CPU.")
                        device = 'cpu'
                elif preferred_gpu_id == -1:
                    self.signal[str].emit("[INFO] Using CPU as specified by gpu_id.")
                else:
                    self.signal[str].emit("[INFO] CUDA not supported. Using CPU.")
            else:
                self.signal[str].emit("[INFO] No gpu_id provided, defaulting to CPU.")

            # 确保最终设备被设置（即使上面已设，再设一次也无妨）
            paddle.set_device(device)

            # 去除参数gpu_id
            predict_args = {k: v for k, v in self.args.items() if k != 'gpu_id'}

            self.predict(**predict_args)
        except Exception as e:
            import traceback
            self.signal[str].emit(f"[ERROR] {str(e)}\n{traceback.format_exc()}")
        #     if paddle.is_compiled_with_cuda():
        #         # 你可以指定想用的 GPU 编号（例如 0, 1）
        #         preferred_gpu_id = self.args['gpu_id']
        #
        #         try:
        #             # 直接尝试设置目标 GPU
        #             test_device = f'gpu:{preferred_gpu_id}'
        #             paddle.set_device(test_device)
        #             device = test_device
        #             self.signal[str].emit(f"[INFO] Successfully using {device} for inference.")
        #         except Exception as e1:
        #             # 尝试 fallback 到 gpu:0（如果 preferred 不是 0）
        #             if preferred_gpu_id != 0:
        #                 try:
        #                     paddle.set_device('gpu:0')
        #                     device = 'gpu:0'
        #                     self.signal[str].emit(f"[INFO] Preferred GPU {preferred_gpu_id} unavailable, using gpu:0.")
        #                 except Exception as e2:
        #                     self.signal[str].emit(f"[WARNING] Failed to use any GPU: {e1}, {e2}. Falling back to CPU.")
        #                     device = 'cpu'
        #             else:
        #                 self.signal[str].emit(f"[WARNING] Failed to use GPU: {e1}. Falling back to CPU.")
        #                 device = 'cpu'
        #     else:
        #         self.signal[str].emit("[INFO] CUDA not supported. Using CPU.")
        #
        #     # 确保最终设备被设置（即使上面已设，再设一次也无妨）
        #     paddle.set_device(device)
        #
        #     self.predict(**self.args)
        # except Exception as e:
        #     import traceback
        #     self.signal[str].emit(f"[ERROR] {str(e)}\n{traceback.format_exc()}")

    def stop(self):
        self.isOn = False
        # self.terminate()
        # self.wait()
        # self.deleteLater()

    def predict(self, model_type, image_path, model_path, save_dir, is_slide, crop_size, stride, image_rootpath):
        if '_' in model_type:
            model_name, model_scale = model_type.split('_')
        else:
            model_name = model_type
            model_scale = None
        config = copy.deepcopy(base)
        config.update(copy.deepcopy(modeltype[model_name]))
        config['model']['pretrained'] = None
        if model_scale is not None:
            if model_scale in ['resnet50']:
                pass
            elif model_scale == 'resnet101':
                config['model']['backbone']['type'] = 'ResNet101_vd'
                # config['model']['backbone'][
                #     'pretrained'] = 'https://bj.bcebos.com/paddleseg/dygraph/resnet101_vd_ssld.tar.gz'
            elif model_scale == 'b0':
                config['model']['type'] = 'SegFormer_B0' if model_name == 'segformer' else 'ESegFormer_B0'
            elif model_scale == 'b1':
                config['model']['type'] = 'SegFormer_B1' if model_name == 'segformer' else 'ESegFormer_B1'
                # config['model']['pretrained'] = config['model']['pretrained'].replace('b0', 'b1')
            elif model_scale == 'b2':
                config['model']['type'] = 'SegFormer_B2' if model_name == 'segformer' else 'ESegFormer_B2'
                # config['model']['pretrained'] = config['model']['pretrained'].replace('b0', 'b2')
            elif model_scale == 'b3':
                config['model']['type'] = 'SegFormer_B3' if model_name == 'segformer' else 'ESegFormer_B3'
                # config['model']['pretrained'] = config['model']['pretrained'].replace('b0', 'b3')
            elif model_scale == 'b4':
                config['model']['type'] = 'SegFormer_B4' if model_name == 'segformer' else 'ESegFormer_B4'
                # config['model']['pretrained'] = config['model']['pretrained'].replace('b0', 'b4')
            elif model_scale == 'b5':
                config['model']['type'] = 'SegFormer_B5' if model_name == 'segformer' else 'ESegFormer_B5'
                # config['model']['pretrained'] = config['model']['pretrained'].replace('b0', 'b5')
            else:
                raise Exception("model is not supported")
        cfg = Config(config)

        self.model = cfg.model
        self.transforms = paddleseg.transforms.Normalize()
        self.image_list, self.image_dir = get_image_list(image_path)
        logger.info('Number of predict images = {}'.format(len(self.image_list)))
        self.signal[str].emit('[SEG]\tNumber of predict images = {}'.format(len(self.image_list)))

        utils.utils.load_entire_model(self.model, model_path)
        self.model.eval()
        nranks = paddle.distributed.get_world_size()
        local_rank = paddle.distributed.get_rank()
        if nranks > 1:
            n = int(math.ceil(len(self.image_list) / float(nranks)))
            img_lists = [self.image_list[i:i + n] for i in range(0, len(self.image_list), n)]
        else:
            img_lists = [self.image_list]

        logger.info("Start to predict...")
        self.signal[str].emit('[SEG]\tStart to predict...')
        progbar_pred = progbar.Progbar(target=len(img_lists[0]), verbose=1)
        self.signal[bool].emit(True)
        with paddle.no_grad():
            for i, im_path in enumerate(img_lists[local_rank]):
                if not self.isOn:
                    return

                # 图像读取
                # im = cv2.imread(im_path)
                img = Image.open(im_path).convert('RGB')
                # img = cv2.cvtColor(np.array(img), cv2.COLOR_RGB2BGR)
                img = np.array(img)
                # im = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
                ori_shape = img.shape[:2]

                # 预处理
                im = self.transforms(img)[0]
                im = im[np.newaxis, ...]
                im = paddle.to_tensor(im)
                im = im.transpose([0, 3, 1, 2])

                # 图像推理
                pred = infer.aug_inference(
                    self.model,
                    im,
                    ori_shape=ori_shape,
                    transforms=[self.transforms],
                    scales=1.0,
                    flip_horizontal=False,
                    flip_vertical=False,
                    is_slide=is_slide,
                    stride=[stride] * 2,
                    crop_size=[crop_size] * 2,
                    isOn=self.isOn)
                if not self.isOn:
                    return

                # 保存结果
                pred = paddle.squeeze(pred)
                pred = pred.numpy().astype('uint8')

                # 保存黑白图
                save_pred = pred.copy()
                save_pred[save_pred == 1] = 255

                binary_path = im_path.replace(image_rootpath, save_dir)
                if not os.path.exists(binary_dir := os.path.dirname(binary_path)):
                    os.makedirs(binary_dir)
                save_img = Image.fromarray(save_pred)
                save_img.save(binary_path, compress_level=0)

                self.signal[str, str, str].emit(im_path, binary_path, 'binary_path')
                self.signal[str].emit('[SEG]\t{}/{} image: {} saved'.format(i + 1, len(img_lists[0]), binary_path))
                progbar_pred.update(i + 1)

# if __name__ == '__main__':
#     args = {
#         'model_type': 'esegformer_b2',
#         'image_path': r'E:/data/process/image\\date2\\2_18_4_7.png',
#         'model_path': 'E:/data/model_weight/Esegformerb2/best_model/model.pdparams',
#         'save_dir': 'E:/data/process/root',
#         'is_slide': True,
#         'crop_size': 1024,
#         'stride': 768,
#         'image_rootpath': 'E:/data/process/image'
#     }
#     pre = PredictThread()
#     pre.args = args
#     pre.isOn = True
#     pre.predict(**args)
