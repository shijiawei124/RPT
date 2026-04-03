#ifndef MODELTRAINER_H
#define MODELTRAINER_H

#include <QString>
#include <QMap>

class ModelTrainer
{
public:
    ModelTrainer();
    ~ModelTrainer();

    // 训练配置
    void setDatasetPath(const QString &datasetPath);
    void setSaveDir(const QString &saveDir);
    void setModelType(const QString &modelType);
    void setBatchSize(int batchSize);
    void setEpochs(int epochs);
    void setLearningRate(float learningRate);
    void setDatasetSplit(float trainSplit, float valSplit);
    
    // 开始训练
    bool startTraining();
    
    // 停止训练
    void stopTraining();
    
    // 获取训练状态
    bool isTraining() const;
    float getTrainingProgress() const;
    QString getTrainingStatus() const;

private:
    // 训练配置
    QString datasetPath;
    QString saveDir;
    QString modelType;
    int batchSize;
    int epochs;
    float learningRate;
    float trainSplit;
    float valSplit;
    
    // 训练状态
    bool training;
    float progress;
    QString status;
    
    // 辅助函数
    bool prepareDataset();
    bool runTrainingScript();
};

#endif // MODELTRAINER_H
