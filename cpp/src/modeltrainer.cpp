#include "modeltrainer.h"
#include <QProcess>
#include <QDir>
#include <QFile>
#include <QTextStream>

ModelTrainer::ModelTrainer() :
    batchSize(1),
    epochs(40000),
    learningRate(0.01),
    trainSplit(0.8),
    valSplit(0.2),
    training(false),
    progress(0.0),
    status("Ready")
{
}

ModelTrainer::~ModelTrainer()
{
    stopTraining();
}

void ModelTrainer::setDatasetPath(const QString &datasetPath)
{
    this->datasetPath = datasetPath;
}

void ModelTrainer::setSaveDir(const QString &saveDir)
{
    this->saveDir = saveDir;
}

void ModelTrainer::setModelType(const QString &modelType)
{
    this->modelType = modelType;
}

void ModelTrainer::setBatchSize(int batchSize)
{
    this->batchSize = batchSize;
}

void ModelTrainer::setEpochs(int epochs)
{
    this->epochs = epochs;
}

void ModelTrainer::setLearningRate(float learningRate)
{
    this->learningRate = learningRate;
}

void ModelTrainer::setDatasetSplit(float trainSplit, float valSplit)
{
    this->trainSplit = trainSplit;
    this->valSplit = valSplit;
}

bool ModelTrainer::startTraining()
{
    if (training) {
        return false;
    }
    
    if (datasetPath.isEmpty() || saveDir.isEmpty() || modelType.isEmpty()) {
        status = "Error: Missing required parameters";
        return false;
    }
    
    if (!prepareDataset()) {
        status = "Error: Failed to prepare dataset";
        return false;
    }
    
    training = true;
    progress = 0.0;
    status = "Starting training...";
    
    // 启动训练进程
    QProcess *process = new QProcess();
    process->setWorkingDirectory(QDir::currentPath());
    
    // 构建训练命令
    QString pythonScript = "train.py";
    QStringList arguments;
    arguments << pythonScript;
    arguments << "--dataset_path" << datasetPath;
    arguments << "--save_dir" << saveDir;
    arguments << "--model_type" << modelType;
    arguments << "--batch_size" << QString::number(batchSize);
    arguments << "--epochs" << QString::number(epochs);
    arguments << "--learning_rate" << QString::number(learningRate);
    arguments << "--train_split" << QString::number(trainSplit);
    arguments << "--val_split" << QString::number(valSplit);
    
    // 启动进程
    process->start("python", arguments);
    
    // 连接信号槽
    connect(process, &QProcess::readyReadStandardOutput, [this, process]() {
        QString output = process->readAllStandardOutput();
        // 解析输出，更新进度和状态
        // 这里需要根据实际的训练脚本输出格式进行调整
        status = output.trimmed();
        // 假设输出中包含进度信息
        // progress = ...
    });
    
    connect(process, &QProcess::readyReadStandardError, [this, process]() {
        QString error = process->readAllStandardError();
        status = "Error: " + error.trimmed();
    });
    
    connect(process, &QProcess::finished, [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::NormalExit && exitCode == 0) {
            status = "Training completed successfully";
            progress = 1.0;
        } else {
            status = "Training failed with exit code " + QString::number(exitCode);
        }
        training = false;
        process->deleteLater();
    });
    
    return true;
}

void ModelTrainer::stopTraining()
{
    // 停止训练进程
    // 这里需要实现停止训练的逻辑
    training = false;
    status = "Training stopped";
}

bool ModelTrainer::isTraining() const
{
    return training;
}

float ModelTrainer::getTrainingProgress() const
{
    return progress;
}

QString ModelTrainer::getTrainingStatus() const
{
    return status;
}

bool ModelTrainer::prepareDataset()
{
    // 检查数据集路径是否存在
    QDir datasetDir(datasetPath);
    if (!datasetDir.exists()) {
        return false;
    }
    
    // 检查保存目录是否存在，如果不存在则创建
    QDir saveDirObj(saveDir);
    if (!saveDirObj.exists()) {
        if (!saveDirObj.mkpath(".")) {
            return false;
        }
    }
    
    // 生成训练配置文件
    QString configPath = saveDir + "/train_config.yml";
    QFile configFile(configPath);
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&configFile);
    out << "batch_size: " << batchSize << "\n";
    out << "iters: " << epochs << "\n";
    out << "train_dataset:\n";
    out << "  type: RootSeg\n";
    out << "  dataset_root: " << datasetPath << "\n";
    out << "  transforms:\n";
    out << "    - type: ResizeStepScaling\n";
    out << "      min_scale_factor: 0.5\n";
    out << "      max_scale_factor: 2.0\n";
    out << "      scale_step_size: 0.25\n";
    out << "    - type: RandomPaddingCrop\n";
    out << "      crop_size: [512, 512]\n";
    out << "      label_padding_value: 0\n";
    out << "    - type: RandomHorizontalFlip\n";
    out << "    - type: RandomBlur\n";
    out << "      prob: 0.1\n";
    out << "    - type: RandomDistort\n";
    out << "      brightness_range: 0.4\n";
    out << "      contrast_range: 0.4\n";
    out << "      saturation_range: 0.4\n";
    out << "    - type: RandomNoise\n";
    out << "    - type: Normalize\n";
    out << "  mode: train\n";
    out << "val_dataset:\n";
    out << "  type: RootSeg\n";
    out << "  dataset_root: " << datasetPath << "\n";
    out << "  transforms: [{'type': 'Normalize'}]\n";
    out << "  mode: val\n";
    out << "optimizer:\n";
    out << "  type: sgd\n";
    out << "  momentum: 0.9\n";
    out << "  weight_decay: 4e-05\n";
    out << "lr_scheduler:\n";
    out << "  type: PolynomialDecay\n";
    out << "  learning_rate: " << learningRate << "\n";
    out << "  end_lr: 0\n";
    out << "  power: 0.9\n";
    out << "loss:\n";
    out << "  types: [{'type': 'CrossEntropyLoss'}]\n";
    out << "  coef: [1]\n";
    out << "model:\n";
    out << "  num_classes: 2\n";
    out << "  pretrained: null\n";
    out << "test_config:\n";
    out << "  aug_eval: true\n";
    out << "  flip_horizontal: true\n";
    out << "  flip_vertical: true\n";
    out << "  is_slide: true\n";
    out << "  crop_size: [1024, 1024]\n";
    out << "  stride: [768, 768]\n";
    
    configFile.close();
    
    return true;
}

bool ModelTrainer::runTrainingScript()
{
    // 运行训练脚本
    // 这里需要实现运行训练脚本的逻辑
    return true;
}
