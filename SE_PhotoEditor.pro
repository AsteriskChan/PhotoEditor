#-------------------------------------------------
#
# Project created by QtCreator 2017-12-07T15:40:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SE_PhotoEditer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    imageprocess.cpp \
    geometrydialog.cpp \
    colorspace.cpp \
    contrastdialog.cpp \
    filter.cpp \
    qssloader.cpp

HEADERS += \
        mainwindow.h \
    imageprocess.h \
    imageprocess.h \
    geometrydialog.h \
    colorspace.h \
    contrastdialog.h \
    filter.h \
    qssloader.h

FORMS += \
        mainwindow.ui \
    geometrydialog.ui \
    contrastdialog.ui

INCLUDEPATH += F:\opencv\build\include

LIBS += F:\opencv-build\bin\libopencv_core320.dll
LIBS += F:\opencv-build\bin\libopencv_highgui320.dll
LIBS += F:\opencv-build\bin\libopencv_imgcodecs320.dll
LIBS += F:\opencv-build\bin\libopencv_imgproc320.dll
LIBS += F:\opencv-build\bin\libopencv_features2d320.dll
LIBS += F:\opencv-build\bin\libopencv_calib3d320.dll

RESOURCES += \
    icon.qrc \
    qss.qrc

DISTFILES +=
