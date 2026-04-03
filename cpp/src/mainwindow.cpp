#include "mainwindow.h"
#include "ui_GF_rootAnalysis.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QDir>
#include <QImage>
#include <QPixmap>
#include <QTreeWidgetItem>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

GF_rootAnalysis::GF_rootAnalysis(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    inpaintingModeFlag(false),
    processingTimer(new QTimer(this))
{
    ui->setupUi(this);
    setupLogit();
    setupDefaultValue();
    
    // 初始化工具类
    imageProcessor = new ImageProcessor();
    modelManager = new ModelManager();
    traitsExtractor = new TraitsExtractor();
    modelTrainer = new ModelTrainer();
    
    // 防抖计时器：延时图像处理
    processingTimer->setSingleShot(true);
    connect(processingTimer, &QTimer::timeout, this, &GF_rootAnalysis::_doUpdateProcessing);
    
    // 将相关控件统一入口
    QList<QWidget*> paramWidgets = {
        ui->spinBox_AreaThreshold,
        ui->spinBox_Dilation,
        ui->spinBox_rbo_Top,
        ui->spinBox_rbo_Bottom,
        ui->spinBox_rbo_Left,
        ui->spinBox_rbo_Right
    };
    
    for (QWidget *widget : paramWidgets) {
        connect(widget, SIGNAL(valueChanged(int)), this, SLOT(onProcessingParamChanged()));
    }
    
    // 检查环境
    place = modelManager->getDeviceType();
    if (place == "cpu") {
        updateTextBrowser("Warning: CPU is used, which is slow and not recommended.");
        ui->tab_Train->setEnabled(false);
        ui->spinBox_GPU_id->setValue(-1);
        ui->spinBox_GPU_id->setEnabled(false);
    }
    ui->spinBox_GPU_id->setMaximum(modelManager->getGpuCount() - 1);
}

GF_rootAnalysis::~GF_rootAnalysis()
{
    delete ui;
    delete imageProcessor;
    delete modelManager;
    delete traitsExtractor;
    delete modelTrainer;
    delete processingTimer;
}

void GF_rootAnalysis::setupLogit()
{
    // 初始化文件字典
    fileDict.clear();
    
    // 初始化模型列表
    trainedModels = {
        "esegformer_b0", "esegformer_b1", "esegformer_b2", "esegformer_b3", "esegformer_b4", "esegformer_b5",
        "deeplabv3_resnet50", "deeplabv3_resnet101", "deeplabv3p_resnet50", "deeplabv3p_resnet101", "hardnet",
        "pspnet_resnet50", "pspnet_resnet101", "segformer_b0", "segformer_b1", "segformer_b2", "segformer_b3",
        "segformer_b4", "segformer_b5", "segnet", "unet", "unet_3plus", "unet_plusplus"
    };
    
    trainableModels = trainedModels;
    
    // 初始化当前图像
    currentImage = {
        {"image_path", QVariant()},
        {"img", QVariant()},
        {"gray", QVariant()},
        {"binary", QVariant()},
        {"thresholdseg", QVariant()},
        {"processed", QVariant()},
        {"traits", QVariant()},
        {"visualization", QVariant()}
    };
    
    // 连接信号槽
    connect(ui->pushButton_DataDir, &QPushButton::clicked, this, &GF_rootAnalysis::selectDirPath);
    connect(ui->treeWidget_Files, &QTreeWidget::itemDoubleClicked, this, &GF_rootAnalysis::updateCurrentImage);
    connect(ui->pushButton_FirstImage, &QPushButton::clicked, this, &GF_rootAnalysis::updateCurrentImage);
    connect(ui->pushButton_PreviousImage, &QPushButton::clicked, this, &GF_rootAnalysis::updateCurrentImage);
    connect(ui->pushButton_NextImage, &QPushButton::clicked, this, &GF_rootAnalysis::updateCurrentImage);
    connect(ui->pushButton_LastImage, &QPushButton::clicked, this, &GF_rootAnalysis::updateCurrentImage);
    connect(ui->pushButton_FitAllImageView, &QPushButton::clicked, ui->graphicsView_Main, &MyGraphicsView::fitInView);
    connect(ui->pushButton_FitAllImageView, &QPushButton::clicked, ui->graphicsView_Main2, &MyGraphicsView::fitInView);
    connect(ui->pushButton_FitAllImageView, &QPushButton::clicked, ui->graphicsView_Main3, &MyGraphicsView::fitInView);
    
    // 分割标签页
    connect(ui->pushButton_WeightPath, &QPushButton::clicked, this, &GF_rootAnalysis::selectFilePath);
    connect(ui->pushButton_SaveDir_predict, &QPushButton::clicked, this, &GF_rootAnalysis::selectDirPath);
    connect(ui->lineEdit_SaveDir_predict, &QLineEdit::textChanged, this, &GF_rootAnalysis::updateByLineEdit);
    connect(ui->checkBox_IsSlide, &QCheckBox::stateChanged, this, &GF_rootAnalysis::updateIsSlide);
    connect(ui->pushButton_PredictOne, &QPushButton::clicked, this, &GF_rootAnalysis::startSegmentation);
    connect(ui->pushButton_PredictAll, &QPushButton::clicked, this, &GF_rootAnalysis::startSegmentation);
    connect(ui->pushButton_StopPredict, &QPushButton::clicked, this, &GF_rootAnalysis::stopThread);
    
    // 变形标签页
    connect(ui->pushButton_SaveDir_warp, &QPushButton::clicked, this, &GF_rootAnalysis::selectDirPath);
    connect(ui->lineEdit_SaveDir_warp, &QLineEdit::textChanged, this, &GF_rootAnalysis::updateByLineEdit);
    connect(ui->pushButton_ManualWarp, &QPushButton::clicked, this, &GF_rootAnalysis::inpaintingMode);
    connect(ui->pushButton_Mask, &QPushButton::clicked, this, &GF_rootAnalysis::paintCurrentImage);
    connect(ui->pushButton_Paint, &QPushButton::clicked, this, &GF_rootAnalysis::paintCurrentImage);
    connect(ui->pushButton_Erase, &QPushButton::clicked, this, &GF_rootAnalysis::paintCurrentImage);
    connect(ui->pushButton_Warp, &QPushButton::clicked, this, &GF_rootAnalysis::startSegmentation);
    connect(ui->pushButton_WarpAll, &QPushButton::clicked, this, &GF_rootAnalysis::startSegmentation);
    connect(ui->pushButton_StopWarp, &QPushButton::clicked, this, &GF_rootAnalysis::stopThread);
    
    // 后处理标签页
    connect(ui->comboBox_ColorPlane, &QComboBox::currentTextChanged, this, &GF_rootAnalysis::updateThresholdSegmentation);
    connect(ui->comboBox_Method, &QComboBox::currentTextChanged, this, &GF_rootAnalysis::updateThresholdSegmentation);
    connect(ui->doubleSpinBox_Threshold, &QDoubleSpinBox::valueChanged, this, &GF_rootAnalysis::updateThresholdSegmentation);
    connect(ui->treeWidget_ProcessingConfig, &QTreeWidget::itemChanged, this, &GF_rootAnalysis::updateTreeWidgetProcessingConfig);
    connect(ui->pushButton_Inpainting, &QPushButton::clicked, this, &GF_rootAnalysis::inpaintingMode);
    connect(ui->pushButton_SaveDir_postporcess, &QPushButton::clicked, this, &GF_rootAnalysis::selectDirPath);
    connect(ui->lineEdit_SaveDir_postporcess, &QLineEdit::textChanged, this, &GF_rootAnalysis::updateByLineEdit);
    connect(ui->pushButton_SaveThisImage, &QPushButton::clicked, this, &GF_rootAnalysis::saveProcessedImage);
    connect(ui->pushButton_ProcessingAllImage, &QPushButton::clicked, this, &GF_rootAnalysis::startProcess);
    connect(ui->pushButton_StopProcess, &QPushButton::clicked, this, &GF_rootAnalysis::stopThread);
    
    // 计算标签页
    connect(ui->pushButton_SaveDir_cacuate, &QPushButton::clicked, this, &GF_rootAnalysis::selectDirPath);
    connect(ui->lineEdit_SaveDir_calculate, &QLineEdit::textChanged, this, &GF_rootAnalysis::updateByLineEdit);
    connect(ui->pushButton_Calculate, &QPushButton::clicked, this, &GF_rootAnalysis::startCalculate);
    connect(ui->pushButton_CalculateAll, &QPushButton::clicked, this, &GF_rootAnalysis::startCalculate);
    connect(ui->pushButton_StopCalculate, &QPushButton::clicked, this, &GF_rootAnalysis::stopThread);
    
    // 训练标签页
    connect(ui->pushButton_DatasetPath, &QPushButton::clicked, this, &GF_rootAnalysis::selectDirPath);
    connect(ui->pushButton_SaveDir_train, &QPushButton::clicked, this, &GF_rootAnalysis::selectDirPath);
    connect(ui->doubleSpinBox_DatasetSplit_train, &QDoubleSpinBox::valueChanged, this, &GF_rootAnalysis::datasetSplitBalance);
    connect(ui->doubleSpinBox_DatasetSplit_val, &QDoubleSpinBox::valueChanged, this, &GF_rootAnalysis::datasetSplitBalance);
    connect(ui->pushButton_ResumeModel, &QPushButton::clicked, this, &GF_rootAnalysis::selectDirPath);
    connect(ui->pushButton_StartTrain, &QPushButton::clicked, this, &GF_rootAnalysis::startTrain);
    connect(ui->pushButton_StopTrain, &QPushButton::clicked, this, &GF_rootAnalysis::stopThread);
}

void GF_rootAnalysis::setupDefaultValue()
{
    ui->comboBox_SegmentationModel->addItems(trainedModels);
    for (const QString &model : trainableModels) {
        ui->comboBox_TrainableModel->addItem(model);
    }
}

void GF_rootAnalysis::updateTextBrowser(const QString &text)
{
    if (ui->splitter_Right->sizes()[1] == 0) {
        ui->splitter_Right->setSizes({1, 1});
    }
    if (ui->splitter_Middle->sizes()[1] == 0) {
        ui->splitter_Middle->setSizes({1, 1});
    }
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    ui->textBrowser_MessagePrinter->append(currentTime + " >>" + text);
    ui->textBrowser_MessagePrinter->moveCursor(QTextCursor::End);
    ui->textBrowser_MessagePrinter->repaint();
}

void GF_rootAnalysis::loadTreeWidgetFromDirPath(const QString &dirPath, QTreeWidgetItem *treeWidget)
{
    QStringList imageExtensions = {"*.jpg", "*.jpeg", "*.png", "*.bmp", "*.gif"};
    QDir dir(dirPath);
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    
    for (const QFileInfo &fileInfo : fileInfoList) {
        if (fileInfo.isDir()) {
            QTreeWidgetItem *dirItem = new QTreeWidgetItem(treeWidget);
            dirItem->setText(0, fileInfo.fileName());
            dirItem->setText(1, fileInfo.absoluteFilePath());
            loadTreeWidgetFromDirPath(fileInfo.absoluteFilePath(), dirItem);
            if (dirItem->childCount() == 0) {
                if (treeWidget->treeWidget()) {
                    treeWidget->treeWidget()->takeTopLevelItem(treeWidget->treeWidget()->indexOfTopLevelItem(dirItem));
                } else {
                    treeWidget->removeChild(dirItem);
                }
                delete dirItem;
            }
        } else if (fileInfo.isFile()) {
            for (const QString &extension : imageExtensions) {
                if (fileInfo.fileName().endsWith(extension, Qt::CaseInsensitive)) {
                    QString filePath = fileInfo.absoluteFilePath();
                    fileDict[filePath] = {
                        {"binary_path", ""},
                        {"processed_path", ""},
                        {"traits", ""},
                        {"visualization", ""}
                    };
                    QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
                    item->setText(0, fileInfo.fileName());
                    item->setText(1, filePath);
                    break;
                }
            }
        }
    }
}

void GF_rootAnalysis::selectDirPath()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, "Open dir");
    if (dirPath.isEmpty()) {
        return;
    }
    
    QPushButton *senderButton = qobject_cast<QPushButton*>(sender());
    if (!senderButton) {
        return;
    }
    
    if (senderButton == ui->pushButton_SaveDir_train) {
        ui->lineEdit_SaveDir_train->setText(dirPath);
    } else if (senderButton == ui->pushButton_SaveDir_predict) {
        ui->lineEdit_SaveDir_predict->setText(dirPath);
    } else if (senderButton == ui->pushButton_DatasetPath) {
        ui->lineEdit_DatasetPath->setText(dirPath);
    } else if (senderButton == ui->pushButton_ResumeModel) {
        ui->lineEdit_ResumeModel->setText(dirPath);
    } else if (senderButton == ui->pushButton_DataDir) {
        datasetRootPath = dirPath;
        ui->label_DataDirShow->setText(dirPath);
        
        fileDict.clear();
        ui->treeWidget_Files->clear();
        loadTreeWidgetFromDirPath(dirPath, ui->treeWidget_Files);
        ui->treeWidget_Files->expandAll();
        
        currentImage = {
            {"image_path", QVariant()},
            {"img", QVariant()},
            {"gray", QVariant()},
            {"binary", QVariant()},
            {"thresholdseg", QVariant()},
            {"processed", QVariant()},
            {"traits", QVariant()},
            {"visualization", QVariant()}
        };
        
        updateFileDict(ui->lineEdit_SaveDir_predict);
        updateFileDict(ui->lineEdit_SaveDir_postporcess);
        updateFileDict(ui->lineEdit_SaveDir_calculate);
        updateCurrentImage();
        inpaintingModeFlag = false;
    } else if (senderButton == ui->pushButton_SaveDir_postporcess) {
        ui->lineEdit_SaveDir_postporcess->setText(dirPath);
    } else if (senderButton == ui->pushButton_SaveDir_cacuate) {
        ui->lineEdit_SaveDir_calculate->setText(dirPath);
    } else if (senderButton == ui->pushButton_SaveDir_warp) {
        ui->lineEdit_SaveDir_warp->setText(dirPath);
    }
}

void GF_rootAnalysis::selectFilePath()
{
    QStringList filePaths = QFileDialog::getOpenFileName(this, "Open file", "./").split(";");
    if (filePaths.isEmpty() || filePaths.first().isEmpty()) {
        return;
    }
    
    QPushButton *senderButton = qobject_cast<QPushButton*>(sender());
    if (!senderButton) {
        return;
    }
    
    if (senderButton == ui->pushButton_WeightPath) {
        ui->lineEdit_WeighPath->setText(filePaths.first());
    }
}

void GF_rootAnalysis::updateByLineEdit()
{
    QLineEdit *senderLineEdit = qobject_cast<QLineEdit*>(sender());
    if (!senderLineEdit) {
        return;
    }
    
    if (senderLineEdit == ui->lineEdit_SaveDir_predict) {
        predictSaveDir = senderLineEdit->text();
    } else if (senderLineEdit == ui->lineEdit_SaveDir_postporcess) {
        postprocessSaveDir = senderLineEdit->text();
    } else if (senderLineEdit == ui->lineEdit_SaveDir_calculate) {
        traitsSaveDir = senderLineEdit->text();
    } else if (senderLineEdit == ui->lineEdit_SaveDir_warp) {
        warpSaveDir = senderLineEdit->text();
    }
    updateFileDict(senderLineEdit);
}

void GF_rootAnalysis::updateFileDict(QWidget *sender)
{
    if (datasetRootPath.isEmpty()) {
        return;
    }
    
    if (sender == ui->lineEdit_SaveDir_calculate) {
        if (traitsSaveDir.isEmpty()) {
            return;
        }
        updateTextBrowser("try to update traits");
        
        // 读取traits.csv文件
        QString traitsCsvPath = traitsSaveDir + "/traits.csv";
        QFile file(traitsCsvPath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString header = in.readLine();
            int num = 0;
            
            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList fields = line.split(",");
                if (fields.size() > 0) {
                    QString imagePath = fields[0];
                    if (fileDict.contains(imagePath)) {
                        QMap<QString, QString> traits;
                        // 解析CSV行到traits映射
                        // 这里需要根据实际CSV格式进行调整
                        fileDict[imagePath]["traits"] = line;
                        num++;
                        if (imagePath == currentImage["image_path"].toString()) {
                            currentImage["traits"] = line;
                            updateTraitsTable();
                        }
                    }
                }
            }
            file.close();
            updateTextBrowser(QString("update traits success: %1, total: %2").arg(num).arg(fileDict.size()));
        } else {
            updateTextBrowser(QString("can't find traits.csv in %1").arg(traitsSaveDir));
        }
        
        // 更新可视化图像
        updateTextBrowser("try to update traits visualization");
        QString visualizationDir = traitsSaveDir + "/traits_visualization";
        QDir visDir(visualizationDir);
        if (visDir.exists()) {
            int num = 0;
            QFileInfoList fileInfoList = visDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
            for (const QFileInfo &fileInfo : fileInfoList) {
                if (fileInfo.isFile() && (fileInfo.suffix() == "png" || fileInfo.suffix() == "jpg" || fileInfo.suffix() == "bmp")) {
                    QString filePath = fileInfo.absoluteFilePath();
                    QString imagePath = filePath.replace(visualizationDir, datasetRootPath);
                    if (fileDict.contains(imagePath)) {
                        fileDict[imagePath]["visualization"] = filePath;
                        num++;
                        if (imagePath == currentImage["image_path"].toString()) {
                            currentImage["visualization"] = filePath;
                            updateCurrentImage();
                        }
                    }
                }
            }
            updateTextBrowser(QString("update visualization success: %1, total: %2").arg(num).arg(fileDict.size()));
        } else {
            updateTextBrowser(QString("can't find traits_visualization directory in %1").arg(traitsSaveDir));
        }
        return;
    }
    
    QString dirPath;
    QString pathType;
    
    if (sender == ui->lineEdit_SaveDir_predict) {
        dirPath = predictSaveDir;
        pathType = "binary_path";
    } else if (sender == ui->lineEdit_SaveDir_postporcess) {
        dirPath = postprocessSaveDir;
        pathType = "processed_path";
    } else {
        return;
    }
    
    if (dirPath.isEmpty()) {
        return;
    }
    
    updateTextBrowser(QString("try to update %1 images").arg(pathType.split("_").first()));
    
    QDir dir(dirPath);
    if (dir.exists()) {
        int num = 0;
        QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QFileInfo &fileInfo : fileInfoList) {
            if (fileInfo.isFile() && (fileInfo.suffix() == "png" || fileInfo.suffix() == "jpg" || fileInfo.suffix() == "bmp")) {
                QString filePath = fileInfo.absoluteFilePath();
                QString imagePath = filePath.replace(dirPath, datasetRootPath);
                if (fileDict.contains(imagePath)) {
                    fileDict[imagePath][pathType] = filePath;
                    num++;
                }
            }
        }
        updateTextBrowser(QString("update %1 success: %2, total: %3").arg(pathType).arg(num).arg(fileDict.size()));
        
        if (!currentImage["thresholdseg"].isValid() && !currentImage["gray"].isValid() && !currentImage["processed"].isValid()) {
            updateCurrentImage();
        }
    } else {
        updateTextBrowser(QString("update %1 error: directory does not exist").arg(pathType));
    }
}

void GF_rootAnalysis::inpaintingMode()
{
    QPushButton *senderButton = qobject_cast<QPushButton*>(sender());
    if (!senderButton) {
        return;
    }
    
    if (datasetRootPath.isEmpty()) {
        updateTextBrowser("please select dataset rootpath");
        inpaintingModeFlag = true;
    }
    
    if (senderButton->text() == "Quit Inpainting" || senderButton->text() == "Quit") {
        ui->graphicsView_Main3->setDrawMode(false);
        ui->graphicsView_Main2->setDrawMode(false);
        ui->graphicsView_Main->setDrawMode(false);
        ui->pushButton_Mask->setVisible(false);
        ui->pushButton_Paint->setVisible(false);
        ui->pushButton_Erase->setVisible(false);
        
        if (senderButton == ui->pushButton_Inpainting) {
            senderButton->setText("Inpainting");
        } else if (senderButton == ui->pushButton_ManualWarp) {
            senderButton->setText("manual");
        }
        
        inpaintingModeFlag = false;
        return;
    }
    
    if (senderButton == ui->pushButton_Inpainting) {
        if (!currentImage["binary"].isValid() && !inpaintingModeFlag) {
            QMessageBox::warning(this, "Warning", "Please segment first!");
            return;
        }
        
        if (!currentImage["processed"].isValid()) {
            if (currentImage["binary"].isValid()) {
                currentImage["processed"] = currentImage["binary"];
            } else if (currentImage["thresholdseg"].isValid()) {
                currentImage["processed"] = currentImage["thresholdseg"];
            }
            if (currentImage["processed"].isValid()) {
                updateGraphicsView(currentImage["processed"].toString(), ui->graphicsView_Main3);
            }
        }
        
        ui->splitter_Right->setSizes({1, 0});
        ui->graphicsView_Main3->setDrawMode(true);
        senderButton->setText("Quit Inpainting");
        ui->pushButton_Paint->setVisible(true);
        ui->pushButton_Erase->setVisible(true);
        inpaintingModeFlag = true;
    } else if (senderButton == ui->pushButton_ManualWarp) {
        if (!currentImage["image_path"].isValid()) {
            QMessageBox::warning(this, "Warning", "Please select one image!");
            return;
        }
        
        if (!currentImage["binary"].isValid()) {
            cv::Mat binary = cv::Mat::zeros(currentImage["img"].value<cv::Mat>().rows, currentImage["img"].value<cv::Mat>().cols, CV_8UC1);
            currentImage["binary"] = QVariant::fromValue(binary);
            updateGraphicsView(currentImage["binary"].toString(), ui->graphicsView_Main2);
        }
        
        ui->graphicsView_Main->setDrawMode(true);
        ui->graphicsView_Main2->setDrawMode(true);
        senderButton->setText("Quit");
        ui->pushButton_Mask->setVisible(true);
        ui->pushButton_Paint->setVisible(true);
        ui->pushButton_Erase->setVisible(true);
    }
}

void GF_rootAnalysis::paintCurrentImage()
{
    QPushButton *senderButton = qobject_cast<QPushButton*>(sender());
    if (!senderButton) {
        return;
    }
    
    QString saveDir;
    MyGraphicsView *graphicsView = nullptr;
    cv::Mat img;
    
    if (ui->pushButton_Inpainting->text() == "Quit Inpainting") {
        saveDir = postprocessSaveDir;
        if (saveDir.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Please select a save dir!");
            return;
        }
        graphicsView = ui->graphicsView_Main3;
        if (currentImage["processed"].isValid()) {
            img = currentImage["processed"].value<cv::Mat>();
        } else if (currentImage["binary"].isValid()) {
            img = currentImage["binary"].value<cv::Mat>();
        }
    } else if (ui->pushButton_ManualWarp->text() == "Quit") {
        if (predictSaveDir.isEmpty()) {
            updateTextBrowser("please select a predict save dir");
            return;
        }
        saveDir = predictSaveDir;
        
        if (senderButton == ui->pushButton_Mask) {
            graphicsView = ui->graphicsView_Main;
            if (currentImage["binary"].isValid()) {
                img = currentImage["binary"].value<cv::Mat>();
            } else {
                img = cv::Mat::zeros(currentImage["img"].value<cv::Mat>().rows, currentImage["img"].value<cv::Mat>().cols, CV_8UC1);
            }
        } else {
            graphicsView = ui->graphicsView_Main2;
            img = currentImage["binary"].value<cv::Mat>();
        }
    }
    
    QString savePath = currentImage["image_path"].toString().replace(datasetRootPath, saveDir);
    QList<QPolygonF> polygons = graphicsView->getPolygons();
    
    if (polygons.isEmpty()) {
        return;
    }
    
    uchar color = (senderButton == ui->pushButton_Erase) ? 0 : 255;
    
    for (const QPolygonF &polygon : polygons) {
        std::vector<cv::Point> pts;
        for (const QPointF &point : polygon) {
            pts.push_back(cv::Point(point.x(), point.y()));
        }
        std::vector<std::vector<cv::Point>> contours;
        contours.push_back(pts);
        cv::fillPoly(img, contours, cv::Scalar(color));
    }
    
    if (ui->pushButton_ManualWarp->text() == "Quit") {
        updateGraphicsView(currentImage["binary"].toString(), ui->graphicsView_Main2, false);
        currentImage["binary"] = QVariant::fromValue(img);
    }
    
    if (ui->pushButton_Inpainting->text() == "Quit Inpainting") {
        try {
            updateGraphicsView(currentImage["processed"].toString(), ui->graphicsView_Main3, false);
        } catch (const std::exception &e) {
            updateTextBrowser(QString("Error: %1").arg(e.what()));
        }
        currentImage["processed"] = QVariant::fromValue(img);
    }
    
    QDir saveDirObj(QFileInfo(savePath).absolutePath());
    if (!saveDirObj.exists()) {
        saveDirObj.mkpath(".");
    }
    
    cv::imwrite(savePath.toStdString(), img);
    updateTextBrowser(QString("saved at %1").arg(savePath));
}

cv::Mat GF_rootAnalysis::autoInpainting(const cv::Mat &img)
{
    int times = ui->spinBox_AutoInpainting->value();
    if (times == 0) {
        return img;
    }
    
    // 找到roi
    std::vector<cv::Point> points;
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            if (img.at<uchar>(i, j) == 255) {
                points.push_back(cv::Point(j, i));
            }
        }
    }
    
    if (points.empty()) {
        return img;
    }
    
    cv::Rect roi = cv::boundingRect(points);
    cv::Mat roiImg = img(roi);
    cv::resize(roiImg, roiImg, cv::Size(384, 640));
    cv::threshold(roiImg, roiImg, 127, 1, cv::THRESH_BINARY);
    roiImg = roiImg.reshape(1, 1);
    roiImg.convertTo(roiImg, CV_32F);
    
    // 使用ONNX模型进行修复
    // 这里需要实现ONNX Runtime的调用
    
    // 临时返回原始图像
    return img;
}

void GF_rootAnalysis::updateCurrentImage()
{
    QObject *senderObj = sender();
    QString currentImagePath = currentImage["image_path"].toString();
    
    if (currentImagePath.isEmpty() && senderObj != ui->treeWidget_Files) {
        return;
    }
    
    if (senderObj == ui->pushButton_FirstImage) {
        if (!fileDict.isEmpty()) {
            currentImagePath = fileDict.keys().first();
        }
    } else if (senderObj == ui->pushButton_LastImage) {
        if (!fileDict.isEmpty()) {
            currentImagePath = fileDict.keys().last();
        }
    } else if (senderObj == ui->pushButton_NextImage) {
        if (!currentImagePath.isEmpty() && fileDict.contains(currentImagePath)) {
            int index = fileDict.keys().indexOf(currentImagePath);
            if (index < fileDict.size() - 1) {
                currentImagePath = fileDict.keys().at(index + 1);
            }
        }
    } else if (senderObj == ui->pushButton_PreviousImage) {
        if (!currentImagePath.isEmpty() && fileDict.contains(currentImagePath)) {
            int index = fileDict.keys().indexOf(currentImagePath);
            if (index > 0) {
                currentImagePath = fileDict.keys().at(index - 1);
            }
        }
    } else if (senderObj == ui->treeWidget_Files) {
        QTreeWidgetItem *item = ui->treeWidget_Files->currentItem();
        if (item && item->childCount() == 0) {
            currentImagePath = item->text(1);
        }
    }
    
    if (!currentImagePath.isEmpty()) {
        currentImage = {
            {"image_path", currentImagePath},
            {"img", QVariant::fromValue(cv::imread(currentImagePath.toStdString()))},
            {"gray", QVariant()},
            {"binary", QVariant()},
            {"thresholdseg", QVariant()},
            {"processed", QVariant()},
            {"traits", QVariant()},
            {"visualization", QVariant()}
        };
        
        if (fileDict.contains(currentImagePath)) {
            if (!fileDict[currentImagePath]["binary_path"].isEmpty()) {
                currentImage["binary"] = QVariant::fromValue(cv::imread(fileDict[currentImagePath]["binary_path"].toStdString(), cv::IMREAD_GRAYSCALE));
            }
            if (!fileDict[currentImagePath]["processed_path"].isEmpty()) {
                currentImage["processed"] = QVariant::fromValue(cv::imread(fileDict[currentImagePath]["processed_path"].toStdString(), cv::IMREAD_GRAYSCALE));
            }
            if (!fileDict[currentImagePath]["traits"].isEmpty()) {
                currentImage["traits"] = fileDict[currentImagePath]["traits"];
            }
            if (!fileDict[currentImagePath]["visualization"].isEmpty()) {
                currentImage["visualization"] = fileDict[currentImagePath]["visualization"];
            }
        }
        
        cv::Mat img = currentImage["img"].value<cv::Mat>();
        ui->label_ImageSize->setText(QString("image size: %1x%2").arg(img.rows).arg(img.cols));
        ui->label_CurrentImage->setText(currentImagePath);
        updateGraphicsView(currentImagePath);
        updateTraitsTable();
    }
}

void GF_rootAnalysis::updateGraphicsView(const QString &imagePath, MyGraphicsView *graphicsView, bool fitInView)
{
    QList<MyGraphicsView*> graphicsViews = {ui->graphicsView_Main, ui->graphicsView_Main2, ui->graphicsView_Main3};
    QList<QString> images;
    
    if (graphicsView) {
        graphicsView->setVisible(true);
        images = {imagePath};
    } else if (fileDict.contains(imagePath) && !fileDict[imagePath]["visualization"].isEmpty() && ui->tabWidget_Processing->currentWidget() == ui->tab_Analysis) {
        images = {imagePath, fileDict[imagePath]["binary_path"], fileDict[imagePath]["visualization"]};
    } else if (fileDict.contains(imagePath)) {
        images = {imagePath, fileDict[imagePath]["binary_path"], fileDict[imagePath]["processed_path"]};
    }
    
    for (int i = 0; i < graphicsViews.size(); i++) {
        if (i < images.size() && !images[i].isEmpty()) {
            graphicsViews[i]->cleanItems();
            graphicsViews[i]->setVisible(true);
        } else {
            graphicsViews[i]->setVisible(false);
        }
    }
    
    for (int i = 0; i < images.size(); i++) {
        MyGraphicsView *view = nullptr;
        if (i == 0) {
            view = graphicsView ? graphicsView : graphicsViews[i];
        } else if (i == 2) {
            view = graphicsViews[2];
        } else {
            view = graphicsViews[1];
        }
        
        if (view && !images[i].isEmpty()) {
            QImage qimage;
            if (QFile::exists(images[i])) {
                qimage.load(images[i]);
            } else if (currentImage.contains(images[i]) && currentImage[images[i]].canConvert<cv::Mat>()) {
                cv::Mat mat = currentImage[images[i]].value<cv::Mat>();
                if (mat.channels() == 3) {
                    cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
                    qimage = QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).copy();
                } else if (mat.channels() == 1) {
                    qimage = QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
                }
            }
            
            if (!qimage.isNull()) {
                QPixmap pixmap = QPixmap::fromImage(qimage);
                view->setPixmap(pixmap, fitInView);
                
                if (view == ui->graphicsView_Main) {
                    ui->label_ImageSize->setText(QString("image size: %1x%2").arg(qimage.width()).arg(qimage.height()));
                }
            }
        }
    }
}

void GF_rootAnalysis::datasetSplitBalance()
{
    double trainValue = ui->doubleSpinBox_DatasetSplit_train->value();
    double valValue = ui->doubleSpinBox_DatasetSplit_val->value();
    
    if (trainValue + valValue == 1.0) {
        return;
    }
    
    QDoubleSpinBox *senderSpinBox = qobject_cast<QDoubleSpinBox*>(sender());
    if (!senderSpinBox) {
        return;
    }
    
    if (senderSpinBox == ui->doubleSpinBox_DatasetSplit_train) {
        ui->doubleSpinBox_DatasetSplit_val->setValue(1.0 - trainValue);
    } else if (senderSpinBox == ui->doubleSpinBox_DatasetSplit_val) {
        ui->doubleSpinBox_DatasetSplit_train->setValue(1.0 - valValue);
    }
}

void GF_rootAnalysis::updateIsSlide()
{
    if (ui->checkBox_IsSlide->isChecked()) {
        ui->spinBox_CropSize_predict->setEnabled(true);
        ui->spinBox_Stride->setEnabled(true);
    } else {
        ui->spinBox_CropSize_predict->setEnabled(false);
        ui->spinBox_Stride->setEnabled(false);
    }
}

void GF_rootAnalysis::updateThresholdSegmentation()
{
    if (!currentImage["image_path"].isValid() || !QFile::exists(currentImage["image_path"].toString())) {
        if (ui->comboBox_ColorPlane->currentIndex() != 0) {
            updateTextBrowser("please select an existing image!");
        }
        ui->comboBox_ColorPlane->setCurrentIndex(0);
        return;
    }
    
    QString colorplane = ui->comboBox_ColorPlane->currentText().left(1);
    
    if (sender() == ui->comboBox_ColorPlane) {
        if (colorplane == "N") {
            ui->comboBox_Method->setCurrentIndex(0);
            ui->comboBox_Method->setEnabled(false);
            ui->doubleSpinBox_Threshold->setEnabled(false);
            updateGraphicsView(currentImage["image_path"].toString());
            currentImage["img"] = QVariant();
            currentImage["gray"] = QVariant();
            currentImage["thresholdseg"] = QVariant();
            currentImage["processed"] = QVariant();
            return;
        } else {
            if (!currentImage["img"].isValid()) {
                currentImage["img"] = QVariant::fromValue(cv::imread(currentImage["image_path"].toString().toStdString()));
            }
            ui->comboBox_Method->setEnabled(true);
        }
        
        cv::Mat img = currentImage["img"].value<cv::Mat>();
        cv::Mat gray;
        
        if (colorplane == "R") {
            std::vector<cv::Mat> channels;
            cv::split(img, channels);
            gray = channels[2]; // BGR format
        } else if (colorplane == "G") {
            std::vector<cv::Mat> channels;
            cv::split(img, channels);
            gray = channels[1];
        } else if (colorplane == "B") {
            std::vector<cv::Mat> channels;
            cv::split(img, channels);
            gray = channels[0];
        } else if (colorplane == "H") {
            cv::Mat hsv;
            cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);
            std::vector<cv::Mat> channels;
            cv::split(hsv, channels);
            gray = channels[0];
        } else if (colorplane == "S") {
            cv::Mat hsv;
            cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);
            std::vector<cv::Mat> channels;
            cv::split(hsv, channels);
            gray = channels[1];
        } else if (colorplane == "V") {
            cv::Mat hsv;
            cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);
            std::vector<cv::Mat> channels;
            cv::split(hsv, channels);
            gray = channels[2];
        }
        
        currentImage["gray"] = QVariant::fromValue(gray);
    }
    
    QString method = ui->comboBox_Method->currentText();
    cv::Mat img = currentImage["gray"].value<cv::Mat>();
    
    if (method == "manual threshold") {
        ui->doubleSpinBox_Threshold->setEnabled(true);
        double threshold = ui->doubleSpinBox_Threshold->value();
        cv::threshold(img, img, threshold, 255, cv::THRESH_BINARY);
        cv::medianBlur(img, img, 5);
        cv::medianBlur(img, img, 5);
        cv::medianBlur(img, img, 5);
        currentImage["thresholdseg"] = QVariant::fromValue(img);
    } else if (method == "Adaptive threshold(OTSU)") {
        ui->doubleSpinBox_Threshold->setEnabled(false);
        double threshold;
        cv::threshold(img, img, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
        cv::medianBlur(img, img, 5);
        cv::medianBlur(img, img, 5);
        cv::medianBlur(img, img, 5);
        ui->doubleSpinBox_Threshold->setValue(threshold);
        currentImage["thresholdseg"] = QVariant::fromValue(img);
    }
    
    ui->graphicsView_Main2->setVisible(true);
    updateGraphicsView(currentImage["thresholdseg"].toString(), ui->graphicsView_Main2);
}

void GF_rootAnalysis::updateTreeWidgetProcessingConfig()
{
    // 实现处理配置树的更新逻辑
}

void GF_rootAnalysis::updateTraitsTable()
{
    if (!currentImage["traits"].isValid()) {
        traitsInit();
        return;
    }
    
    QString traitsStr = currentImage["traits"].toString();
    QStringList traits = traitsStr.split(",");
    
    // 这里需要根据实际CSV格式进行调整
    if (traits.size() > 0) {
        // 更新各个特征值到表格
    }
}

void GF_rootAnalysis::traitsInit()
{
    // 初始化特征表格
}

void GF_rootAnalysis::onProcessingParamChanged()
{
    processingTimer->start(500); // 500ms防抖
}

void GF_rootAnalysis::_doUpdateProcessing()
{
    // 实现参数变化后的处理逻辑
}

void GF_rootAnalysis::startSegmentation()
{
    // 实现分割功能
}

void GF_rootAnalysis::stopThread()
{
    // 实现线程停止功能
}

void GF_rootAnalysis::saveProcessedImage()
{
    // 实现保存处理后的图像
}

void GF_rootAnalysis::startProcess()
{
    // 实现批量处理功能
}

void GF_rootAnalysis::startCalculate()
{
    // 实现特征计算功能
}

void GF_rootAnalysis::startTrain()
{
    // 设置训练参数
    modelTrainer->setDatasetPath(ui->lineEdit_DatasetPath->text());
    modelTrainer->setSaveDir(ui->lineEdit_SaveDir_train->text());
    modelTrainer->setModelType(ui->comboBox_TrainableModel->currentText());
    modelTrainer->setBatchSize(ui->spinBox_BatchSize_train->value());
    modelTrainer->setEpochs(ui->spinBox_Epochs_train->value());
    modelTrainer->setLearningRate(ui->doubleSpinBox_LearningRate_train->value());
    modelTrainer->setDatasetSplit(ui->doubleSpinBox_DatasetSplit_train->value(), ui->doubleSpinBox_DatasetSplit_val->value());
    
    // 开始训练
    if (modelTrainer->startTraining()) {
        updateTextBrowser("Training started");
    } else {
        updateTextBrowser("Failed to start training: " + modelTrainer->getTrainingStatus());
    }
    
    // 定期更新训练状态
    QTimer *trainTimer = new QTimer(this);
    connect(trainTimer, &QTimer::timeout, this, [this, trainTimer]() {
        if (!modelTrainer->isTraining()) {
            trainTimer->stop();
            trainTimer->deleteLater();
        }
        updateTextBrowser("Training status: " + modelTrainer->getTrainingStatus());
        updateTextBrowser("Training progress: " + QString::number(modelTrainer->getTrainingProgress() * 100) + "%");
    });
    trainTimer->start(1000); // 每秒更新一次
}
