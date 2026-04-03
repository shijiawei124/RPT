QT       += core gui widgets charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RPT
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/mygraphicsview.cpp \
    src/imageprocessor.cpp \
    src/modelmanager.cpp \
    src/traitsextractor.cpp

HEADERS += \
    include/mainwindow.h \
    include/mygraphicsview.h \
    include/imageprocessor.h \
    include/modelmanager.h \
    include/traitsextractor.h

FORMS += \
    ui/mainwindow.ui

# OpenCV
INCLUDEPATH += D:/opencv/build/include
LIBS += -LD:/opencv/build/x64/vc15/lib -lopencv_core455 -lopencv_imgproc455 -lopencv_highgui455 -lopencv_imgcodecs455

# PaddlePaddle
INCLUDEPATH += D:/paddle/paddle/include
LIBS += -LD:/paddle/paddle/lib -lpaddle_inference

# ONNX Runtime
INCLUDEPATH += D:/onnxruntime/include
LIBS += -LD:/onnxruntime/lib -lonnxruntime

# Boost
INCLUDEPATH += D:/boost/include
LIBS += -LD:/boost/lib -lboost_system -lboost_filesystem

RESOURCES += \
    resources/resources.qrc
