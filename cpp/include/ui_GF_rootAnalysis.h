/********************************************************************************
** Form generated from reading UI file 'GF_rootAnalysis.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GF_ROOTANALYSIS_H
#define UI_GF_ROOTANALYSIS_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "drawGraphicsview.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionimport_image;
    QAction *actionimport_image_set;
    QAction *actionExit;
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QSplitter *splitter_Middle;
    QSplitter *splitter_Left;
    QFrame *frame_files;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_DataDir;
    QLabel *label_DataDir;
    QLabel *label_DataDirShow;
    QPushButton *pushButton_DataDir;
    QTreeWidget *treeWidget_Files;
    QFrame *frame_Processing;
    QVBoxLayout *verticalLayout_7;
    QTabWidget *tabWidget_Processing;
    QWidget *tab_Predict;
    QVBoxLayout *verticalLayout_tab_Segmentation;
    QWidget *widget_Predict;
    QGridLayout *gridLayout;
    QLineEdit *lineEdit_WeighPath;
    QComboBox *comboBox_SegmentationModel;
    QLabel *label_IsSlide;
    QSpinBox *spinBox_Stride;
    QLabel *label_CropSize;
    QLabel *label_WeightPath;
    QPushButton *pushButton_WeightPath;
    QLabel *label_SaveDir_predict;
    QLabel *label_Stride;
    QLineEdit *lineEdit_SaveDir_predict;
    QCheckBox *checkBox_IsSlide;
    QLabel *label_Model;
    QPushButton *pushButton_SaveDir_predict;
    QSpinBox *spinBox_CropSize_predict;
    QLabel *label_GPU_id;
    QSpinBox *spinBox_GPU_id;
    QGroupBox *groupBox_Warp;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_SaveDir_warp;
    QLabel *label_SaveDir_warp;
    QLineEdit *lineEdit_SaveDir_warp;
    QPushButton *pushButton_SaveDir_warp;
    QHBoxLayout *horizontalLayout_TargetWidth;
    QLabel *label_TargetWidth;
    QSpinBox *spinBox_TargetWidth;
    QSpacerItem *horizontalSpacer_TargetWidth;
    QHBoxLayout *horizontalLayout_TargetHeight;
    QLabel *label_TargetHeight;
    QSpinBox *spinBox_TargetHeight;
    QSpacerItem *horizontalSpacer_TargetHeight;
    QHBoxLayout *horizontalLayout_btnWarp;
    QSpacerItem *horizontalSpacer_Warp;
    QPushButton *pushButton_ManualWarp;
    QPushButton *pushButton_Warp;
    QPushButton *pushButton_WarpAll;
    QPushButton *pushButton_StopWarp;
    QSpacerItem *verticalSpacer_Predict;
    QHBoxLayout *horizontalLayout_Predict;
    QSpacerItem *horizontalSpacer_Predict;
    QPushButton *pushButton_PredictOne;
    QPushButton *pushButton_PredictAll;
    QPushButton *pushButton_StopPredict;
    QWidget *tab_PostProcessing;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox_ThresholdSegmentation;
    QFormLayout *formLayout;
    QLabel *label_ColorPlane;
    QComboBox *comboBox_ColorPlane;
    QLabel *label_Method;
    QComboBox *comboBox_Method;
    QLabel *label_Threshold;
    QDoubleSpinBox *doubleSpinBox_Threshold;
    QLabel *label;
    QGroupBox *groupBox_ImageProcessing;
    QVBoxLayout *verticalLayout_5;
    QTreeWidget *treeWidget_ProcessingConfig;
    QHBoxLayout *horizontalLayout_SaveDir_postprocess;
    QLabel *label_SaveDir_postporcess;
    QLineEdit *lineEdit_SaveDir_postporcess;
    QPushButton *pushButton_SaveDir_postporcess;
    QHBoxLayout *horizontalLayout_PostProcess;
    QSpacerItem *horizontalSpacer_PostProcessing;
    QPushButton *pushButton_SaveThisImage;
    QPushButton *pushButton_ProcessingAllImage;
    QPushButton *pushButton_StopProcess;
    QWidget *tab_Analysis;
    QVBoxLayout *verticalLayout_2;
    QGridLayout *gridLayout_LayerSize;
    QSpinBox *spinBox_LayerWidth;
    QLabel *label_LayerWidth;
    QSpinBox *spinBox_LayerHeight;
    QLabel *label_LayerHeight;
    QDoubleSpinBox *doubleSpinBox_LayerHeight;
    QDoubleSpinBox *doubleSpinBox_LayerWidth;
    QTreeWidget *treeWidget_Traits;
    QHBoxLayout *horizontalLayout_SacveDir_calculate;
    QLabel *label_SaveDir_calculate;
    QLineEdit *lineEdit_SaveDir_calculate;
    QPushButton *pushButton_SaveDir_cacuate;
    QHBoxLayout *horizontalLayout_Analysis;
    QSpacerItem *horizontalSpacer_Analysis;
    QPushButton *pushButton_Calculate;
    QPushButton *pushButton_CalculateAll;
    QPushButton *pushButton_StopCalculate;
    QWidget *tab_Train;
    QVBoxLayout *_2;
    QTreeWidget *treeWidget_Train;
    QSpacerItem *verticalSpacer_Train;
    QHBoxLayout *horizontalLayout_Train;
    QSpacerItem *horizontalSpacer_Train;
    QPushButton *pushButton_StartTrain;
    QPushButton *pushButton_StopTrain;
    QComboBox *comboBox_TrainableModel;
    QSpinBox *spinBox_Iters;
    QSpinBox *spinBox_BatchSize;
    QDoubleSpinBox *doubleSpinBox_Lr;
    QLineEdit *lineEdit_DatasetPath;
    QPushButton *pushButton_DatasetPath;
    QLineEdit *lineEdit_SaveDir_train;
    QPushButton *pushButton_SaveDir_train;
    QDoubleSpinBox *doubleSpinBox_DatasetSplit_train;
    QDoubleSpinBox *doubleSpinBox_DatasetSplit_val;
    QLineEdit *lineEdit_ResumeModel;
    QPushButton *pushButton_ResumeModel;
    QSpinBox *spinBox_LogIters;
    QSpinBox *spinBox_SaveInterval;
    QSpinBox *spinBox_KeepCheckpointMax;
    QSpinBox *spinBox_CropSize_train;
    QSplitter *splitter_Right;
    QFrame *frame_imageview;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout_imageview;
    DrawGraphicsView *graphicsView_Main;
    DrawGraphicsView *graphicsView_Main2;
    DrawGraphicsView *graphicsView_Main3;
    QHBoxLayout *horizontalLayout_ImageInfo;
    QLabel *label_ImageSize;
    QLabel *label_CurrentImage;
    QHBoxLayout *horizontalLayout_btns;
    QSpacerItem *horizontalSpacer_btnsLeft;
    QPushButton *pushButton_Mask;
    QPushButton *pushButton_Paint;
    QPushButton *pushButton_Erase;
    QPushButton *pushButton_FirstImage;
    QPushButton *pushButton_PreviousImage;
    QPushButton *pushButton_NextImage;
    QPushButton *pushButton_LastImage;
    QPushButton *pushButton_FitAllImageView;
    QSpacerItem *horizontalSpacer_btnsRight;
    QFrame *frame_MessagePrinter;
    QHBoxLayout *horizontalLayout_4;
    QTextBrowser *textBrowser_MessagePrinter;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1060, 792);
        QIcon icon;
        icon.addFile(QString::fromUtf8("C:/Users/w1863/.designer/gf.ico"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        actionimport_image = new QAction(MainWindow);
        actionimport_image->setObjectName(QString::fromUtf8("actionimport_image"));
        actionimport_image_set = new QAction(MainWindow);
        actionimport_image_set->setObjectName(QString::fromUtf8("actionimport_image_set"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        splitter_Middle = new QSplitter(centralwidget);
        splitter_Middle->setObjectName(QString::fromUtf8("splitter_Middle"));
        splitter_Middle->setOrientation(Qt::Horizontal);
        splitter_Left = new QSplitter(splitter_Middle);
        splitter_Left->setObjectName(QString::fromUtf8("splitter_Left"));
        splitter_Left->setOrientation(Qt::Vertical);
        frame_files = new QFrame(splitter_Left);
        frame_files->setObjectName(QString::fromUtf8("frame_files"));
        frame_files->setFrameShape(QFrame::StyledPanel);
        frame_files->setFrameShadow(QFrame::Raised);
        verticalLayout_3 = new QVBoxLayout(frame_files);
        verticalLayout_3->setSpacing(10);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(10, 10, 10, 10);
        horizontalLayout_DataDir = new QHBoxLayout();
        horizontalLayout_DataDir->setObjectName(QString::fromUtf8("horizontalLayout_DataDir"));
        label_DataDir = new QLabel(frame_files);
        label_DataDir->setObjectName(QString::fromUtf8("label_DataDir"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_DataDir->sizePolicy().hasHeightForWidth());
        label_DataDir->setSizePolicy(sizePolicy);
        label_DataDir->setMinimumSize(QSize(0, 0));

        horizontalLayout_DataDir->addWidget(label_DataDir);

        label_DataDirShow = new QLabel(frame_files);
        label_DataDirShow->setObjectName(QString::fromUtf8("label_DataDirShow"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_DataDirShow->sizePolicy().hasHeightForWidth());
        label_DataDirShow->setSizePolicy(sizePolicy1);

        horizontalLayout_DataDir->addWidget(label_DataDirShow);

        pushButton_DataDir = new QPushButton(frame_files);
        pushButton_DataDir->setObjectName(QString::fromUtf8("pushButton_DataDir"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(pushButton_DataDir->sizePolicy().hasHeightForWidth());
        pushButton_DataDir->setSizePolicy(sizePolicy2);
        pushButton_DataDir->setMaximumSize(QSize(50, 16777215));

        horizontalLayout_DataDir->addWidget(pushButton_DataDir);


        verticalLayout_3->addLayout(horizontalLayout_DataDir);

        treeWidget_Files = new QTreeWidget(frame_files);
        treeWidget_Files->setObjectName(QString::fromUtf8("treeWidget_Files"));
        treeWidget_Files->header()->setVisible(false);

        verticalLayout_3->addWidget(treeWidget_Files);

        splitter_Left->addWidget(frame_files);
        frame_Processing = new QFrame(splitter_Left);
        frame_Processing->setObjectName(QString::fromUtf8("frame_Processing"));
        frame_Processing->setFrameShape(QFrame::StyledPanel);
        frame_Processing->setFrameShadow(QFrame::Raised);
        verticalLayout_7 = new QVBoxLayout(frame_Processing);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        tabWidget_Processing = new QTabWidget(frame_Processing);
        tabWidget_Processing->setObjectName(QString::fromUtf8("tabWidget_Processing"));
        tab_Predict = new QWidget();
        tab_Predict->setObjectName(QString::fromUtf8("tab_Predict"));
        verticalLayout_tab_Segmentation = new QVBoxLayout(tab_Predict);
        verticalLayout_tab_Segmentation->setSpacing(9);
        verticalLayout_tab_Segmentation->setObjectName(QString::fromUtf8("verticalLayout_tab_Segmentation"));
        verticalLayout_tab_Segmentation->setContentsMargins(3, 3, 3, 3);
        widget_Predict = new QWidget(tab_Predict);
        widget_Predict->setObjectName(QString::fromUtf8("widget_Predict"));
        gridLayout = new QGridLayout(widget_Predict);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        lineEdit_WeighPath = new QLineEdit(widget_Predict);
        lineEdit_WeighPath->setObjectName(QString::fromUtf8("lineEdit_WeighPath"));

        gridLayout->addWidget(lineEdit_WeighPath, 3, 1, 1, 1);

        comboBox_SegmentationModel = new QComboBox(widget_Predict);
        comboBox_SegmentationModel->setObjectName(QString::fromUtf8("comboBox_SegmentationModel"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(comboBox_SegmentationModel->sizePolicy().hasHeightForWidth());
        comboBox_SegmentationModel->setSizePolicy(sizePolicy3);

        gridLayout->addWidget(comboBox_SegmentationModel, 0, 1, 1, 2);

        label_IsSlide = new QLabel(widget_Predict);
        label_IsSlide->setObjectName(QString::fromUtf8("label_IsSlide"));
        sizePolicy.setHeightForWidth(label_IsSlide->sizePolicy().hasHeightForWidth());
        label_IsSlide->setSizePolicy(sizePolicy);
        label_IsSlide->setMaximumSize(QSize(121, 25));

        gridLayout->addWidget(label_IsSlide, 5, 0, 1, 1);

        spinBox_Stride = new QSpinBox(widget_Predict);
        spinBox_Stride->setObjectName(QString::fromUtf8("spinBox_Stride"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(spinBox_Stride->sizePolicy().hasHeightForWidth());
        spinBox_Stride->setSizePolicy(sizePolicy4);
        spinBox_Stride->setMaximumSize(QSize(383, 28));
        spinBox_Stride->setMaximum(999999999);
        spinBox_Stride->setValue(768);

        gridLayout->addWidget(spinBox_Stride, 7, 1, 1, 2);

        label_CropSize = new QLabel(widget_Predict);
        label_CropSize->setObjectName(QString::fromUtf8("label_CropSize"));
        sizePolicy.setHeightForWidth(label_CropSize->sizePolicy().hasHeightForWidth());
        label_CropSize->setSizePolicy(sizePolicy);
        label_CropSize->setMaximumSize(QSize(121, 28));

        gridLayout->addWidget(label_CropSize, 6, 0, 1, 1);

        label_WeightPath = new QLabel(widget_Predict);
        label_WeightPath->setObjectName(QString::fromUtf8("label_WeightPath"));
        sizePolicy.setHeightForWidth(label_WeightPath->sizePolicy().hasHeightForWidth());
        label_WeightPath->setSizePolicy(sizePolicy);
        label_WeightPath->setMaximumSize(QSize(121, 40));

        gridLayout->addWidget(label_WeightPath, 3, 0, 1, 1);

        pushButton_WeightPath = new QPushButton(widget_Predict);
        pushButton_WeightPath->setObjectName(QString::fromUtf8("pushButton_WeightPath"));
        QSizePolicy sizePolicy5(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(pushButton_WeightPath->sizePolicy().hasHeightForWidth());
        pushButton_WeightPath->setSizePolicy(sizePolicy5);
        pushButton_WeightPath->setMaximumSize(QSize(70, 16777215));

        gridLayout->addWidget(pushButton_WeightPath, 3, 2, 1, 1);

        label_SaveDir_predict = new QLabel(widget_Predict);
        label_SaveDir_predict->setObjectName(QString::fromUtf8("label_SaveDir_predict"));
        sizePolicy.setHeightForWidth(label_SaveDir_predict->sizePolicy().hasHeightForWidth());
        label_SaveDir_predict->setSizePolicy(sizePolicy);
        label_SaveDir_predict->setMaximumSize(QSize(121, 40));

        gridLayout->addWidget(label_SaveDir_predict, 4, 0, 1, 1);

        label_Stride = new QLabel(widget_Predict);
        label_Stride->setObjectName(QString::fromUtf8("label_Stride"));
        sizePolicy.setHeightForWidth(label_Stride->sizePolicy().hasHeightForWidth());
        label_Stride->setSizePolicy(sizePolicy);
        label_Stride->setMaximumSize(QSize(121, 28));

        gridLayout->addWidget(label_Stride, 7, 0, 1, 1);

        lineEdit_SaveDir_predict = new QLineEdit(widget_Predict);
        lineEdit_SaveDir_predict->setObjectName(QString::fromUtf8("lineEdit_SaveDir_predict"));

        gridLayout->addWidget(lineEdit_SaveDir_predict, 4, 1, 1, 1);

        checkBox_IsSlide = new QCheckBox(widget_Predict);
        checkBox_IsSlide->setObjectName(QString::fromUtf8("checkBox_IsSlide"));
        checkBox_IsSlide->setChecked(true);

        gridLayout->addWidget(checkBox_IsSlide, 5, 1, 1, 2);

        label_Model = new QLabel(widget_Predict);
        label_Model->setObjectName(QString::fromUtf8("label_Model"));
        sizePolicy.setHeightForWidth(label_Model->sizePolicy().hasHeightForWidth());
        label_Model->setSizePolicy(sizePolicy);
        label_Model->setMaximumSize(QSize(121, 40));

        gridLayout->addWidget(label_Model, 0, 0, 1, 1);

        pushButton_SaveDir_predict = new QPushButton(widget_Predict);
        pushButton_SaveDir_predict->setObjectName(QString::fromUtf8("pushButton_SaveDir_predict"));
        sizePolicy5.setHeightForWidth(pushButton_SaveDir_predict->sizePolicy().hasHeightForWidth());
        pushButton_SaveDir_predict->setSizePolicy(sizePolicy5);
        pushButton_SaveDir_predict->setMaximumSize(QSize(70, 16777215));

        gridLayout->addWidget(pushButton_SaveDir_predict, 4, 2, 1, 1);

        spinBox_CropSize_predict = new QSpinBox(widget_Predict);
        spinBox_CropSize_predict->setObjectName(QString::fromUtf8("spinBox_CropSize_predict"));
        sizePolicy4.setHeightForWidth(spinBox_CropSize_predict->sizePolicy().hasHeightForWidth());
        spinBox_CropSize_predict->setSizePolicy(sizePolicy4);
        spinBox_CropSize_predict->setMaximumSize(QSize(383, 28));
        spinBox_CropSize_predict->setMaximum(999999999);
        spinBox_CropSize_predict->setValue(1024);

        gridLayout->addWidget(spinBox_CropSize_predict, 6, 1, 1, 2);

        label_GPU_id = new QLabel(widget_Predict);
        label_GPU_id->setObjectName(QString::fromUtf8("label_GPU_id"));

        gridLayout->addWidget(label_GPU_id, 2, 0, 1, 1);

        spinBox_GPU_id = new QSpinBox(widget_Predict);
        spinBox_GPU_id->setObjectName(QString::fromUtf8("spinBox_GPU_id"));

        gridLayout->addWidget(spinBox_GPU_id, 2, 1, 1, 2);


        verticalLayout_tab_Segmentation->addWidget(widget_Predict);

        groupBox_Warp = new QGroupBox(tab_Predict);
        groupBox_Warp->setObjectName(QString::fromUtf8("groupBox_Warp"));
        verticalLayout_4 = new QVBoxLayout(groupBox_Warp);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_SaveDir_warp = new QHBoxLayout();
        horizontalLayout_SaveDir_warp->setObjectName(QString::fromUtf8("horizontalLayout_SaveDir_warp"));
        label_SaveDir_warp = new QLabel(groupBox_Warp);
        label_SaveDir_warp->setObjectName(QString::fromUtf8("label_SaveDir_warp"));

        horizontalLayout_SaveDir_warp->addWidget(label_SaveDir_warp);

        lineEdit_SaveDir_warp = new QLineEdit(groupBox_Warp);
        lineEdit_SaveDir_warp->setObjectName(QString::fromUtf8("lineEdit_SaveDir_warp"));

        horizontalLayout_SaveDir_warp->addWidget(lineEdit_SaveDir_warp);

        pushButton_SaveDir_warp = new QPushButton(groupBox_Warp);
        pushButton_SaveDir_warp->setObjectName(QString::fromUtf8("pushButton_SaveDir_warp"));
        sizePolicy2.setHeightForWidth(pushButton_SaveDir_warp->sizePolicy().hasHeightForWidth());
        pushButton_SaveDir_warp->setSizePolicy(sizePolicy2);

        horizontalLayout_SaveDir_warp->addWidget(pushButton_SaveDir_warp);


        verticalLayout_4->addLayout(horizontalLayout_SaveDir_warp);

        horizontalLayout_TargetWidth = new QHBoxLayout();
        horizontalLayout_TargetWidth->setObjectName(QString::fromUtf8("horizontalLayout_TargetWidth"));
        label_TargetWidth = new QLabel(groupBox_Warp);
        label_TargetWidth->setObjectName(QString::fromUtf8("label_TargetWidth"));

        horizontalLayout_TargetWidth->addWidget(label_TargetWidth);

        spinBox_TargetWidth = new QSpinBox(groupBox_Warp);
        spinBox_TargetWidth->setObjectName(QString::fromUtf8("spinBox_TargetWidth"));
        spinBox_TargetWidth->setMaximum(999999999);

        horizontalLayout_TargetWidth->addWidget(spinBox_TargetWidth);

        horizontalSpacer_TargetWidth = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_TargetWidth->addItem(horizontalSpacer_TargetWidth);


        verticalLayout_4->addLayout(horizontalLayout_TargetWidth);

        horizontalLayout_TargetHeight = new QHBoxLayout();
        horizontalLayout_TargetHeight->setObjectName(QString::fromUtf8("horizontalLayout_TargetHeight"));
        label_TargetHeight = new QLabel(groupBox_Warp);
        label_TargetHeight->setObjectName(QString::fromUtf8("label_TargetHeight"));

        horizontalLayout_TargetHeight->addWidget(label_TargetHeight);

        spinBox_TargetHeight = new QSpinBox(groupBox_Warp);
        spinBox_TargetHeight->setObjectName(QString::fromUtf8("spinBox_TargetHeight"));
        spinBox_TargetHeight->setMaximum(999999999);

        horizontalLayout_TargetHeight->addWidget(spinBox_TargetHeight);

        horizontalSpacer_TargetHeight = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_TargetHeight->addItem(horizontalSpacer_TargetHeight);


        verticalLayout_4->addLayout(horizontalLayout_TargetHeight);

        horizontalLayout_btnWarp = new QHBoxLayout();
        horizontalLayout_btnWarp->setObjectName(QString::fromUtf8("horizontalLayout_btnWarp"));
        horizontalSpacer_Warp = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_btnWarp->addItem(horizontalSpacer_Warp);

        pushButton_ManualWarp = new QPushButton(groupBox_Warp);
        pushButton_ManualWarp->setObjectName(QString::fromUtf8("pushButton_ManualWarp"));

        horizontalLayout_btnWarp->addWidget(pushButton_ManualWarp);

        pushButton_Warp = new QPushButton(groupBox_Warp);
        pushButton_Warp->setObjectName(QString::fromUtf8("pushButton_Warp"));

        horizontalLayout_btnWarp->addWidget(pushButton_Warp);

        pushButton_WarpAll = new QPushButton(groupBox_Warp);
        pushButton_WarpAll->setObjectName(QString::fromUtf8("pushButton_WarpAll"));

        horizontalLayout_btnWarp->addWidget(pushButton_WarpAll);

        pushButton_StopWarp = new QPushButton(groupBox_Warp);
        pushButton_StopWarp->setObjectName(QString::fromUtf8("pushButton_StopWarp"));

        horizontalLayout_btnWarp->addWidget(pushButton_StopWarp);


        verticalLayout_4->addLayout(horizontalLayout_btnWarp);


        verticalLayout_tab_Segmentation->addWidget(groupBox_Warp);

        verticalSpacer_Predict = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_tab_Segmentation->addItem(verticalSpacer_Predict);

        horizontalLayout_Predict = new QHBoxLayout();
        horizontalLayout_Predict->setObjectName(QString::fromUtf8("horizontalLayout_Predict"));
        horizontalSpacer_Predict = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_Predict->addItem(horizontalSpacer_Predict);

        pushButton_PredictOne = new QPushButton(tab_Predict);
        pushButton_PredictOne->setObjectName(QString::fromUtf8("pushButton_PredictOne"));

        horizontalLayout_Predict->addWidget(pushButton_PredictOne);

        pushButton_PredictAll = new QPushButton(tab_Predict);
        pushButton_PredictAll->setObjectName(QString::fromUtf8("pushButton_PredictAll"));

        horizontalLayout_Predict->addWidget(pushButton_PredictAll);

        pushButton_StopPredict = new QPushButton(tab_Predict);
        pushButton_StopPredict->setObjectName(QString::fromUtf8("pushButton_StopPredict"));

        horizontalLayout_Predict->addWidget(pushButton_StopPredict);


        verticalLayout_tab_Segmentation->addLayout(horizontalLayout_Predict);

        tabWidget_Processing->addTab(tab_Predict, QString());
        tab_PostProcessing = new QWidget();
        tab_PostProcessing->setObjectName(QString::fromUtf8("tab_PostProcessing"));
        verticalLayout = new QVBoxLayout(tab_PostProcessing);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(3, 3, 3, 3);
        groupBox_ThresholdSegmentation = new QGroupBox(tab_PostProcessing);
        groupBox_ThresholdSegmentation->setObjectName(QString::fromUtf8("groupBox_ThresholdSegmentation"));
        formLayout = new QFormLayout(groupBox_ThresholdSegmentation);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_ColorPlane = new QLabel(groupBox_ThresholdSegmentation);
        label_ColorPlane->setObjectName(QString::fromUtf8("label_ColorPlane"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_ColorPlane);

        comboBox_ColorPlane = new QComboBox(groupBox_ThresholdSegmentation);
        comboBox_ColorPlane->addItem(QString());
        comboBox_ColorPlane->addItem(QString());
        comboBox_ColorPlane->addItem(QString());
        comboBox_ColorPlane->addItem(QString());
        comboBox_ColorPlane->addItem(QString());
        comboBox_ColorPlane->addItem(QString());
        comboBox_ColorPlane->addItem(QString());
        comboBox_ColorPlane->setObjectName(QString::fromUtf8("comboBox_ColorPlane"));

        formLayout->setWidget(1, QFormLayout::FieldRole, comboBox_ColorPlane);

        label_Method = new QLabel(groupBox_ThresholdSegmentation);
        label_Method->setObjectName(QString::fromUtf8("label_Method"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_Method);

        comboBox_Method = new QComboBox(groupBox_ThresholdSegmentation);
        comboBox_Method->addItem(QString());
        comboBox_Method->addItem(QString());
        comboBox_Method->setObjectName(QString::fromUtf8("comboBox_Method"));

        formLayout->setWidget(2, QFormLayout::FieldRole, comboBox_Method);

        label_Threshold = new QLabel(groupBox_ThresholdSegmentation);
        label_Threshold->setObjectName(QString::fromUtf8("label_Threshold"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_Threshold);

        doubleSpinBox_Threshold = new QDoubleSpinBox(groupBox_ThresholdSegmentation);
        doubleSpinBox_Threshold->setObjectName(QString::fromUtf8("doubleSpinBox_Threshold"));
        doubleSpinBox_Threshold->setMaximum(255.000000000000000);

        formLayout->setWidget(3, QFormLayout::FieldRole, doubleSpinBox_Threshold);

        label = new QLabel(groupBox_ThresholdSegmentation);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::SpanningRole, label);


        verticalLayout->addWidget(groupBox_ThresholdSegmentation);

        groupBox_ImageProcessing = new QGroupBox(tab_PostProcessing);
        groupBox_ImageProcessing->setObjectName(QString::fromUtf8("groupBox_ImageProcessing"));
        verticalLayout_5 = new QVBoxLayout(groupBox_ImageProcessing);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        treeWidget_ProcessingConfig = new QTreeWidget(groupBox_ImageProcessing);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem(treeWidget_ProcessingConfig);
        __qtreewidgetitem->setCheckState(1, Qt::Unchecked);
        new QTreeWidgetItem(__qtreewidgetitem);
        new QTreeWidgetItem(__qtreewidgetitem);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem(treeWidget_ProcessingConfig);
        __qtreewidgetitem1->setCheckState(1, Qt::Unchecked);
        new QTreeWidgetItem(__qtreewidgetitem1);
        new QTreeWidgetItem(__qtreewidgetitem1);
        new QTreeWidgetItem(__qtreewidgetitem1);
        new QTreeWidgetItem(__qtreewidgetitem1);
        QTreeWidgetItem *__qtreewidgetitem2 = new QTreeWidgetItem(treeWidget_ProcessingConfig);
        new QTreeWidgetItem(__qtreewidgetitem2);
        new QTreeWidgetItem(__qtreewidgetitem2);
        treeWidget_ProcessingConfig->setObjectName(QString::fromUtf8("treeWidget_ProcessingConfig"));
        QSizePolicy sizePolicy6(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(treeWidget_ProcessingConfig->sizePolicy().hasHeightForWidth());
        treeWidget_ProcessingConfig->setSizePolicy(sizePolicy6);

        verticalLayout_5->addWidget(treeWidget_ProcessingConfig);

        horizontalLayout_SaveDir_postprocess = new QHBoxLayout();
        horizontalLayout_SaveDir_postprocess->setObjectName(QString::fromUtf8("horizontalLayout_SaveDir_postprocess"));
        label_SaveDir_postporcess = new QLabel(groupBox_ImageProcessing);
        label_SaveDir_postporcess->setObjectName(QString::fromUtf8("label_SaveDir_postporcess"));

        horizontalLayout_SaveDir_postprocess->addWidget(label_SaveDir_postporcess);

        lineEdit_SaveDir_postporcess = new QLineEdit(groupBox_ImageProcessing);
        lineEdit_SaveDir_postporcess->setObjectName(QString::fromUtf8("lineEdit_SaveDir_postporcess"));

        horizontalLayout_SaveDir_postprocess->addWidget(lineEdit_SaveDir_postporcess);

        pushButton_SaveDir_postporcess = new QPushButton(groupBox_ImageProcessing);
        pushButton_SaveDir_postporcess->setObjectName(QString::fromUtf8("pushButton_SaveDir_postporcess"));
        sizePolicy2.setHeightForWidth(pushButton_SaveDir_postporcess->sizePolicy().hasHeightForWidth());
        pushButton_SaveDir_postporcess->setSizePolicy(sizePolicy2);

        horizontalLayout_SaveDir_postprocess->addWidget(pushButton_SaveDir_postporcess);


        verticalLayout_5->addLayout(horizontalLayout_SaveDir_postprocess);

        verticalLayout_5->setStretch(0, 1);

        verticalLayout->addWidget(groupBox_ImageProcessing);

        horizontalLayout_PostProcess = new QHBoxLayout();
        horizontalLayout_PostProcess->setObjectName(QString::fromUtf8("horizontalLayout_PostProcess"));
        horizontalSpacer_PostProcessing = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_PostProcess->addItem(horizontalSpacer_PostProcessing);

        pushButton_SaveThisImage = new QPushButton(tab_PostProcessing);
        pushButton_SaveThisImage->setObjectName(QString::fromUtf8("pushButton_SaveThisImage"));

        horizontalLayout_PostProcess->addWidget(pushButton_SaveThisImage);

        pushButton_ProcessingAllImage = new QPushButton(tab_PostProcessing);
        pushButton_ProcessingAllImage->setObjectName(QString::fromUtf8("pushButton_ProcessingAllImage"));

        horizontalLayout_PostProcess->addWidget(pushButton_ProcessingAllImage);

        pushButton_StopProcess = new QPushButton(tab_PostProcessing);
        pushButton_StopProcess->setObjectName(QString::fromUtf8("pushButton_StopProcess"));

        horizontalLayout_PostProcess->addWidget(pushButton_StopProcess);


        verticalLayout->addLayout(horizontalLayout_PostProcess);

        verticalLayout->setStretch(1, 1);
        tabWidget_Processing->addTab(tab_PostProcessing, QString());
        tab_Analysis = new QWidget();
        tab_Analysis->setObjectName(QString::fromUtf8("tab_Analysis"));
        verticalLayout_2 = new QVBoxLayout(tab_Analysis);
        verticalLayout_2->setSpacing(9);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(3, 3, 3, 3);
        gridLayout_LayerSize = new QGridLayout();
        gridLayout_LayerSize->setObjectName(QString::fromUtf8("gridLayout_LayerSize"));
        spinBox_LayerWidth = new QSpinBox(tab_Analysis);
        spinBox_LayerWidth->setObjectName(QString::fromUtf8("spinBox_LayerWidth"));
        spinBox_LayerWidth->setMaximum(999999999);

        gridLayout_LayerSize->addWidget(spinBox_LayerWidth, 1, 1, 1, 1);

        label_LayerWidth = new QLabel(tab_Analysis);
        label_LayerWidth->setObjectName(QString::fromUtf8("label_LayerWidth"));

        gridLayout_LayerSize->addWidget(label_LayerWidth, 1, 0, 1, 1);

        spinBox_LayerHeight = new QSpinBox(tab_Analysis);
        spinBox_LayerHeight->setObjectName(QString::fromUtf8("spinBox_LayerHeight"));
        spinBox_LayerHeight->setMaximum(999999999);

        gridLayout_LayerSize->addWidget(spinBox_LayerHeight, 0, 1, 1, 1);

        label_LayerHeight = new QLabel(tab_Analysis);
        label_LayerHeight->setObjectName(QString::fromUtf8("label_LayerHeight"));

        gridLayout_LayerSize->addWidget(label_LayerHeight, 0, 0, 1, 1);

        doubleSpinBox_LayerHeight = new QDoubleSpinBox(tab_Analysis);
        doubleSpinBox_LayerHeight->setObjectName(QString::fromUtf8("doubleSpinBox_LayerHeight"));
        doubleSpinBox_LayerHeight->setDecimals(4);
        doubleSpinBox_LayerHeight->setMaximum(1.000000000000000);
        doubleSpinBox_LayerHeight->setSingleStep(0.100000000000000);

        gridLayout_LayerSize->addWidget(doubleSpinBox_LayerHeight, 0, 2, 1, 1);

        doubleSpinBox_LayerWidth = new QDoubleSpinBox(tab_Analysis);
        doubleSpinBox_LayerWidth->setObjectName(QString::fromUtf8("doubleSpinBox_LayerWidth"));
        doubleSpinBox_LayerWidth->setDecimals(4);
        doubleSpinBox_LayerWidth->setMaximum(1.000000000000000);
        doubleSpinBox_LayerWidth->setSingleStep(0.100000000000000);

        gridLayout_LayerSize->addWidget(doubleSpinBox_LayerWidth, 1, 2, 1, 1);


        verticalLayout_2->addLayout(gridLayout_LayerSize);

        treeWidget_Traits = new QTreeWidget(tab_Analysis);
        new QTreeWidgetItem(treeWidget_Traits);
        new QTreeWidgetItem(treeWidget_Traits);
        new QTreeWidgetItem(treeWidget_Traits);
        new QTreeWidgetItem(treeWidget_Traits);
        new QTreeWidgetItem(treeWidget_Traits);
        new QTreeWidgetItem(treeWidget_Traits);
        new QTreeWidgetItem(treeWidget_Traits);
        new QTreeWidgetItem(treeWidget_Traits);
        new QTreeWidgetItem(treeWidget_Traits);
        new QTreeWidgetItem(treeWidget_Traits);
        new QTreeWidgetItem(treeWidget_Traits);
        new QTreeWidgetItem(treeWidget_Traits);
        new QTreeWidgetItem(treeWidget_Traits);
        new QTreeWidgetItem(treeWidget_Traits);
        new QTreeWidgetItem(treeWidget_Traits);
        new QTreeWidgetItem(treeWidget_Traits);
        new QTreeWidgetItem(treeWidget_Traits);
        new QTreeWidgetItem(treeWidget_Traits);
        QTreeWidgetItem *__qtreewidgetitem3 = new QTreeWidgetItem(treeWidget_Traits);
        new QTreeWidgetItem(__qtreewidgetitem3);
        new QTreeWidgetItem(__qtreewidgetitem3);
        new QTreeWidgetItem(__qtreewidgetitem3);
        new QTreeWidgetItem(__qtreewidgetitem3);
        new QTreeWidgetItem(__qtreewidgetitem3);
        new QTreeWidgetItem(__qtreewidgetitem3);
        treeWidget_Traits->setObjectName(QString::fromUtf8("treeWidget_Traits"));
        QSizePolicy sizePolicy7(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(treeWidget_Traits->sizePolicy().hasHeightForWidth());
        treeWidget_Traits->setSizePolicy(sizePolicy7);
        treeWidget_Traits->setSelectionMode(QAbstractItemView::ExtendedSelection);

        verticalLayout_2->addWidget(treeWidget_Traits);

        horizontalLayout_SacveDir_calculate = new QHBoxLayout();
        horizontalLayout_SacveDir_calculate->setSpacing(9);
        horizontalLayout_SacveDir_calculate->setObjectName(QString::fromUtf8("horizontalLayout_SacveDir_calculate"));
        label_SaveDir_calculate = new QLabel(tab_Analysis);
        label_SaveDir_calculate->setObjectName(QString::fromUtf8("label_SaveDir_calculate"));

        horizontalLayout_SacveDir_calculate->addWidget(label_SaveDir_calculate);

        lineEdit_SaveDir_calculate = new QLineEdit(tab_Analysis);
        lineEdit_SaveDir_calculate->setObjectName(QString::fromUtf8("lineEdit_SaveDir_calculate"));

        horizontalLayout_SacveDir_calculate->addWidget(lineEdit_SaveDir_calculate);

        pushButton_SaveDir_cacuate = new QPushButton(tab_Analysis);
        pushButton_SaveDir_cacuate->setObjectName(QString::fromUtf8("pushButton_SaveDir_cacuate"));
        sizePolicy2.setHeightForWidth(pushButton_SaveDir_cacuate->sizePolicy().hasHeightForWidth());
        pushButton_SaveDir_cacuate->setSizePolicy(sizePolicy2);

        horizontalLayout_SacveDir_calculate->addWidget(pushButton_SaveDir_cacuate);


        verticalLayout_2->addLayout(horizontalLayout_SacveDir_calculate);

        horizontalLayout_Analysis = new QHBoxLayout();
        horizontalLayout_Analysis->setSpacing(9);
        horizontalLayout_Analysis->setObjectName(QString::fromUtf8("horizontalLayout_Analysis"));
        horizontalSpacer_Analysis = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_Analysis->addItem(horizontalSpacer_Analysis);

        pushButton_Calculate = new QPushButton(tab_Analysis);
        pushButton_Calculate->setObjectName(QString::fromUtf8("pushButton_Calculate"));

        horizontalLayout_Analysis->addWidget(pushButton_Calculate);

        pushButton_CalculateAll = new QPushButton(tab_Analysis);
        pushButton_CalculateAll->setObjectName(QString::fromUtf8("pushButton_CalculateAll"));

        horizontalLayout_Analysis->addWidget(pushButton_CalculateAll);

        pushButton_StopCalculate = new QPushButton(tab_Analysis);
        pushButton_StopCalculate->setObjectName(QString::fromUtf8("pushButton_StopCalculate"));

        horizontalLayout_Analysis->addWidget(pushButton_StopCalculate);


        verticalLayout_2->addLayout(horizontalLayout_Analysis);

        verticalLayout_2->setStretch(1, 1);
        tabWidget_Processing->addTab(tab_Analysis, QString());
        tab_Train = new QWidget();
        tab_Train->setObjectName(QString::fromUtf8("tab_Train"));
        _2 = new QVBoxLayout(tab_Train);
        _2->setObjectName(QString::fromUtf8("_2"));
        _2->setContentsMargins(3, 3, 3, 3);
        treeWidget_Train = new QTreeWidget(tab_Train);
        treeWidget_Train->setObjectName(QString::fromUtf8("treeWidget_Train"));
        treeWidget_Train->setIndentation(0);

        // 添加训练相关控件
        QTreeWidgetItem *treewidgetitem_model = new QTreeWidgetItem(treeWidget_Train);
        treewidgetitem_model->setText(0, QString::fromUtf8("Model"));
        comboBox_TrainableModel = new QComboBox();
        treeWidget_Train.setItemWidget(treewidgetitem_model, 1, comboBox_TrainableModel);

        QTreeWidgetItem *treewidgetitem_iters = new QTreeWidgetItem(treeWidget_Train);
        treewidgetitem_iters->setText(0, QString::fromUtf8("Epochs"));
        spinBox_Iters = new QSpinBox();
        treeWidget_Train.setItemWidget(treewidgetitem_iters, 1, spinBox_Iters);

        QTreeWidgetItem *treewidgetitem_batchsize = new QTreeWidgetItem(treeWidget_Train);
        treewidgetitem_batchsize->setText(0, QString::fromUtf8("Batch Size"));
        spinBox_BatchSize = new QSpinBox();
        treeWidget_Train.setItemWidget(treewidgetitem_batchsize, 1, spinBox_BatchSize);

        QTreeWidgetItem *treewidgetitem_lr = new QTreeWidgetItem(treeWidget_Train);
        treewidgetitem_lr->setText(0, QString::fromUtf8("Learning Rate"));
        doubleSpinBox_Lr = new QDoubleSpinBox();
        treeWidget_Train.setItemWidget(treewidgetitem_lr, 1, doubleSpinBox_Lr);

        QTreeWidgetItem *treeWidgetItem_datsetpath = new QTreeWidgetItem(treeWidget_Train);
        treeWidgetItem_datsetpath->setText(0, QString::fromUtf8("Dataset Path"));
        QHBoxLayout *horizontalLayout_DatasetPath = new QHBoxLayout();
        lineEdit_DatasetPath = new QLineEdit();
        pushButton_DatasetPath = new QPushButton(QString::fromUtf8(" . . . "));
        horizontalLayout_DatasetPath->addWidget(lineEdit_DatasetPath);
        horizontalLayout_DatasetPath->addWidget(pushButton_DatasetPath);
        QWidget *widget_DatsetPath = new QWidget();
        widget_DatsetPath->setLayout(horizontalLayout_DatasetPath);
        treeWidget_Train.setItemWidget(treeWidgetItem_datsetpath, 1, widget_DatsetPath);

        QTreeWidgetItem *treeWidgetItem_SaveDir = new QTreeWidgetItem(treeWidget_Train);
        treeWidgetItem_SaveDir->setText(0, QString::fromUtf8("Save Dir"));
        QHBoxLayout *horizontalLayout_SaveDir = new QHBoxLayout();
        lineEdit_SaveDir_train = new QLineEdit();
        pushButton_SaveDir_train = new QPushButton(QString::fromUtf8(" . . . "));
        horizontalLayout_SaveDir->addWidget(lineEdit_SaveDir_train);
        horizontalLayout_SaveDir->addWidget(pushButton_SaveDir_train);
        QWidget *widget_SaveDir = new QWidget();
        widget_SaveDir->setLayout(horizontalLayout_SaveDir);
        treeWidget_Train.setItemWidget(treeWidgetItem_SaveDir, 1, widget_SaveDir);

        QTreeWidgetItem *treeWidgetItem_DatasetSplit = new QTreeWidgetItem(treeWidget_Train);
        treeWidgetItem_DatasetSplit->setText(0, QString::fromUtf8("Dataset Split"));
        QHBoxLayout *horizontalLayout_DatasetSplit = new QHBoxLayout();
        doubleSpinBox_DatasetSplit_train = new QDoubleSpinBox();
        doubleSpinBox_DatasetSplit_val = new QDoubleSpinBox();
        horizontalLayout_DatasetSplit->addWidget(doubleSpinBox_DatasetSplit_train);
        horizontalLayout_DatasetSplit->addWidget(doubleSpinBox_DatasetSplit_val);
        QWidget *widget_DatasetSplit = new QWidget();
        widget_DatasetSplit->setLayout(horizontalLayout_DatasetSplit);
        treeWidget_Train.setItemWidget(treeWidgetItem_DatasetSplit, 1, widget_DatasetSplit);

        QTreeWidgetItem *treeWidgetItem_ResumeModel = new QTreeWidgetItem(treeWidget_Train);
        treeWidgetItem_ResumeModel->setText(0, QString::fromUtf8("Resume Model"));
        QHBoxLayout *horizontalLayout_ResumeModel = new QHBoxLayout();
        lineEdit_ResumeModel = new QLineEdit();
        pushButton_ResumeModel = new QPushButton(QString::fromUtf8(" . . . "));
        horizontalLayout_ResumeModel->addWidget(lineEdit_ResumeModel);
        horizontalLayout_ResumeModel->addWidget(pushButton_ResumeModel);
        QWidget *widget_ResumeModel = new QWidget();
        widget_ResumeModel->setLayout(horizontalLayout_ResumeModel);
        treeWidget_Train.setItemWidget(treeWidgetItem_ResumeModel, 1, widget_ResumeModel);

        QTreeWidgetItem *treeWidgetItem_LogIters = new QTreeWidgetItem(treeWidget_Train);
        treeWidgetItem_LogIters->setText(0, QString::fromUtf8("Log Iters"));
        spinBox_LogIters = new QSpinBox();
        treeWidget_Train.setItemWidget(treeWidgetItem_LogIters, 1, spinBox_LogIters);

        QTreeWidgetItem *treeWidgetItem_SaveInterval = new QTreeWidgetItem(treeWidget_Train);
        treeWidgetItem_SaveInterval->setText(0, QString::fromUtf8("Save Interval"));
        spinBox_SaveInterval = new QSpinBox();
        treeWidget_Train.setItemWidget(treeWidgetItem_SaveInterval, 1, spinBox_SaveInterval);

        QTreeWidgetItem *treeWidgetItem_KeepCheckpointMax = new QTreeWidgetItem(treeWidget_Train);
        treeWidgetItem_KeepCheckpointMax->setText(0, QString::fromUtf8("Keep Checkpoint Max"));
        spinBox_KeepCheckpointMax = new QSpinBox();
        treeWidget_Train.setItemWidget(treeWidgetItem_KeepCheckpointMax, 1, spinBox_KeepCheckpointMax);

        QTreeWidgetItem *treeWidgetItem_CropSize = new QTreeWidgetItem(treeWidget_Train);
        treeWidgetItem_CropSize->setText(0, QString::fromUtf8("Crop Size"));
        spinBox_CropSize_train = new QSpinBox();
        treeWidget_Train.setItemWidget(treeWidgetItem_CropSize, 1, spinBox_CropSize_train);

        _2->addWidget(treeWidget_Train);

        verticalSpacer_Train = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        _2->addItem(verticalSpacer_Train);

        horizontalLayout_Train = new QHBoxLayout();
        horizontalLayout_Train->setObjectName(QString::fromUtf8("horizontalLayout_Train"));
        horizontalSpacer_Train = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_Train->addItem(horizontalSpacer_Train);

        pushButton_StartTrain = new QPushButton(tab_Train);
        pushButton_StartTrain->setObjectName(QString::fromUtf8("pushButton_StartTrain"));

        horizontalLayout_Train->addWidget(pushButton_StartTrain);

        pushButton_StopTrain = new QPushButton(tab_Train);
        pushButton_StopTrain->setObjectName(QString::fromUtf8("pushButton_StopTrain"));

        horizontalLayout_Train->addWidget(pushButton_StopTrain);


        _2->addLayout(horizontalLayout_Train);

        _2->setStretch(0, 1);
        tabWidget_Processing->addTab(tab_Train, QString());

        verticalLayout_7->addWidget(tabWidget_Processing);

        splitter_Left->addWidget(frame_Processing);
        splitter_Middle->addWidget(splitter_Left);
        splitter_Right = new QSplitter(splitter_Middle);
        splitter_Right->setObjectName(QString::fromUtf8("splitter_Right"));
        splitter_Right->setOrientation(Qt::Vertical);
        frame_imageview = new QFrame(splitter_Right);
        frame_imageview->setObjectName(QString::fromUtf8("frame_imageview"));
        frame_imageview->setFrameShape(QFrame::StyledPanel);
        frame_imageview->setFrameShadow(QFrame::Raised);
        verticalLayout_6 = new QVBoxLayout(frame_imageview);
        verticalLayout_6->setSpacing(0);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 6);
        horizontalLayout_imageview = new QHBoxLayout();
        horizontalLayout_imageview->setObjectName(QString::fromUtf8("horizontalLayout_imageview"));
        graphicsView_Main = new DrawGraphicsView(frame_imageview);
        graphicsView_Main->setObjectName(QString::fromUtf8("graphicsView_Main"));

        horizontalLayout_imageview->addWidget(graphicsView_Main);

        graphicsView_Main2 = new DrawGraphicsView(frame_imageview);
        graphicsView_Main2->setObjectName(QString::fromUtf8("graphicsView_Main2"));

        horizontalLayout_imageview->addWidget(graphicsView_Main2);

        graphicsView_Main3 = new DrawGraphicsView(frame_imageview);
        graphicsView_Main3->setObjectName(QString::fromUtf8("graphicsView_Main3"));

        horizontalLayout_imageview->addWidget(graphicsView_Main3);


        verticalLayout_6->addLayout(horizontalLayout_imageview);

        horizontalLayout_ImageInfo = new QHBoxLayout();
        horizontalLayout_ImageInfo->setObjectName(QString::fromUtf8("horizontalLayout_ImageInfo"));
        label_ImageSize = new QLabel(frame_imageview);
        label_ImageSize->setObjectName(QString::fromUtf8("label_ImageSize"));
        sizePolicy.setHeightForWidth(label_ImageSize->sizePolicy().hasHeightForWidth());
        label_ImageSize->setSizePolicy(sizePolicy);

        horizontalLayout_ImageInfo->addWidget(label_ImageSize);

        label_CurrentImage = new QLabel(frame_imageview);
        label_CurrentImage->setObjectName(QString::fromUtf8("label_CurrentImage"));
        label_CurrentImage->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_ImageInfo->addWidget(label_CurrentImage);

        horizontalLayout_ImageInfo->setStretch(1, 1);

        verticalLayout_6->addLayout(horizontalLayout_ImageInfo);

        horizontalLayout_btns = new QHBoxLayout();
        horizontalLayout_btns->setSpacing(9);
        horizontalLayout_btns->setObjectName(QString::fromUtf8("horizontalLayout_btns"));
        horizontalSpacer_btnsLeft = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_btns->addItem(horizontalSpacer_btnsLeft);

        pushButton_Mask = new QPushButton(frame_imageview);
        pushButton_Mask->setObjectName(QString::fromUtf8("pushButton_Mask"));

        horizontalLayout_btns->addWidget(pushButton_Mask);

        pushButton_Paint = new QPushButton(frame_imageview);
        pushButton_Paint->setObjectName(QString::fromUtf8("pushButton_Paint"));

        horizontalLayout_btns->addWidget(pushButton_Paint);

        pushButton_Erase = new QPushButton(frame_imageview);
        pushButton_Erase->setObjectName(QString::fromUtf8("pushButton_Erase"));

        horizontalLayout_btns->addWidget(pushButton_Erase);

        pushButton_FirstImage = new QPushButton(frame_imageview);
        pushButton_FirstImage->setObjectName(QString::fromUtf8("pushButton_FirstImage"));
        pushButton_FirstImage->setMaximumSize(QSize(50, 50));

        horizontalLayout_btns->addWidget(pushButton_FirstImage);

        pushButton_PreviousImage = new QPushButton(frame_imageview);
        pushButton_PreviousImage->setObjectName(QString::fromUtf8("pushButton_PreviousImage"));
        pushButton_PreviousImage->setMaximumSize(QSize(50, 50));

        horizontalLayout_btns->addWidget(pushButton_PreviousImage);

        pushButton_NextImage = new QPushButton(frame_imageview);
        pushButton_NextImage->setObjectName(QString::fromUtf8("pushButton_NextImage"));
        pushButton_NextImage->setMaximumSize(QSize(50, 50));

        horizontalLayout_btns->addWidget(pushButton_NextImage);

        pushButton_LastImage = new QPushButton(frame_imageview);
        pushButton_LastImage->setObjectName(QString::fromUtf8("pushButton_LastImage"));
        pushButton_LastImage->setMaximumSize(QSize(50, 50));

        horizontalLayout_btns->addWidget(pushButton_LastImage);

        pushButton_FitAllImageView = new QPushButton(frame_imageview);
        pushButton_FitAllImageView->setObjectName(QString::fromUtf8("pushButton_FitAllImageView"));
        pushButton_FitAllImageView->setMaximumSize(QSize(50, 50));

        horizontalLayout_btns->addWidget(pushButton_FitAllImageView);

        horizontalSpacer_btnsRight = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_btns->addItem(horizontalSpacer_btnsRight);


        verticalLayout_6->addLayout(horizontalLayout_btns);

        splitter_Right->addWidget(frame_imageview);
        frame_MessagePrinter = new QFrame(splitter_Right);
        frame_MessagePrinter->setObjectName(QString::fromUtf8("frame_MessagePrinter"));
        frame_MessagePrinter->setFrameShape(QFrame::StyledPanel);
        frame_MessagePrinter->setFrameShadow(QFrame::Raised);
        horizontalLayout_4 = new QHBoxLayout(frame_MessagePrinter);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        textBrowser_MessagePrinter = new QTextBrowser(frame_MessagePrinter);
        textBrowser_MessagePrinter->setObjectName(QString::fromUtf8("textBrowser_MessagePrinter"));

        horizontalLayout_4->addWidget(textBrowser_MessagePrinter);

        splitter_Right->addWidget(frame_MessagePrinter);
        splitter_Middle->addWidget(splitter_Right);

        gridLayout_2->addWidget(splitter_Middle, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        QWidget::setTabOrder(pushButton_FirstImage, pushButton_PreviousImage);
        QWidget::setTabOrder(pushButton_PreviousImage, pushButton_NextImage);
        QWidget::setTabOrder(pushButton_NextImage, pushButton_LastImage);
        QWidget::setTabOrder(pushButton_LastImage, pushButton_FitAllImageView);

        retranslateUi(MainWindow);

        tabWidget_Processing->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Root Phenotype Toolbox ", nullptr));
        actionimport_image->setText(QCoreApplication::translate("MainWindow", "import image", nullptr));
        actionimport_image_set->setText(QCoreApplication::translate("MainWindow", "import image set", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
#if QT_CONFIG(shortcut)
        actionExit->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Q", nullptr));
#endif // QT_CONFIG(shortcut)
        label_DataDir->setText(QCoreApplication::translate("MainWindow", "Data Path:", nullptr));
        label_DataDirShow->setText(QString());
        pushButton_DataDir->setText(QCoreApplication::translate("MainWindow", " . . . ", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget_Files->headerItem();
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("MainWindow", "\346\226\207\344\273\266\345\210\227\350\241\250", nullptr));
        label_IsSlide->setText(QCoreApplication::translate("MainWindow", "is slide:", nullptr));
        label_CropSize->setText(QCoreApplication::translate("MainWindow", "crop size:", nullptr));
        label_WeightPath->setText(QCoreApplication::translate("MainWindow", "weight:", nullptr));
        pushButton_WeightPath->setText(QCoreApplication::translate("MainWindow", " . . . ", nullptr));
        label_SaveDir_predict->setText(QCoreApplication::translate("MainWindow", "save dir:", nullptr));
        label_Stride->setText(QCoreApplication::translate("MainWindow", "stride:", nullptr));
        checkBox_IsSlide->setText(QCoreApplication::translate("MainWindow", "prediction by sliding window", nullptr));
        label_Model->setText(QCoreApplication::translate("MainWindow", "model:", nullptr));
        pushButton_SaveDir_predict->setText(QCoreApplication::translate("MainWindow", " . . . ", nullptr));
        label_GPU_id->setText(QCoreApplication::translate("MainWindow", "GPU_id:", nullptr));
        groupBox_Warp->setTitle(QCoreApplication::translate("MainWindow", "perspective warp", nullptr));
        label_SaveDir_warp->setText(QCoreApplication::translate("MainWindow", "save dir:", nullptr));
        pushButton_SaveDir_warp->setText(QCoreApplication::translate("MainWindow", " . . . ", nullptr));
        label_TargetWidth->setText(QCoreApplication::translate("MainWindow", "target width:", nullptr));
        label_TargetHeight->setText(QCoreApplication::translate("MainWindow", "target height:", nullptr));
        pushButton_ManualWarp->setText(QCoreApplication::translate("MainWindow", "manual", nullptr));
        pushButton_Warp->setText(QCoreApplication::translate("MainWindow", "warp", nullptr));
        pushButton_WarpAll->setText(QCoreApplication::translate("MainWindow", "warp all", nullptr));
        pushButton_StopWarp->setText(QCoreApplication::translate("MainWindow", "stop", nullptr));
        pushButton_PredictOne->setText(QCoreApplication::translate("MainWindow", "predict one", nullptr));
        pushButton_PredictAll->setText(QCoreApplication::translate("MainWindow", "predict all", nullptr));
        pushButton_StopPredict->setText(QCoreApplication::translate("MainWindow", "stop", nullptr));
        tabWidget_Processing->setTabText(tabWidget_Processing->indexOf(tab_Predict), QCoreApplication::translate("MainWindow", "Predict", nullptr));
        groupBox_ThresholdSegmentation->setTitle(QCoreApplication::translate("MainWindow", "Threshold segmentation", nullptr));
        label_ColorPlane->setText(QCoreApplication::translate("MainWindow", "color plane:", nullptr));
        comboBox_ColorPlane->setItemText(0, QCoreApplication::translate("MainWindow", "None", nullptr));
        comboBox_ColorPlane->setItemText(1, QCoreApplication::translate("MainWindow", "Red", nullptr));
        comboBox_ColorPlane->setItemText(2, QCoreApplication::translate("MainWindow", "Green", nullptr));
        comboBox_ColorPlane->setItemText(3, QCoreApplication::translate("MainWindow", "Blue", nullptr));
        comboBox_ColorPlane->setItemText(4, QCoreApplication::translate("MainWindow", "Hue", nullptr));
        comboBox_ColorPlane->setItemText(5, QCoreApplication::translate("MainWindow", "Saturation", nullptr));
        comboBox_ColorPlane->setItemText(6, QCoreApplication::translate("MainWindow", "Value", nullptr));

        label_Method->setText(QCoreApplication::translate("MainWindow", "segmentation method:", nullptr));
        comboBox_Method->setItemText(0, QCoreApplication::translate("MainWindow", "Adaptive threshold(OTSU)", nullptr));
        comboBox_Method->setItemText(1, QCoreApplication::translate("MainWindow", "manual threshold", nullptr));

        label_Threshold->setText(QCoreApplication::translate("MainWindow", "threshold:", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "This will cover the results of predict", nullptr));
        groupBox_ImageProcessing->setTitle(QCoreApplication::translate("MainWindow", "Image processing", nullptr));
        QTreeWidgetItem *___qtreewidgetitem1 = treeWidget_ProcessingConfig->headerItem();
        ___qtreewidgetitem1->setText(1, QCoreApplication::translate("MainWindow", "value", nullptr));
        ___qtreewidgetitem1->setText(0, QCoreApplication::translate("MainWindow", "operation", nullptr));

        const bool __sortingEnabled = treeWidget_ProcessingConfig->isSortingEnabled();
        treeWidget_ProcessingConfig->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem2 = treeWidget_ProcessingConfig->topLevelItem(0);
        ___qtreewidgetitem2->setText(0, QCoreApplication::translate("MainWindow", "remove small areas", nullptr));
        QTreeWidgetItem *___qtreewidgetitem3 = ___qtreewidgetitem2->child(0);
        ___qtreewidgetitem3->setText(0, QCoreApplication::translate("MainWindow", "dilate", nullptr));
        QTreeWidgetItem *___qtreewidgetitem4 = ___qtreewidgetitem2->child(1);
        ___qtreewidgetitem4->setText(0, QCoreApplication::translate("MainWindow", "threshold", nullptr));
        QTreeWidgetItem *___qtreewidgetitem5 = treeWidget_ProcessingConfig->topLevelItem(1);
        ___qtreewidgetitem5->setText(0, QCoreApplication::translate("MainWindow", "remove boundary objects", nullptr));
        QTreeWidgetItem *___qtreewidgetitem6 = ___qtreewidgetitem5->child(0);
        ___qtreewidgetitem6->setText(0, QCoreApplication::translate("MainWindow", "left", nullptr));
        QTreeWidgetItem *___qtreewidgetitem7 = ___qtreewidgetitem5->child(1);
        ___qtreewidgetitem7->setText(0, QCoreApplication::translate("MainWindow", "top", nullptr));
        QTreeWidgetItem *___qtreewidgetitem8 = ___qtreewidgetitem5->child(2);
        ___qtreewidgetitem8->setText(0, QCoreApplication::translate("MainWindow", "right", nullptr));
        QTreeWidgetItem *___qtreewidgetitem9 = ___qtreewidgetitem5->child(3);
        ___qtreewidgetitem9->setText(0, QCoreApplication::translate("MainWindow", "bottom", nullptr));
        QTreeWidgetItem *___qtreewidgetitem10 = treeWidget_ProcessingConfig->topLevelItem(2);
        ___qtreewidgetitem10->setText(0, QCoreApplication::translate("MainWindow", "inpainting", nullptr));
        QTreeWidgetItem *___qtreewidgetitem11 = ___qtreewidgetitem10->child(0);
        ___qtreewidgetitem11->setText(0, QCoreApplication::translate("MainWindow", "manual", nullptr));
        QTreeWidgetItem *___qtreewidgetitem12 = ___qtreewidgetitem10->child(1);
        ___qtreewidgetitem12->setText(0, QCoreApplication::translate("MainWindow", "auto(times)", nullptr));
        treeWidget_ProcessingConfig->setSortingEnabled(__sortingEnabled);

        label_SaveDir_postporcess->setText(QCoreApplication::translate("MainWindow", "save dir:", nullptr));
        pushButton_SaveDir_postporcess->setText(QCoreApplication::translate("MainWindow", " . . . ", nullptr));
        pushButton_SaveThisImage->setText(QCoreApplication::translate("MainWindow", "save", nullptr));
        pushButton_ProcessingAllImage->setText(QCoreApplication::translate("MainWindow", "process all", nullptr));
        pushButton_StopProcess->setText(QCoreApplication::translate("MainWindow", "stop", nullptr));
        tabWidget_Processing->setTabText(tabWidget_Processing->indexOf(tab_PostProcessing), QCoreApplication::translate("MainWindow", "PostProcessing", nullptr));
        label_LayerWidth->setText(QCoreApplication::translate("MainWindow", "Layer_width:", nullptr));
        label_LayerHeight->setText(QCoreApplication::translate("MainWindow", "Layer_height:", nullptr));
        QTreeWidgetItem *___qtreewidgetitem13 = treeWidget_Traits->headerItem();
        ___qtreewidgetitem13->setText(1, QCoreApplication::translate("MainWindow", "value", nullptr));
        ___qtreewidgetitem13->setText(0, QCoreApplication::translate("MainWindow", "trait", nullptr));

        const bool __sortingEnabled1 = treeWidget_Traits->isSortingEnabled();
        treeWidget_Traits->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem14 = treeWidget_Traits->topLevelItem(0);
        ___qtreewidgetitem14->setText(0, QCoreApplication::translate("MainWindow", "area", nullptr));
        QTreeWidgetItem *___qtreewidgetitem15 = treeWidget_Traits->topLevelItem(1);
        ___qtreewidgetitem15->setText(0, QCoreApplication::translate("MainWindow", "convex_area", nullptr));
        QTreeWidgetItem *___qtreewidgetitem16 = treeWidget_Traits->topLevelItem(2);
        ___qtreewidgetitem16->setText(0, QCoreApplication::translate("MainWindow", "length", nullptr));
        QTreeWidgetItem *___qtreewidgetitem17 = treeWidget_Traits->topLevelItem(3);
        ___qtreewidgetitem17->setText(0, QCoreApplication::translate("MainWindow", "diameter", nullptr));
        QTreeWidgetItem *___qtreewidgetitem18 = treeWidget_Traits->topLevelItem(4);
        ___qtreewidgetitem18->setText(0, QCoreApplication::translate("MainWindow", "depth", nullptr));
        QTreeWidgetItem *___qtreewidgetitem19 = treeWidget_Traits->topLevelItem(5);
        ___qtreewidgetitem19->setText(0, QCoreApplication::translate("MainWindow", "width", nullptr));
        QTreeWidgetItem *___qtreewidgetitem20 = treeWidget_Traits->topLevelItem(6);
        ___qtreewidgetitem20->setText(0, QCoreApplication::translate("MainWindow", "wdRatio", nullptr));
        QTreeWidgetItem *___qtreewidgetitem21 = treeWidget_Traits->topLevelItem(7);
        ___qtreewidgetitem21->setText(0, QCoreApplication::translate("MainWindow", "sturdiness", nullptr));
        QTreeWidgetItem *___qtreewidgetitem22 = treeWidget_Traits->topLevelItem(8);
        ___qtreewidgetitem22->setText(0, QCoreApplication::translate("MainWindow", "initial_x", nullptr));
        QTreeWidgetItem *___qtreewidgetitem23 = treeWidget_Traits->topLevelItem(9);
        ___qtreewidgetitem23->setText(0, QCoreApplication::translate("MainWindow", "initial_y", nullptr));
        QTreeWidgetItem *___qtreewidgetitem24 = treeWidget_Traits->topLevelItem(10);
        ___qtreewidgetitem24->setText(0, QCoreApplication::translate("MainWindow", "centroid_x", nullptr));
        QTreeWidgetItem *___qtreewidgetitem25 = treeWidget_Traits->topLevelItem(11);
        ___qtreewidgetitem25->setText(0, QCoreApplication::translate("MainWindow", "centroid_y", nullptr));
        QTreeWidgetItem *___qtreewidgetitem26 = treeWidget_Traits->topLevelItem(12);
        ___qtreewidgetitem26->setText(0, QCoreApplication::translate("MainWindow", "angle_apex_left", nullptr));
        QTreeWidgetItem *___qtreewidgetitem27 = treeWidget_Traits->topLevelItem(13);
        ___qtreewidgetitem27->setText(0, QCoreApplication::translate("MainWindow", "angle_apex_right", nullptr));
        QTreeWidgetItem *___qtreewidgetitem28 = treeWidget_Traits->topLevelItem(14);
        ___qtreewidgetitem28->setText(0, QCoreApplication::translate("MainWindow", "angle_apex_all", nullptr));
        QTreeWidgetItem *___qtreewidgetitem29 = treeWidget_Traits->topLevelItem(15);
        ___qtreewidgetitem29->setText(0, QCoreApplication::translate("MainWindow", "angle_entire_left", nullptr));
        QTreeWidgetItem *___qtreewidgetitem30 = treeWidget_Traits->topLevelItem(16);
        ___qtreewidgetitem30->setText(0, QCoreApplication::translate("MainWindow", "angle_entire_right", nullptr));
        QTreeWidgetItem *___qtreewidgetitem31 = treeWidget_Traits->topLevelItem(17);
        ___qtreewidgetitem31->setText(0, QCoreApplication::translate("MainWindow", "angle_entire_all", nullptr));
        QTreeWidgetItem *___qtreewidgetitem32 = treeWidget_Traits->topLevelItem(18);
        ___qtreewidgetitem32->setText(0, QCoreApplication::translate("MainWindow", "layer_mass", nullptr));
        QTreeWidgetItem *___qtreewidgetitem33 = ___qtreewidgetitem32->child(0);
        ___qtreewidgetitem33->setText(0, QCoreApplication::translate("MainWindow", "Area", nullptr));
        QTreeWidgetItem *___qtreewidgetitem34 = ___qtreewidgetitem32->child(1);
        ___qtreewidgetitem34->setText(0, QCoreApplication::translate("MainWindow", "Length", nullptr));
        QTreeWidgetItem *___qtreewidgetitem35 = ___qtreewidgetitem32->child(2);
        ___qtreewidgetitem35->setText(0, QCoreApplication::translate("MainWindow", "Convex_hull", nullptr));
        QTreeWidgetItem *___qtreewidgetitem36 = ___qtreewidgetitem32->child(3);
        ___qtreewidgetitem36->setText(0, QCoreApplication::translate("MainWindow", "A/C", nullptr));
        QTreeWidgetItem *___qtreewidgetitem37 = ___qtreewidgetitem32->child(4);
        ___qtreewidgetitem37->setText(0, QCoreApplication::translate("MainWindow", "A/L", nullptr));
        QTreeWidgetItem *___qtreewidgetitem38 = ___qtreewidgetitem32->child(5);
        ___qtreewidgetitem38->setText(0, QCoreApplication::translate("MainWindow", "L/C", nullptr));
        treeWidget_Traits->setSortingEnabled(__sortingEnabled1);

        label_SaveDir_calculate->setText(QCoreApplication::translate("MainWindow", "save dir:", nullptr));
        pushButton_SaveDir_cacuate->setText(QCoreApplication::translate("MainWindow", " . . . ", nullptr));
        pushButton_Calculate->setText(QCoreApplication::translate("MainWindow", "calculate", nullptr));
        pushButton_CalculateAll->setText(QCoreApplication::translate("MainWindow", "calculate all", nullptr));
        pushButton_StopCalculate->setText(QCoreApplication::translate("MainWindow", "stop", nullptr));
        tabWidget_Processing->setTabText(tabWidget_Processing->indexOf(tab_Analysis), QCoreApplication::translate("MainWindow", "Analysis", nullptr));
        QTreeWidgetItem *___qtreewidgetitem39 = treeWidget_Train->headerItem();
        ___qtreewidgetitem39->setText(1, QCoreApplication::translate("MainWindow", "value", nullptr));
        ___qtreewidgetitem39->setText(0, QCoreApplication::translate("MainWindow", "Hyperparameter", nullptr));
        pushButton_StartTrain->setText(QCoreApplication::translate("MainWindow", "start train", nullptr));
        pushButton_StopTrain->setText(QCoreApplication::translate("MainWindow", "stop", nullptr));
        tabWidget_Processing->setTabText(tabWidget_Processing->indexOf(tab_Train), QCoreApplication::translate("MainWindow", "Train", nullptr));
        label_ImageSize->setText(QCoreApplication::translate("MainWindow", "image size: 0x0", nullptr));
        label_CurrentImage->setText(QCoreApplication::translate("MainWindow", "currentlabel", nullptr));
        pushButton_Mask->setText(QCoreApplication::translate("MainWindow", " mask ", nullptr));
        pushButton_Paint->setText(QCoreApplication::translate("MainWindow", " paint ", nullptr));
        pushButton_Erase->setText(QCoreApplication::translate("MainWindow", " erase ", nullptr));
        pushButton_FirstImage->setText(QCoreApplication::translate("MainWindow", " \342\217\256 ", nullptr));
        pushButton_PreviousImage->setText(QCoreApplication::translate("MainWindow", " \342\217\252 ", nullptr));
        pushButton_NextImage->setText(QCoreApplication::translate("MainWindow", " \342\217\251 ", nullptr));
        pushButton_LastImage->setText(QCoreApplication::translate("MainWindow", " \342\217\255 ", nullptr));
        pushButton_FitAllImageView->setText(QCoreApplication::translate("MainWindow", " \360\237\224\201 ", nullptr));
        textBrowser_MessagePrinter->setHtml(QCoreApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">&gt;&gt;Root Phenotype Toolbox v0.0.1</p></body></html>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GF_ROOTANALYSIS_H
