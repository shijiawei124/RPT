#include "imageprocessor.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

ImageProcessor::ImageProcessor()
{
}

ImageProcessor::~ImageProcessor()
{
}

cv::Mat ImageProcessor::loadImage(const QString &imagePath)
{
    return cv::imread(imagePath.toStdString());
}

cv::Mat ImageProcessor::preprocessImage(const cv::Mat &image)
{
    cv::Mat processed;
    cv::cvtColor(image, processed, cv::COLOR_BGR2RGB);
    cv::normalize(processed, processed, 0, 255, cv::NORM_MINMAX);
    return processed;
}

cv::Mat ImageProcessor::postprocessImage(const cv::Mat &image, int areaThreshold, int dilationSize)
{
    cv::Mat processed = image.clone();
    
    // 二值化
    if (processed.channels() > 1) {
        cv::cvtColor(processed, processed, cv::COLOR_BGR2GRAY);
    }
    cv::threshold(processed, processed, 127, 255, cv::THRESH_BINARY);
    
    // 去除小面积区域
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(processed, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area < areaThreshold) {
            cv::drawContours(processed, contours, i, cv::Scalar(0), -1);
        }
    }
    
    // 膨胀操作
    if (dilationSize > 0) {
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilationSize, dilationSize));
        cv::dilate(processed, processed, kernel);
    }
    
    return processed;
}

cv::Mat ImageProcessor::thresholdSegmentation(const cv::Mat &image, const QString &colorPlane, const QString &method, double threshold)
{
    cv::Mat gray = extractColorChannel(image, colorPlane);
    cv::Mat segmented;
    
    if (method == "manual threshold") {
        cv::threshold(gray, segmented, threshold, 255, cv::THRESH_BINARY);
    } else if (method == "Adaptive threshold(OTSU)") {
        cv::threshold(gray, segmented, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
    }
    
    // 中值滤波
    cv::medianBlur(segmented, segmented, 5);
    cv::medianBlur(segmented, segmented, 5);
    cv::medianBlur(segmented, segmented, 5);
    
    return segmented;
}

cv::Mat ImageProcessor::inpaintImage(const cv::Mat &image, const cv::Mat &mask)
{
    cv::Mat inpainted;
    cv::inpaint(image, mask, inpainted, 3, cv::INPAINT_TELEA);
    return inpainted;
}

cv::Mat ImageProcessor::resizeImage(const cv::Mat &image, int width, int height)
{
    cv::Mat resized;
    cv::resize(image, resized, cv::Size(width, height));
    return resized;
}

cv::Mat ImageProcessor::cropImage(const cv::Mat &image, const cv::Rect &roi)
{
    return image(roi);
}

cv::Mat ImageProcessor::erodeImage(const cv::Mat &image, int kernelSize)
{
    cv::Mat eroded;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, kernelSize));
    cv::erode(image, eroded, kernel);
    return eroded;
}

cv::Mat ImageProcessor::dilateImage(const cv::Mat &image, int kernelSize)
{
    cv::Mat dilated;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, kernelSize));
    cv::dilate(image, dilated, kernel);
    return dilated;
}

cv::Mat ImageProcessor::closeImage(const cv::Mat &image, int kernelSize)
{
    cv::Mat closed;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, kernelSize));
    cv::morphologyEx(image, closed, cv::MORPH_CLOSE, kernel);
    return closed;
}

cv::Mat ImageProcessor::openImage(const cv::Mat &image, int kernelSize)
{
    cv::Mat opened;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, kernelSize));
    cv::morphologyEx(image, opened, cv::MORPH_OPEN, kernel);
    return opened;
}

double ImageProcessor::calculateArea(const cv::Mat &image)
{
    return cv::countNonZero(image);
}

double ImageProcessor::calculatePerimeter(const cv::Mat &image)
{
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(image, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    double perimeter = 0;
    for (const auto &contour : contours) {
        perimeter += cv::arcLength(contour, true);
    }
    return perimeter;
}

cv::Point ImageProcessor::calculateCentroid(const cv::Mat &image)
{
    cv::Moments moments = cv::moments(image);
    int centroidX = moments.m10 / moments.m00;
    int centroidY = moments.m01 / moments.m00;
    return cv::Point(centroidX, centroidY);
}

bool ImageProcessor::saveImage(const cv::Mat &image, const QString &savePath)
{
    return cv::imwrite(savePath.toStdString(), image);
}

cv::Mat ImageProcessor::extractColorChannel(const cv::Mat &image, const QString &colorPlane)
{
    cv::Mat channel;
    
    if (colorPlane == "R") {
        std::vector<cv::Mat> channels;
        cv::split(image, channels);
        channel = channels[2]; // BGR format
    } else if (colorPlane == "G") {
        std::vector<cv::Mat> channels;
        cv::split(image, channels);
        channel = channels[1];
    } else if (colorPlane == "B") {
        std::vector<cv::Mat> channels;
        cv::split(image, channels);
        channel = channels[0];
    } else if (colorPlane == "H") {
        cv::Mat hsv;
        cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
        std::vector<cv::Mat> channels;
        cv::split(hsv, channels);
        channel = channels[0];
    } else if (colorPlane == "S") {
        cv::Mat hsv;
        cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
        std::vector<cv::Mat> channels;
        cv::split(hsv, channels);
        channel = channels[1];
    } else if (colorPlane == "V") {
        cv::Mat hsv;
        cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
        std::vector<cv::Mat> channels;
        cv::split(hsv, channels);
        channel = channels[2];
    } else {
        cv::cvtColor(image, channel, cv::COLOR_BGR2GRAY);
    }
    
    return channel;
}
