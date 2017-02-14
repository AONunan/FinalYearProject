#-------------------------------------------------
#
# Project created by QtCreator 2017-01-25T15:09:39
#
#-------------------------------------------------

QT       += core gui

# included to allow use of libpcap
LIBS += -lpcap

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FinalYearProject
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    packetcapturewindow.cpp \
    packettracer.cpp \
    packet.cpp \
    tcppacket.cpp

HEADERS  += mainwindow.h \
    packettracer.h \
    packet.h \
    packecapturewindow.h \
    tcppacket.h

FORMS    += mainwindow.ui \
    packetcapturewindow.ui
