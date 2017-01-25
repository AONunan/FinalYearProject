#-------------------------------------------------
#
# Project created by QtCreator 2017-01-25T15:09:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FinalYearProject
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    packetcapturewindow.cpp \
    packettracer.cpp

HEADERS  += mainwindow.h \
    packetcapturewindow.h \
    packettracer.h

FORMS    += mainwindow.ui \
    packetcapturewindow.ui
