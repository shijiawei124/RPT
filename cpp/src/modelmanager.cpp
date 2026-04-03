#include "modelmanager.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <onnxruntime/core/session/onnxruntime_cxx_api.h>

ModelManager::ModelManager() :
    model(nullptr),
    deviceType("cpu"),
    gpuCount(0)
{
    // 检查GPU是否可用
    try {
        Ort::Env env;
        Ort::SessionOptions session_options;
        Ort::Session session(env, "dummy_model.onnx", session_options);
        // 检查是否有GPU可用
        Ort::AllocatorWithDefaultOptions allocator;
        int device_count = Ort::GetAvailableDevices(allocator);
        gpuCount = device_count;
        if (gpuCount > 0) {
            deviceType = "gpu";
        }
    } catch (...) {
        // 如果出错，默认使用CPU
        deviceType = "cpu";
        gpuCount = 0;
    }
}

ModelManager::~ModelManager()
{
    // 释放模型资源
    if (model) {
        delete static_cast<Ort::Session*>(model);
    }
}

bool ModelManager::loadModel(const QString &modelPath, const QString &modelType)
{
    try {
        Ort::Env env;
        Ort::SessionOptions session_options;
        
        // 设置运行设备
        if (deviceType == "gpu") {
            OrtCUDAProviderOptions cuda_options;
            session_options.AppendExecutionProvider_CUDA(cuda_options);
        }
        
        // 加载模型
        Ort::Session *session = new Ort::Session(env, modelPath\.toStdWString\(\)\.c_str\(\), session_options);
        model = session;
        return true;
    } catch (const std::exception &e) {
        std::cerr << "Error loading model: " << e.what() << std::endl;
        return false;
    }
}

cv::Mat ModelManager::predict(const cv::Mat &image)
{
    if (!model) {
        return cv::Mat();
    }
    
    // 预处理输入图像
    cv::Mat preprocessed = preprocessInput(image);
    
    // 模型推理
    try {
        Ort::Session *session = static_cast<Ort::Session*>(model);
        Ort::AllocatorWithDefaultOptions allocator;
        
        // 获取输入输出信息
        size_t num_input_nodes = session->GetInputCount();
        size_t num_output_nodes = session->GetOutputCount();
        
        // 准备输入张量
        std::vector<int64_t> input_shape = {1, 3, 512, 512};
        size_t input_size = 1 * 3 * 512 * 512;
        std::vector<float> input_data(input_size);
        
        // 填充输入数据
        for (int i = 0; i < 512; i++) {
            for (int j = 0; j < 512; j++) {
                for (int c = 0; c < 3; c++) {
                    input_data[c * 512 * 512 + i * 512 + j] = preprocessed.at<cv::Vec3f>(i, j)[c];
                }
            }
        }
        
        // 创建输入张量
        auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        Ort::Value input_tensor = Ort::Value::CreateTensor<float>(memory_info, input_data.data(), input_size, input_shape.data(), input_shape.size());
        
        // 运行推理
        std::vector<const char*> input_names = {session->GetInputName(0, allocator)};
        std::vector<const char*> output_names = {session->GetOutputName(0, allocator)};
        std::vector<Ort::Value> output_tensors = session->Run(Ort::RunOptions{nullptr}, input_names.data(), &input_tensor, 1, output_names.data(), num_output_nodes);
        
        // 处理输出
        Ort::Value &output_tensor = output_tensors[0];
        float *output_data = output_tensor.GetTensorMutableData<float>();
        
        // 后处理输出
        cv::Mat output = cv::Mat::zeros(512, 512, CV_32F);
        for (int i = 0; i < 512; i++) {
            for (int j = 0; j < 512; j++) {
                output.at<float>(i, j) = output_data[i * 512 + j];
            }
        }
        
        cv::resize(output, output, image.size());
        return postprocessOutput(output);
    } catch (const std::exception &e) {
        std::cerr << "Error during prediction: " << e.what() << std::endl;
        return cv::Mat();
    }
}

void ModelManager::setDevice(const QString &device)
{
    deviceType = device;
}

QString ModelManager::getDeviceType() const
{
    return deviceType;
}

int ModelManager::getGpuCount() const
{
    return gpuCount;
}

cv::Mat ModelManager::slideInference(const cv::Mat &image, int cropSize, int stride)
{
    // 分割图像为多个 patches
    std::vector<cv::Mat> patches = splitImage(image, cropSize, stride);
    
    // 对每个 patch 进行推理
    std::vector<cv::Mat> outputs;
    for (const cv::Mat &patch : patches) {
        outputs.push_back(predict(patch));
    }
    
    // 合并 patches
    return mergePatches(outputs, image.cols, image.rows, cropSize, stride);
}

cv::Mat ModelManager::preprocessInput(const cv::Mat &image)
{
    // 预处理输入图像
    cv::Mat preprocessed;
    cv::resize(image, preprocessed, cv::Size(512, 512));
    cv::cvtColor(preprocessed, preprocessed, cv::COLOR_BGR2RGB);
    preprocessed.convertTo(preprocessed, CV_32F);
    preprocessed = preprocessed / 255.0;
    return preprocessed;
}

cv::Mat ModelManager::postprocessOutput(const cv::Mat &output)
{
    // 后处理输出
    cv::Mat postprocessed;
    cv::threshold(output, postprocessed, 0.5, 255, cv::THRESH_BINARY);
    return postprocessed;
}

std::vector<cv::Mat> ModelManager::splitImage(const cv::Mat &image, int cropSize, int stride)
{
    std::vector<cv::Mat> patches;
    int width = image.cols;
    int height = image.rows;
    
    for (int y = 0; y < height; y += stride) {
        for (int x = 0; x < width; x += stride) {
            int patchWidth = std::min(cropSize, width - x);
            int patchHeight = std::min(cropSize, height - y);
            cv::Rect roi(x, y, patchWidth, patchHeight);
            patches.push_back(image(roi).clone());
        }
    }
    
    return patches;
}

cv::Mat ModelManager::mergePatches(const std::vector<cv::Mat> &patches, int originalWidth, int originalHeight, int cropSize, int stride)
{
    cv::Mat merged = cv::Mat::zeros(originalHeight, originalWidth, CV_8UC1);
    int patchIndex = 0;
    
    for (int y = 0; y < originalHeight; y += stride) {
        for (int x = 0; x < originalWidth; x += stride) {
            if (patchIndex < patches.size()) {
                int patchWidth = std::min(cropSize, originalWidth - x);
                int patchHeight = std::min(cropSize, originalHeight - y);
                cv::Rect roi(x, y, patchWidth, patchHeight);
                patches[patchIndex].copyTo(merged(roi));
                patchIndex++;
            }
        }
    }
    
    return merged;
}

