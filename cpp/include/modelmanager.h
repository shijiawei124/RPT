#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include <opencv2/opencv.hpp>
#include <QString>
#include <vector>

class ModelManager
{
public:
    ModelManager();
    ~ModelManager();

    // 模型加载和推理
    bool loadModel(const QString &modelPath, const QString &modelType);
    cv::Mat predict(const cv::Mat &image);
    
    // 模型配置
    void setDevice(const QString &device);
    QString getDeviceType() const;
    int getGpuCount() const;
    
    // 滑动窗口推理
    cv::Mat slideInference(const cv::Mat &image, int cropSize, int stride);

private:
    // 模型相关
    void *model; // 模型指针，具体类型根据使用的框架而定
    QString deviceType;
    int gpuCount;
    
    // 辅助函数
    cv::Mat preprocessInput(const cv::Mat &image);
    cv::Mat postprocessOutput(const cv::Mat &output);
    std::vector<cv::Mat> splitImage(const cv::Mat &image, int cropSize, int stride);
    cv::Mat mergePatches(const std::vector<cv::Mat> &patches, int originalWidth, int originalHeight, int cropSize, int stride);
};

#endif // MODELMANAGER_H
