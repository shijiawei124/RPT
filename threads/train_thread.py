import glob
import os
import shutil
import time
import copy
from collections import deque

import numpy as np
import paddle
from PyQt5 import QtCore
from visualdl import LogWriter

from component.base_info import models, base, modeltype
from paddleseg.core.val import evaluate
from paddleseg.cvlibs import Config
from paddleseg.utils import TimeAverager, calculate_eta, resume, logger, worker_init_fn, get_sys_env, \
    config_check


class TrainThread(QtCore.QThread):
    signal = QtCore.pyqtSignal([str, float, int], [bool], [str])

    def __init__(self):
        super(TrainThread, self).__init__()
        self.args = None
        self.task = 'Train'
        self.isOn = True

    def run(self) -> None:
        self.signal[bool].emit(False)
        self.isOn = True
        try:
            self.train(**self.args)
        except Exception as e:
            self.signal[str].emit(str(e))

    def stop(self):
        self.isOn = False
        # self.terminate()
        # self.wait()
        # self.deleteLater()

    def train(self,
              model_type,
              dataset_path,
              learning_rate,
              dataset_split,
              save_dir='output',
              iters=10000,
              batch_size=16,
              resume_model=None,
              save_interval=1000,
              log_iters=10,
              keep_checkpoint_max=5,
              crop_size=512):

        env_info = get_sys_env()
        info = ['{}: {}'.format(k, v) for k, v in env_info.items()]
        info = '\n'.join(['', format('Environment Information', '-^48s')] + info +
                         ['-' * 48])
        logger.info(info)
        self.signal[str].emit(f"[TRAIN]\t{info}")

        config = copy.deepcopy(base)
        save_dir = os.path.join(save_dir, model_type)
        if not os.path.exists(save_dir):
            os.makedirs(save_dir)
        if model_type not in models:
            raise Exception("model is not supported")
        if '_' in model_type:
            model_name, model_scale = model_type.split('_')
        else:
            model_name = model_type
            model_scale = None
        config.update(copy.deepcopy(modeltype[model_name]))

        if model_scale is not None:
            if model_scale in ['resnet50']:
                pass
            elif model_scale == 'resnet101':
                config['model']['backbone']['type'] = 'ResNet101_vd'
                config['model']['backbone'][
                    'pretrained'] = 'https://bj.bcebos.com/paddleseg/dygraph/resnet101_vd_ssld.tar.gz'
            elif model_scale == 'b0':
                config['model']['type'] = 'SegFormer_B0' if model_name == 'segformer' else 'ESegFormer_B0'
            elif model_scale == 'b1':
                config['model']['type'] = 'SegFormer_B1' if model_name == 'segformer' else 'ESegFormer_B1'
                config['model']['pretrained'] = config['model']['pretrained'].replace('b0', 'b1')
            elif model_scale == 'b2':
                config['model']['type'] = 'SegFormer_B2' if model_name == 'segformer' else 'ESegFormer_B2'
                config['model']['pretrained'] = config['model']['pretrained'].replace('b0', 'b2')
            elif model_scale == 'b3':
                config['model']['type'] = 'SegFormer_B3' if model_name == 'segformer' else 'ESegFormer_B3'
                config['model']['pretrained'] = config['model']['pretrained'].replace('b0', 'b3')
            elif model_scale == 'b4':
                config['model']['type'] = 'SegFormer_B4' if model_name == 'segformer' else 'ESegFormer_B4'
                config['model']['pretrained'] = config['model']['pretrained'].replace('b0', 'b4')
            elif model_scale == 'b5':
                config['model']['type'] = 'SegFormer_B5' if model_name == 'segformer' else 'ESegFormer_B5'
                config['model']['pretrained'] = config['model']['pretrained'].replace('b0', 'b5')
            else:
                self.signal[str].emit(f"[TRAIN]\tmodel is not supported")
                raise Exception("model is not supported")
        rec, message = self.check_dataset(dataset_path, split=dataset_split)
        self.signal[str].emit(f"[TRAIN]\t{message}")
        config['train_dataset']['dataset_root'] = dataset_path
        config['val_dataset']['dataset_root'] = dataset_path
        if crop_size != 512:
            config['train_dataset']['transforms'][1]['crop_size'] = [crop_size] * 2

        cfg = Config(config, learning_rate=learning_rate, batch_size=batch_size, iters=iters)
        optimizer = cfg.optimizer
        val_dataset = cfg.val_dataset
        train_dataset = cfg.train_dataset
        if len(train_dataset) == 0:
            self.signal[str].emit("[TRAIN]\tThe length of train_dataset is 0. Please check if your dataset is valid")
            raise ValueError('The length of train_dataset is 0. Please check if your dataset is valid')

        msg = '\n---------------Config Information---------------\n'
        msg += str(cfg)
        msg += '------------------------------------------------'
        logger.info(msg)
        self.signal[str].emit(f"[TRAIN]\t{msg}")
        config_check(cfg, train_dataset=train_dataset, val_dataset=val_dataset)

        # convert bn to sync_bn if necessary
        if paddle.distributed.ParallelEnv().nranks > 1:
            model = paddle.nn.SyncBatchNorm.convert_sync_batchnorm(cfg.model)
        else:
            model = cfg.model

        model.train()
        nranks = paddle.distributed.ParallelEnv().nranks
        local_rank = paddle.distributed.ParallelEnv().local_rank

        start_iter = 0
        if resume_model is not None:
            start_iter = resume(model, optimizer, resume_model)
            self.signal[str].emit(f"[TRAIN]\tResume model from {resume_model}")

        if not os.path.isdir(save_dir):
            if os.path.exists(save_dir):
                os.remove(save_dir)
            os.makedirs(save_dir)

        if nranks > 1:
            paddle.distributed.fleet.init(is_collective=True)
            optimizer = paddle.distributed.fleet.distributed_optimizer(
                optimizer)  # The return is Fleet object
            ddp_model = paddle.distributed.fleet.distributed_model(model)

        batch_sampler = paddle.io.DistributedBatchSampler(
            train_dataset, batch_size=batch_size, shuffle=True, drop_last=True)

        loader = paddle.io.DataLoader(
            train_dataset,
            batch_sampler=batch_sampler,
            num_workers=0,
            return_list=True,
            worker_init_fn=worker_init_fn,
        )

        log_writer = LogWriter(save_dir)

        avg_loss = 0.0
        avg_loss_list = []
        iters_per_epoch = len(batch_sampler)
        best_mean_iou = -1.0
        best_model_iter = -1
        reader_cost_averager = TimeAverager()
        batch_cost_averager = TimeAverager()
        save_models = deque()
        batch_start = time.time()

        iter = start_iter
        self.signal[bool].emit(True)
        while iter < iters:
            for data in loader:
                if not self.isOn:
                    return
                iter += 1
                if iter > iters:
                    version = paddle.__version__
                    if version == '2.1.2':
                        continue
                    else:
                        break
                reader_cost_averager.record(time.time() - batch_start)
                images = data[0]
                labels = data[1].astype('int64')
                edges = None
                if len(data) == 3:
                    edges = data[2].astype('int64')
                if hasattr(model, 'data_format') and model.data_format == 'NHWC':
                    images = images.transpose((0, 2, 3, 1))

                if nranks > 1:
                    logits_list = ddp_model(images)
                else:
                    logits_list = model(images)
                loss_list = self.loss_computation(
                    logits_list=logits_list,
                    labels=labels,
                    losses=cfg.loss,
                    edges=edges)
                loss = sum(loss_list)
                loss.backward()
                optimizer.step()

                lr = optimizer.get_lr()

                # update lr
                if isinstance(optimizer, paddle.distributed.fleet.Fleet):
                    lr_sche = optimizer.user_defined_optimizer._learning_rate
                else:
                    lr_sche = optimizer._learning_rate
                if isinstance(lr_sche, paddle.optimizer.lr.LRScheduler):
                    lr_sche.step()

                model.clear_gradients()
                # avg_loss += loss.numpy()[0]
                avg_loss += loss.numpy()
                if not avg_loss_list:
                    avg_loss_list = [l.numpy() for l in loss_list]
                else:
                    for i in range(len(loss_list)):
                        avg_loss_list[i] += loss_list[i].numpy()
                batch_cost_averager.record(
                    time.time() - batch_start, num_samples=batch_size)

                if (iter) % log_iters == 0 and local_rank == 0:
                    avg_loss /= log_iters
                    # avg_loss_list = [l[0] / log_iters for l in avg_loss_list]
                    avg_loss_list = [l / log_iters for l in avg_loss_list]
                    remain_iters = iters - iter
                    avg_train_batch_cost = batch_cost_averager.get_average()
                    avg_train_reader_cost = reader_cost_averager.get_average()
                    eta = calculate_eta(remain_iters, avg_train_batch_cost)
                    logger.info(info :=
                                "epoch: {}, iter: {}/{}, loss: {:.4f}, lr: {:.6f}, batch_cost: {:.4f}, reader_cost: {:.5f}, ips: {:.4f} samples/sec | ETA {}"
                                .format((iter - 1) // iters_per_epoch + 1, iter, iters,
                                        avg_loss, lr, avg_train_batch_cost,
                                        avg_train_reader_cost,
                                        batch_cost_averager.get_ips_average(), eta))
                    self.signal[str].emit(f"[TRAIN]\t{info}")
                    log_writer.add_scalar('Train/loss', avg_loss, iter)
                    self.signal[str, float, int].emit('Train/loss', avg_loss, iter)
                    # Record all losses if there are more than 2 losses.
                    if len(avg_loss_list) > 1:
                        avg_loss_dict = {}
                        for i, value in enumerate(avg_loss_list):
                            avg_loss_dict['loss_' + str(i)] = value
                        for key, value in avg_loss_dict.items():
                            log_tag = 'Train/' + key
                            log_writer.add_scalar(log_tag, value, iter)

                    log_writer.add_scalar('Train/lr', lr, iter)
                    log_writer.add_scalar('Train/batch_cost',
                                          avg_train_batch_cost, iter)
                    log_writer.add_scalar('Train/reader_cost',
                                          avg_train_reader_cost, iter)
                    avg_loss = 0.0
                    avg_loss_list = []
                    reader_cost_averager.reset()
                    batch_cost_averager.reset()

                if (iter % save_interval == 0 or iter == iters) and (val_dataset is not None):
                    self.signal[str].emit('[EVAL]\tStart evaluating (total_samples: {})...'.format(len(val_dataset)))
                    mean_iou, acc, class_iou, class_acc, kappa = evaluate(
                        model, val_dataset, aug_eval=True, scales=1.0, flip_horizontal=True, flip_vertical=True,
                        is_slide=True, crop_size=[int(crop_size)] * 2, stride=[int(crop_size * 0.75)] * 2)
                    acc = np.mean(class_acc)
                    self.signal[str].emit('[EVAL]\t#Images: {} mIoU: {:.4f} Acc: {:.4f} Kappa: {:.4f} '.format(
                        len(val_dataset), mean_iou, acc, kappa))
                    self.signal[str].emit('[EVAL]\tClass IoU: \n' + str(np.round(class_iou, 4)))
                    self.signal[str].emit('[EVAL]\tClass Acc: \n' + str(np.round(class_acc, 4)))

                    model.train()

                if (iter % save_interval == 0 or iter == iters) and local_rank == 0:
                    current_save_dir = os.path.join(save_dir, "iter_{}".format(iter))
                    if not os.path.isdir(current_save_dir):
                        os.makedirs(current_save_dir)
                    paddle.save(model.state_dict(),
                                os.path.join(current_save_dir, 'model.pdparams'))
                    paddle.save(optimizer.state_dict(),
                                os.path.join(current_save_dir, 'model.pdopt'))
                    save_models.append(current_save_dir)
                    if len(save_models) > keep_checkpoint_max > 0:
                        model_to_remove = save_models.popleft()
                        shutil.rmtree(model_to_remove)

                    if val_dataset is not None:
                        if mean_iou > best_mean_iou:
                            best_mean_iou = mean_iou
                            best_model_iter = iter
                            best_model_dir = os.path.join(save_dir, "best_model")
                            paddle.save(
                                model.state_dict(),
                                os.path.join(best_model_dir, 'model.pdparams'))
                        logger.info(info :=
                                    'The model with the best validation mIoU ({:.4f}) was saved at iter {}.'
                                    .format(best_mean_iou, best_model_iter))
                        self.signal[str].emit(f"[EVAL]\t{info}")
                        log_writer.add_scalar('Evaluate/mIoU', mean_iou, iter)
                        self.signal[str, float, int].emit('Evaluate/mIoU', mean_iou, iter)
                        log_writer.add_scalar('Evaluate/Acc', acc, iter)
                        self.signal[str, float, int].emit('Evaluate/mAcc', acc, iter)
                batch_start = time.time()

        # # Calculate flops.
        # if local_rank == 0:
        #     _, c, h, w = images.shape
        #     _ = paddle.flops(
        #         model, [1, c, h, w],
        #         custom_ops={paddle.nn.SyncBatchNorm: op_flops_funs.count_syncbn})

        # Sleep for half a second to let dataloader release resources.
        time.sleep(0.5)
        log_writer.close()

    def loss_computation(self, logits_list, labels, losses, edges=None):
        loss_list = []
        for i in range(len(logits_list)):
            logits = logits_list[i]
            loss_i = losses['types'][i]
            # Whether to use edges as labels According to loss type.
            if loss_i.__class__.__name__ in ('BCELoss',
                                             'FocalLoss') and loss_i.edge_label:
                loss_list.append(losses['coef'][i] * loss_i(logits, edges))
            elif loss_i.__class__.__name__ in ("KLLoss",):
                loss_list.append(losses['coef'][i] * loss_i(
                    logits_list[0], logits_list[1].detach()))
            else:
                loss_list.append(losses['coef'][i] * loss_i(logits, labels))
        return loss_list

    def get_files(self, path, format, postfix):
        pattern = '*%s.%s' % (postfix, format)

        search_files = os.path.join(path, pattern)
        search_files2 = os.path.join(path, "*", pattern)  # 包含子目录
        search_files3 = os.path.join(path, "*", "*", pattern)  # 包含三级目录

        filenames = glob.glob(search_files)
        filenames2 = glob.glob(search_files2)
        filenames3 = glob.glob(search_files3)

        filenames = filenames + filenames2 + filenames3

        return sorted(filenames)

    def check_dataset(self, dataset_root, split=None):
        if os.path.exists(os.path.join(dataset_root, 'train_list.txt')):
            with open(os.path.join(dataset_root, 'train_list.txt'), 'r') as f:
                train_list = f.read().splitlines()
                if len(train_list) != 0:
                    return True, "train_list.txt is not empty"
        file_list = os.path.join(dataset_root, 'labels.txt')
        with open(file_list, "w") as f:
            for label_class in ['__background__', '__foreground__']:
                f.write(label_class + '\n')

        image_dir = os.path.join(dataset_root, 'images')
        label_dir = os.path.join(dataset_root, 'annotations')
        image_files = self.get_files(image_dir, 'png', '')
        image_files += self.get_files(image_dir, 'jpg', '')
        label_files = self.get_files(label_dir, 'png', '')
        if not image_files:
            return False, f"No files in {image_dir}"
        num_images = len(image_files)

        if not label_files:
            return False, f"No files in {label_dir}"
        num_label = len(label_files)

        if num_images != num_label and num_label > 0:
            return False, f"Number of images = {num_images}    number of labels = {num_label} \n \
                            Number of images is not equal to number of labels.\n \
                            Please check your dataset!"

        image_files = np.array(image_files)
        label_files = np.array(label_files)
        state = np.random.get_state()
        np.random.shuffle(image_files)
        np.random.set_state(state)
        np.random.shuffle(label_files)

        start = 0
        split = [0.8, 0.2, 0] if split is None else split
        num_split = len(split)
        dataset_name = ['train', 'val', 'test']
        for i in range(num_split):
            dataset_split = dataset_name[i]
            print("Creating {}_list.txt...".format(dataset_split))

            file_list = os.path.join(dataset_root, dataset_split + '_list.txt')
            with open(file_list, "w") as f:
                num = round(split[i] * num_images)
                end = start + num
                if i == num_split - 1:
                    end = num_images
                for item in range(start, end):
                    left = image_files[item].replace(dataset_root, '')
                    if left[0] == os.path.sep:
                        left = left.lstrip(os.path.sep)

                    try:
                        right = label_files[item].replace(dataset_root, '')
                        if right[0] == os.path.sep:
                            right = right.lstrip(os.path.sep)
                        line = left + " " + right + '\n'
                    except:
                        line = left + '\n'

                    f.write(line)
                    print(line)
                start = end
        return True, "Create dataset list successfully!"


# if __name__ == '__main__':
#     thread = TrainThread()
#     thread.train('esegformer_b2',
#                  'E:/data/train/rice_dataset',
#                  0.0002,
#                  [0.8, 0.2, 0],
#                  save_dir='E:/data/train/weight',
#                  iters=40000,
#                  batch_size=1,
#                  resume_model=None,
#                  save_interval=100,
#                  log_iters=10,
#                  keep_checkpoint_max=5,
#                  crop_size=512)
