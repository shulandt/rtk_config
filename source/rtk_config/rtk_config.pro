#-------------------------------------------------
#
# Project created by QtCreator 2015-09-04T09:39:54
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 1.0.0.2
QMAKE_TARGET_COPYRIGHT = NVS

TARGET = rtk_config
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    nmea.cpp

HEADERS  += mainwindow.h \
    nmea.h

FORMS    += mainwindow.ui

win32:RC_ICONS = appIcon.ico
