#-------------------------------------------------
#
# Project created by QtCreator 2017-01-25T15:09:39
#
#-------------------------------------------------

QT       += core gui charts network

# included to allow use of libpcap
LIBS += -lpcap
LIBS += -pthread

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FinalYearProject
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    packettracer.cpp \
    packet.cpp \
    packetinfodialog.cpp \
    statwindow.cpp \
    sidebysidewindow.cpp \
    congestionwindow.cpp

HEADERS  += mainwindow.h \
    packettracer.h \
    packet.h \
    packetinfodialog.h \
    statwindow.h \
    sidebysidewindow.h \
    congestionwindow.h

FORMS    += mainwindow.ui \
    packetinfodialog.ui \
    statwindow.ui \
    sidebysidewindow.ui \
    congestionwindow.ui
