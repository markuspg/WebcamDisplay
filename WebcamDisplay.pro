#-------------------------------------------------
#
# Project created by QtCreator 2015-07-29T17:40:31
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WebcamDisplay
TEMPLATE = app


SOURCES += main.cpp\
        webcamdisplay.cpp

HEADERS  += webcamdisplay.h

FORMS    += webcamdisplay.ui \
    authenticationdialog.ui

QMAKE_CXXFLAGS += -std=c++11

OTHER_FILES += \
    LICENSE \
    README.md
