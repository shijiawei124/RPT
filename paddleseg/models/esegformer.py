# The SegFormer code was heavily based on https://github.com/NVlabs/SegFormer
# Users should be careful about adopting these functions in any commercial matters.
# https://github.com/NVlabs/SegFormer#license
import cv2
import paddle
import paddle.nn as nn
import paddle.nn.functional as F
import numpy as np

from paddleseg.cvlibs import manager
from paddleseg.models import layers
from paddleseg.utils import utils


class MLP(nn.Layer):
    """
    Linear Embedding
    """

    def __init__(self, input_dim=2048, embed_dim=768):
        super().__init__()
        self.proj = nn.Linear(input_dim, embed_dim)

    def forward(self, x):
        x = x.flatten(2).transpose([0, 2, 1])
        x = self.proj(x)
        return x


@manager.MODELS.add_component
class ESegFormer(nn.Layer):
    """
    The ESegFormer implementation based on PaddlePaddle.

    Args:
        num_classes (int): The unique number of target classes.
        backbone (Paddle.nn.Layer): A backbone network.
        embedding_dim (int): The MLP decoder channel dimension.
        align_corners (bool): An argument of F.interpolate. It should be set to False when the output size of feature.
            is even, e.g. 1024x512, otherwise it is True, e.g. 769x769. Default: False.
        pretrained (str, optional): The path or url of pretrained model. Default: None.
    """

    def __init__(self,
                 backbone,
                 embedding_dim,
                 num_classes=2,
                 align_corners=False,
                 pretrained=None):
        super(ESegFormer, self).__init__()

        self.pretrained = pretrained
        self.align_corners = align_corners
        self.backbone = backbone
        self.num_classes = num_classes
        c1_in_channels, c2_in_channels, c3_in_channels, c4_in_channels = self.backbone.feat_channels
        self.linear_c5 = MLP(input_dim=256, embed_dim=embedding_dim)
        self.linear_c4 = MLP(input_dim=c4_in_channels, embed_dim=embedding_dim)
        self.linear_c3 = MLP(input_dim=c3_in_channels, embed_dim=embedding_dim)
        self.linear_c2 = MLP(input_dim=c2_in_channels, embed_dim=embedding_dim)
        self.linear_c1 = MLP(input_dim=c1_in_channels, embed_dim=embedding_dim)

        self.dropout = nn.Dropout2D(0.1)
        self.linear_fuse = layers.ConvBNReLU(
            in_channels=embedding_dim * 5,
            out_channels=embedding_dim,
            kernel_size=1,
            bias_attr=False)

        self.linear_pred = nn.Conv2D(
            embedding_dim, self.num_classes, kernel_size=1)

        self.init_weight()

    def init_weight(self):
        if self.pretrained is not None:
            utils.load_entire_model(self, self.pretrained)

    def freeze_exceptEAM(self):
        # 遍历所有参数，将所有参数的stop_gradient属性设置为True、trainable属性设置为False
        for name, param in self.named_parameters():
            param.trainable = False
            param.stop_gradient = True
        # 解冻EAM的参数
        for name, param in self.linear_c5.named_parameters():
            param.trainable = True
            param.stop_gradient = False
        for name, param in self.linear_fuse.named_parameters():
            param.trainable = True
            param.stop_gradient = False
        for name, param in self.linear_pred.named_parameters():
            param.trainable = True
            param.stop_gradient = False
        for name, param in self.dropout.named_parameters():
            param.trainable = True
            param.stop_gradient = False
        for name, param in self.backbone.spatial_attn.named_parameters():
            param.trainable = True
            param.stop_gradient = False

    def forward(self, x):
        feats = self.backbone(x)
        c5, c1, c2, c3, c4 = feats

        ############## MLP decoder on C1-C4 ###########
        c1_shape = paddle.shape(c1)
        c2_shape = paddle.shape(c2)
        c3_shape = paddle.shape(c3)
        c4_shape = paddle.shape(c4)
        c5_shape = paddle.shape(c5)

        _c5 = self.linear_c5(c5).transpose([0, 2, 1]).reshape(
            [0, 0, c5_shape[2], c5_shape[3]])

        _c4 = self.linear_c4(c4).transpose([0, 2, 1]).reshape(
            [0, 0, c4_shape[2], c4_shape[3]])
        _c4 = F.interpolate(
            _c4,
            size=c1_shape[2:],
            mode='bilinear',
            align_corners=self.align_corners)

        _c3 = self.linear_c3(c3).transpose([0, 2, 1]).reshape(
            [0, 0, c3_shape[2], c3_shape[3]])
        _c3 = F.interpolate(
            _c3,
            size=c1_shape[2:],
            mode='bilinear',
            align_corners=self.align_corners)

        _c2 = self.linear_c2(c2).transpose([0, 2, 1]).reshape(
            [0, 0, c2_shape[2], c2_shape[3]])
        _c2 = F.interpolate(
            _c2,
            size=c1_shape[2:],
            mode='bilinear',
            align_corners=self.align_corners)

        _c1 = self.linear_c1(c1).transpose([0, 2, 1]).reshape(
            [0, 0, c1_shape[2], c1_shape[3]])

        _c = self.linear_fuse(paddle.concat([_c5, _c4, _c3, _c2, _c1], axis=1))

        logit = self.dropout(_c)
        logit = self.linear_pred(logit)
        return [
            F.interpolate(
                logit,
                size=paddle.shape(x)[2:],
                mode='bilinear',
                align_corners=self.align_corners)
        ]


@manager.MODELS.add_component
def ESegFormer_B0(**kwargs):
    return ESegFormer(
        backbone=manager.BACKBONES['EMixVisionTransformer_B0_'](),
        embedding_dim=256,
        **kwargs)


@manager.MODELS.add_component
def ESegFormer_B1(**kwargs):
    return ESegFormer(
        backbone=manager.BACKBONES['EMixVisionTransformer_B1_'](),
        embedding_dim=256,
        **kwargs)


@manager.MODELS.add_component
def ESegFormer_B2(**kwargs):
    return ESegFormer(
        backbone=manager.BACKBONES['EMixVisionTransformer_B2_'](),
        embedding_dim=768,
        **kwargs)


@manager.MODELS.add_component
def ESegFormer_B3(**kwargs):
    return ESegFormer(
        backbone=manager.BACKBONES['EMixVisionTransformer_B3_'](),
        embedding_dim=768,
        **kwargs)


@manager.MODELS.add_component
def ESegFormer_B4(**kwargs):
    return ESegFormer(
        backbone=manager.BACKBONES['EMixVisionTransformer_B4_'](),
        embedding_dim=768,
        **kwargs)


@manager.MODELS.add_component
def ESegFormer_B5(**kwargs):
    return ESegFormer(
        backbone=manager.BACKBONES['EMixVisionTransformer_B5_'](),
        embedding_dim=768,
        **kwargs)


if __name__ == '__main__':
    paddle.summary(ESegFormer_B2(num_classes=2), (1, 3, 512, 512))
