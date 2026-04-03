#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <opencv2/opencv.hpp>
#include <QString>

class ImageProcessor
{
public:
    ImageProcessor();
    ~ImageProcessor();

    // 图像处理功能
    cv::Mat loadImage(const QString &imagePath);
    cv::Mat preprocessImage(const cv::Mat &image);
    cv::Mat postprocessImage(const cv::Mat &image, int areaThreshold, int dilationSize);
    cv::Mat thresholdSegmentation(const cv::Mat &image, const QString &colorPlane, const QString &method, double threshold);
    cv::Mat inpaintImage(const cv::Mat &image, const cv::Mat &mask);
    cv::Mat resizeImage(const cv::Mat &image, int width, int height);
    cv::Mat cropImage(const cv::Mat &image, const cv::Rect &roi);
    
    // 形态学操作
    cv::Mat erodeImage(const cv::Mat &image, int kernelSize);
    cv::Mat dilateImage(const cv::Mat &image, int kernelSize);
    cv::Mat closeImage(const cv::Mat &image, int kernelSize);
    cv::Mat openImage(const cv::Mat &image, int kernelSize);
    
    // 图像分析
    double calculateArea(const cv::Mat &image);
    double calculatePerimeter(const cv::Mat &image);
    cv::Point calculateCentroid(const cv::Mat &image);
    
    // 文件操作
    bool saveImage(const cv::Mat &image, const QString &savePath);

private:
    // 辅助函数
    cv::Mat extractColorChannel(const cv::Mat &image, const QString &colorPlane);
};

#endif // IMAGEPROCESSOR_H
