trained_models = ['esegformer_b0', 'esegformer_b1', 'esegformer_b2', 'esegformer_b3', 'esegformer_b4', 'esegformer_b5',
                  'deeplabv3_resnet50', 'deeplabv3_resnet101', 'deeplabv3p_resnet50', 'deeplabv3p_resnet101', 'hardnet',
                  'pspnet_resnet50', 'pspnet_resnet101', 'segformer_b0', 'segformer_b1', 'segformer_b2', 'segformer_b3',
                  'segformer_b4', 'segformer_b5', 'segnet', 'unet', 'unet_3plus', 'unet_plusplus']

models = ['esegformer_b0', 'esegformer_b1', 'esegformer_b2', 'esegformer_b3', 'esegformer_b4', 'esegformer_b5',
          'deeplabv3_resnet50', 'deeplabv3_resnet101', 'deeplabv3p_resnet50', 'deeplabv3p_resnet101', 'hardnet',
          'pspnet_resnet50', 'pspnet_resnet101', 'segformer_b0', 'segformer_b1', 'segformer_b2', 'segformer_b3',
          'segformer_b4', 'segformer_b5', 'segnet', 'unet', 'unet_3plus', 'unet_plusplus']

base = {
    'batch_size': 1,
    'iters': 40000,
    'train_dataset': {
        'type': 'RootSeg',
        'dataset_root': None,
        'transforms': [
            {'type': 'ResizeStepScaling',
             'min_scale_factor': 0.5,
             'max_scale_factor': 2.0,
             'scale_step_size': 0.25,
             },
            {'type': 'RandomPaddingCrop',
             'crop_size': [512, 512],
             'label_padding_value': 0},
            {'type': 'RandomHorizontalFlip'},
            {'type': 'RandomBlur',
             'prob': 0.1},
            {'type': 'RandomDistort',
             'brightness_range': 0.4,
             'contrast_range': 0.4,
             'saturation_range': 0.4},
            {'type': 'RandomNoise'},
            {'type': 'Normalize'}],
        'mode': 'train'},
    'val_dataset':
        {'type': 'RootSeg',
         'dataset_root': None,
         'transforms': [{'type': 'Normalize'}],
         'mode': 'val'},
    'optimizer':
        {'type': 'sgd',
         'momentum': 0.9,
         'weight_decay': 4e-05},
    'lr_scheduler':
        {'type': 'PolynomialDecay',
         'learning_rate': 0.01,
         'end_lr': 0,
         'power': 0.9,
         },
    'loss':
        {'types': [{'type': 'CrossEntropyLoss'}],
         'coef': [1]},
    'model':
        {'num_classes': 2,
         'pretrained': None},
    'test_config': {
        'aug_eval': True,
        'flip_horizontal': True,
        'flip_vertical': True,
        'is_slide': True,
        'crop_size': [1024, 1024],
        'stride': [768, 768]},
}

modeltype = {
    'esegformer': {
        'model': {
            'type': 'ESegFormer_B0',
            'pretrained': 'https://bj.bcebos.com/paddleseg/dygraph/mix_vision_transformer_b0.tar.gz',
        },
        'optimizer': {
            'type': 'AdamW',
            'beta1': 0.9,
            'beta2': 0.999,
            'weight_decay': 0.01,
        },
        'lr_scheduler': {
            'type': 'CosineAnnealingDecay',
            'learning_rate': 6e-05,
            'T_max': 40000,
        },
        'loss': {
            'types':
                [{'type': 'MixedLoss', 'losses': [{'type': 'CrossEntropyLoss'}, {'type': 'DiceLoss'}], 'coef': [1, 1]}],
            'coef': [1],
        },
    },
    'deeplabv3': {
        'model': {
            'type': 'DeepLabV3',
            'backbone': {
                'type': 'ResNet50_vd',
                'output_stride': 8,
                'multi_grid': [1, 2, 4],
                'pretrained': 'https://bj.bcebos.com/paddleseg/dygraph/resnet50_vd_ssld_v2.tar.gz',
            },
            'backbone_indices': [3],
            'aspp_ratios': [1, 12, 24, 36],
            'aspp_out_channels': 256,
            'align_corners': False,
        },
    },
    'deeplabv3p': {
        'model': {
            'type': 'DeepLabV3P',
            'backbone': {
                'type': 'ResNet50_vd',
                'output_stride': 8,
                'multi_grid': [1, 2, 4],
                'pretrained': 'https://bj.bcebos.com/paddleseg/dygraph/resnet50_vd_ssld_v2.tar.gz',
            },
            'backbone_indices': [0, 3],
            'aspp_ratios': [1, 12, 24, 36],
            'aspp_out_channels': 256,
            'align_corners': False,
        },
    },
    'hardnet': {
        'model': {
            'type': 'HarDNet'},
        'lr_scheduler': {'type': 'PolynomialDecay',
                         'learning_rate': 0.02},
        'optimizer': {'type': 'sgd',
                      'momentum': 0.9,
                      'weight_decay': 0.0005,
                      },
        'loss': {
            'types': [{
                'type': 'BootstrappedCrossEntropyLoss',
                'min_K': 4096,
                'loss_th': 0.3,
            }],
            'coef': [1],
        },
    },
    'pspnet': {
        'model': {
            'type': 'PSPNet',
            'backbone': {
                'type': 'ResNet50_vd',
                'pretrained': 'https://bj.bcebos.com/paddleseg/dygraph/resnet50_vd_ssld_v2.tar.gz',
                'output_stride': 8,
            },
            'enable_auxiliary_loss': True,
            'align_corners': False,
        },
        'lr_scheduler': {
            'type': 'PolynomialDecay',
            'learning_rate': 0.01,
            'power': 0.9,
            'end_lr': 1e-05,
        },
        'loss': {
            'types':
                [{'type': 'CrossEntropyLoss'}],
            'coef': [1, 0.4]
        },
    },
    'segformer': {
        'model': {
            'type': 'SegFormer_B0',
            'pretrained': 'https://bj.bcebos.com/paddleseg/dygraph/mix_vision_transformer_b0.tar.gz',
        },
        'optimizer': {
            'type': 'AdamW',
            'beta1': 0.9,
            'beta2': 0.999,
            'weight_decay': 0.01,
        },
        'lr_scheduler': {
            'type': 'CosineAnnealingDecay',
            'learning_rate': 6e-05,
            'T_max': 40000,
        },
        'loss': {
            'types':
                [{'type': 'MixedLoss', 'losses': [{'type': 'CrossEntropyLoss'}, {'type': 'DiceLoss'}], 'coef': [1, 1]}],
            'coef': [1],
        },
    },
    'segnet': {
        'model': {
            'type': 'SegNet'},
    },
    'unet': {
        'model': {
            'type': 'UNet',
            'use_deconv': False,
        },
    },
    'unet_3plus': {
        'model': {
            'type': 'UNet3Plus',
            'in_channels': 3,
            'is_batchnorm': True,
            'is_deepsup': False,
            'is_CGM': False,
        },
    },
    'unet_plusplus': {
        'model': {
            'type': 'UNetPlusPlus',
            'in_channels': 3,
            'use_deconv': False,
            'align_corners': False,
            'is_ds': True,
        },
    }}
