#include "traitsextractor.h"
#include <opencv2/opencv.hpp>
#include <fstream>
#include <iostream>

TraitsExtractor::TraitsExtractor()
{
}

TraitsExtractor::~TraitsExtractor()
{
}

QMap<QString, QString> TraitsExtractor::extractTraits(const cv::Mat &binaryImage, const QString &imagePath)
{
    QMap<QString, QString> traits;
    
    // 计算基本特征
    double area = calculateArea(binaryImage);
    double convexArea = calculateConvexArea(binaryImage);
    double length = calculateLength(binaryImage);
    double diameter = calculateDiameter(binaryImage);
    double depth = calculateDepth(binaryImage);
    double width = calculateWidth(binaryImage);
    double wdRatio = calculateWidthDepthRatio(width, depth);
    double sturdiness = calculateSturdiness(area, length);
    cv::Point initialPoint = calculateInitialPoint(binaryImage);
    cv::Point centroid = calculateCentroid(binaryImage);
    double apexAngleLeft = calculateApexAngleLeft(binaryImage);
    double apexAngleRight = calculateApexAngleRight(binaryImage);
    double apexAngleAll = calculateApexAngleAll(apexAngleLeft, apexAngleRight);
    double entireAngleLeft = calculateEntireAngleLeft(binaryImage);
    double entireAngleRight = calculateEntireAngleRight(binaryImage);
    double entireAngleAll = calculateEntireAngleAll(entireAngleLeft, entireAngleRight);
    
    // 计算分层特征
    QMap<QString, QString> layerMass = calculateLayerMass(binaryImage, 5);
    
    // 填充特征映射
    traits["image_path"] = imagePath;
    traits["area"] = QString::number(area);
    traits["convex_area"] = QString::number(convexArea);
    traits["length"] = QString::number(length);
    traits["diameter"] = QString::number(diameter);
    traits["depth"] = QString::number(depth);
    traits["width"] = QString::number(width);
    traits["wd_ratio"] = QString::number(wdRatio);
    traits["sturdiness"] = QString::number(sturdiness);
    traits["initial_x"] = QString::number(initialPoint.x);
    traits["initial_y"] = QString::number(initialPoint.y);
    traits["centroid_x"] = QString::number(centroid.x);
    traits["centroid_y"] = QString::number(centroid.y);
    traits["apex_angle_left"] = QString::number(apexAngleLeft);
    traits["apex_angle_right"] = QString::number(apexAngleRight);
    traits["apex_angle"] = QString::number(apexAngleAll);
    traits["entire_angle_left"] = QString::number(entireAngleLeft);
    traits["entire_angle_right"] = QString::number(entireAngleRight);
    traits["entire_angle"] = QString::number(entireAngleAll);
    
    // 添加分层特征
    for (auto it = layerMass.begin(); it != layerMass.end(); ++it) {
        traits[it.key()] = it.value();
    }
    
    return traits;
}

double TraitsExtractor::calculateArea(const cv::Mat &binaryImage)
{
    return cv::countNonZero(binaryImage);
}

double TraitsExtractor::calculateConvexArea(const cv::Mat &binaryImage)
{
    std::vector<std::vector<cv::Point>> contours = findContours(binaryImage);
    if (contours.empty()) {
        return 0;
    }
    
    std::vector<cv::Point> hull;
    cv::convexHull(contours[0], hull);
    return cv::contourArea(hull);
}

double TraitsExtractor::calculateLength(const cv::Mat &binaryImage)
{
    std::vector<std::vector<cv::Point>> contours = findContours(binaryImage);
    if (contours.empty()) {
        return 0;
    }
    
    return calculateContourLength(contours[0]);
}

double TraitsExtractor::calculateDiameter(const cv::Mat &binaryImage)
{
    std::vector<std::vector<cv::Point>> contours = findContours(binaryImage);
    if (contours.empty()) {
        return 0;
    }
    
    cv::RotatedRect ellipse = fitEllipse(contours[0]);
    return std::min(ellipse.size.width, ellipse.size.height);
}

double TraitsExtractor::calculateDepth(const cv::Mat &binaryImage)
{
    std::vector<std::vector<cv::Point>> contours = findContours(binaryImage);
    if (contours.empty()) {
        return 0;
    }
    
    cv::Rect boundingRect = cv::boundingRect(contours[0]);
    return boundingRect.height;
}

double TraitsExtractor::calculateWidth(const cv::Mat &binaryImage)
{
    std::vector<std::vector<cv::Point>> contours = findContours(binaryImage);
    if (contours.empty()) {
        return 0;
    }
    
    cv::Rect boundingRect = cv::boundingRect(contours[0]);
    return boundingRect.width;
}

double TraitsExtractor::calculateWidthDepthRatio(const double width, const double depth)
{
    if (depth == 0) {
        return 0;
    }
    return width / depth;
}

double TraitsExtractor::calculateSturdiness(const double area, const double length)
{
    if (length == 0) {
        return 0;
    }
    return area / length;
}

cv::Point TraitsExtractor::calculateInitialPoint(const cv::Mat &binaryImage)
{
    std::vector<std::vector<cv::Point>> contours = findContours(binaryImage);
    if (contours.empty()) {
        return cv::Point(0, 0);
    }
    
    // 假设根的起始点在图像的顶部
    int minY = binaryImage.rows;
    cv::Point initialPoint(0, 0);
    
    for (const auto &point : contours[0]) {
        if (point.y < minY) {
            minY = point.y;
            initialPoint = point;
        }
    }
    
    return initialPoint;
}

cv::Point TraitsExtractor::calculateCentroid(const cv::Mat &binaryImage)
{
    cv::Moments moments = cv::moments(binaryImage);
    if (moments.m00 == 0) {
        return cv::Point(0, 0);
    }
    
    int centroidX = moments.m10 / moments.m00;
    int centroidY = moments.m01 / moments.m00;
    return cv::Point(centroidX, centroidY);
}

double TraitsExtractor::calculateApexAngleLeft(const cv::Mat &binaryImage)
{
    // 实现左顶角计算
    return 0.0;
}

double TraitsExtractor::calculateApexAngleRight(const cv::Mat &binaryImage)
{
    // 实现右顶角计算
    return 0.0;
}

double TraitsExtractor::calculateApexAngleAll(const double leftAngle, const double rightAngle)
{
    return leftAngle + rightAngle;
}

double TraitsExtractor::calculateEntireAngleLeft(const cv::Mat &binaryImage)
{
    // 实现左整个角度计算
    return 0.0;
}

double TraitsExtractor::calculateEntireAngleRight(const cv::Mat &binaryImage)
{
    // 实现右整个角度计算
    return 0.0;
}

double TraitsExtractor::calculateEntireAngleAll(const double leftAngle, const double rightAngle)
{
    return leftAngle + rightAngle;
}

QMap<QString, QString> TraitsExtractor::calculateLayerMass(const cv::Mat &binaryImage, int layerCount)
{
    QMap<QString, QString> layerMass;
    std::vector<cv::Mat> layers = splitIntoLayers(binaryImage, layerCount);
    
    double totalArea = calculateArea(binaryImage);
    double totalLength = calculateLength(binaryImage);
    double totalConvexArea = calculateConvexArea(binaryImage);
    
    for (int i = 0; i < layers.size(); i++) {
        double layerArea = calculateArea(layers[i]);
        double layerLength = calculateLength(layers[i]);
        double layerConvexArea = calculateConvexArea(layers[i]);
        
        double areaRatio = totalArea > 0 ? layerArea / totalArea : 0;
        double lengthRatio = totalLength > 0 ? layerLength / totalLength : 0;
        double convexAreaRatio = totalConvexArea > 0 ? layerConvexArea / totalConvexArea : 0;
        
        layerMass[QString("layer_%1_area").arg(i)] = QString::number(layerArea);
        layerMass[QString("layer_%1_length").arg(i)] = QString::number(layerLength);
        layerMass[QString("layer_%1_convex_area").arg(i)] = QString::number(layerConvexArea);
        layerMass[QString("layer_%1_area_ratio").arg(i)] = QString::number(areaRatio);
        layerMass[QString("layer_%1_length_ratio").arg(i)] = QString::number(lengthRatio);
        layerMass[QString("layer_%1_convex_area_ratio").arg(i)] = QString::number(convexAreaRatio);
    }
    
    return layerMass;
}

cv::Mat TraitsExtractor::visualizeTraits(const cv::Mat &originalImage, const cv::Mat &binaryImage, const QMap<QString, QString> &traits)
{
    cv::Mat visualization = originalImage.clone();
    
    // 绘制二值图像轮廓
    std::vector<std::vector<cv::Point>> contours = findContours(binaryImage);
    if (!contours.empty()) {
        cv::drawContours(visualization, contours, -1, cv::Scalar(0, 255, 0), 2);
    }
    
    // 绘制质心
    cv::Point centroid(QString(traits["centroid_x"].toInt()), QString(traits["centroid_y"].toInt()));
    cv::circle(visualization, centroid, 5, cv::Scalar(0, 0, 255), -1);
    
    // 绘制起始点
    cv::Point initialPoint(QString(traits["initial_x"].toInt()), QString(traits["initial_y"].toInt()));
    cv::circle(visualization, initialPoint, 5, cv::Scalar(255, 0, 0), -1);
    
    // 绘制边界框
    if (!contours.empty()) {
        cv::Rect boundingRect = cv::boundingRect(contours[0]);
        cv::rectangle(visualization, boundingRect, cv::Scalar(255, 255, 0), 2);
    }
    
    return visualization;
}

bool TraitsExtractor::saveTraits(const QMap<QString, QString> &traits, const QString &savePath)
{
    std::ofstream file(savePath.toStdString());
    if (!file.is_open()) {
        return false;
    }
    
    // 写入表头
    bool first = true;
    for (auto it = traits.begin(); it != traits.end(); ++it) {
        if (!first) {
            file << ",";
        }
        file << it.key().toStdString();
        first = false;
    }
    file << "\n";
    
    // 写入数据
    first = true;
    for (auto it = traits.begin(); it != traits.end(); ++it) {
        if (!first) {
            file << ",";
        }
        file << it.value().toStdString();
        first = false;
    }
    file << "\n";
    
    file.close();
    return true;
}

bool TraitsExtractor::saveVisualization(const cv::Mat &visualization, const QString &savePath)
{
    return cv::imwrite(savePath.toStdString(), visualization);
}

std::vector<std::vector<cv::Point>> TraitsExtractor::findContours(const cv::Mat &binaryImage)
{
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(binaryImage, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    return contours;
}

double TraitsExtractor::calculateContourLength(const std::vector<cv::Point> &contour)
{
    return cv::arcLength(contour, true);
}

cv::RotatedRect TraitsExtractor::fitEllipse(const std::vector<cv::Point> &contour)
{
    if (contour.size() < 5) {
        return cv::RotatedRect();
    }
    return cv::fitEllipse(contour);
}

std::vector<cv::Mat> TraitsExtractor::splitIntoLayers(const cv::Mat &binaryImage, int layerCount)
{
    std::vector<cv::Mat> layers;
    int height = binaryImage.rows;
    int layerHeight = height / layerCount;
    
    for (int i = 0; i < layerCount; i++) {
        int y = i * layerHeight;
        int h = (i == layerCount - 1) ? (height - y) : layerHeight;
        cv::Rect roi(0, y, binaryImage.cols, h);
        layers.push_back(binaryImage(roi).clone());
    }
    
    return layers;
}
