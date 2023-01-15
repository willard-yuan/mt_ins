#-------------------------------------------------
#
# Project created by yongyuan.name
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ins
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

CONFIG += c++11 -Wno-old-style-cast

SOURCES += \
    main.cpp \
    main.cpp \
    fcnn.cpp \
    mainwindow.cpp \
    helper.cpp \
    params.cpp

HEADERS += \
        mainwindow.h \
    helper.h \
    helper.h \
    mainwindow.h \
    fcnn.h \
    params.h

FORMS    += mainwindow.ui \
    params.ui

# OpenCV include
INCLUDEPATH += C:/opencv/opencv4_6_0/opencv/build/include
INCLUDEPATH += C:/opencv/opencv4_6_0/opencv/build/include/opencv2
LIBS+=-LC:\opencv\opencv4_6_0\opencv\build\x64\vc15\lib -lopencv_world460

ICON = icon.icns

RESOURCES += design.qrc

#RESOURCES += qdarkstyle/style.qrc

#RESOURCES += resources.qrc

RC_FILE += appico.rc

DISTFILES += \
    config.ini
