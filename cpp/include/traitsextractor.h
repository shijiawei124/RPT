#ifndef TRAITSEXTRACTOR_H
#define TRAITSEXTRACTOR_H

#include <opencv2/opencv.hpp>
#include <QString>
#include <QMap>

class TraitsExtractor
{
public:
    TraitsExtractor();
    ~TraitsExtractor();

    // 特征提取
    QMap<QString, QString> extractTraits(const cv::Mat &binaryImage, const QString &imagePath);
    
    // 特征分析
    double calculateArea(const cv::Mat &binaryImage);
    double calculateConvexArea(const cv::Mat &binaryImage);
    double calculateLength(const cv::Mat &binaryImage);
    double calculateDiameter(const cv::Mat &binaryImage);
    double calculateDepth(const cv::Mat &binaryImage);
    double calculateWidth(const cv::Mat &binaryImage);
    double calculateWidthDepthRatio(const double width, const double depth);
    double calculateSturdiness(const double area, const double length);
    cv::Point calculateInitialPoint(const cv::Mat &binaryImage);
    cv::Point calculateCentroid(const cv::Mat &binaryImage);
    double calculateApexAngleLeft(const cv::Mat &binaryImage);
    double calculateApexAngleRight(const cv::Mat &binaryImage);
    double calculateApexAngleAll(const double leftAngle, const double rightAngle);
    double calculateEntireAngleLeft(const cv::Mat &binaryImage);
    double calculateEntireAngleRight(const cv::Mat &binaryImage);
    double calculateEntireAngleAll(const double leftAngle, const double rightAngle);
    
    // 分层分析
    QMap<QString, QString> calculateLayerMass(const cv::Mat &binaryImage, int layerCount);
    
    // 可视化
    cv::Mat visualizeTraits(const cv::Mat &originalImage, const cv::Mat &binaryImage, const QMap<QString, QString> &traits);
    
    // 保存结果
    bool saveTraits(const QMap<QString, QString> &traits, const QString &savePath);
    bool saveVisualization(const cv::Mat &visualization, const QString &savePath);

private:
    // 辅助函数
    std::vector<std::vector<cv::Point>> findContours(const cv::Mat &binaryImage);
    double calculateContourLength(const std::vector<cv::Point> &contour);
    cv::RotatedRect fitEllipse(const std::vector<cv::Point> &contour);
    std::vector<cv::Mat> splitIntoLayers(const cv::Mat &binaryImage, int layerCount);
};

#endif // TRAITSEXTRACTOR_H
