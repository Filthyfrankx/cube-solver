#-------------------------------------------------
#
# Project created by QtCreator 2021-04-07T16:54:11
#
#-------------------------------------------------

QT       += core gui
QT       += multimedia multimediawidgets
QT       += serialport
QT       += core

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cubesolve
TEMPLATE = app
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
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
    videoplayer.cpp \
    video.cpp \
    solution.cpp \
    mypushbutton.cpp \
    axis.cpp \
    cube.cpp \
    cubeblock.cpp \
    cubevalue.cpp \
    log.cpp

HEADERS += \
        mainwindow.h \
    videoplayer.h \
    video.h \
    solution.h \
    mypushbutton.h \
    axis.h \
    cube.h \
    cubeblock.h \
    cubevalue.h \
    log.h

FORMS += \
    mainwindow.ui \
    videoplayer.ui

RESOURCES += \
    resources.qrc

VERSION = 1.0.0
QMAKE_TARGET_PRODUCT = cubesolve.exe
QMAKE_TARGET_COMPANY = "许创鸿 个人"
QMAKE_TARGET_DESCRIPTION = "上位机"
QMAKE_TARGET_COPYRIGHT = "许创鸿 个人"
