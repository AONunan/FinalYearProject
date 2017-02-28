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
    filtersettingswindow.cpp \
    sidebysidewindow.cpp

HEADERS  += mainwindow.h \
    packettracer.h \
    packet.h \
    packetinfodialog.h \
    statwindow.h \
    filtersettingswindow.h \
    sidebysidewindow.h

FORMS    += mainwindow.ui \
    packetinfodialog.ui \
    statwindow.ui \
    filtersettingswindow.ui \
    sidebysidewindow.ui
