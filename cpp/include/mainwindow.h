#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QTimer>
#include <QString>
#include <QMap>
#include <opencv2/opencv.hpp>

#include "mygraphicsview.h"
#include "imageprocessor.h"
#include "modelmanager.h"
#include "traitsextractor.h"
#include "modeltrainer.h"

namespace Ui {
class MainWindow;
}

class GF_rootAnalysis : public QMainWindow
{
    Q_OBJECT

public:
    explicit GF_rootAnalysis(QWidget *parent = nullptr);
    ~GF_rootAnalysis();

private slots:
    // 文件操作
    void selectDirPath();
    void selectFilePath();
    void updateCurrentImage();
    void updateByLineEdit();
    void updateFileDict(QWidget *sender = nullptr);
    
    // 分割功能
    void startSegmentation();
    void stopThread();
    
    // 后处理功能
    void updateThresholdSegmentation();
    void inpaintingMode();
    void paintCurrentImage();
    void saveProcessedImage();
    void startProcess();
    
    // 计算功能
    void startCalculate();
    
    // 训练功能
    void startTrain();
    void datasetSplitBalance();
    
    // 其他功能
    void updateTextBrowser(const QString &text);
    void updateGraphicsView(const QString &imagePath, MyGraphicsView *graphicsView = nullptr, bool fitInView = true);
    void updateTraitsTable();
    void updateTreeWidgetProcessingConfig();
    void updateIsSlide();
    void onProcessingParamChanged();
    void _doUpdateProcessing();

private:
    Ui::MainWindow *ui;
    
    // 数据结构
    QMap<QString, QMap<QString, QString>> fileDict;
    QMap<QString, QVariant> currentImage;
    
    // 路径
    QString datasetRootPath;
    QString predictSaveDir;
    QString warpSaveDir;
    QString postprocessSaveDir;
    QString traitsSaveDir;
    
    // 模型相关
    QStringList trainedModels;
    QStringList trainableModels;
    QString place;
    
    // 标志
    bool inpaintingModeFlag;
    
    // 计时器
    QTimer *processingTimer;
    
    // 工具类
    ImageProcessor *imageProcessor;
    ModelManager *modelManager;
    TraitsExtractor *traitsExtractor;
    ModelTrainer *modelTrainer;
    
    // 辅助函数
    void setupLogit();
    void setupDefaultValue();
    void loadTreeWidgetFromDirPath(const QString &dirPath, QTreeWidgetItem *treeWidget);
    cv::Mat autoInpainting(const cv::Mat &img);
    void traitsInit();
};

#endif // MAINWINDOW_H
