#-------------------------------------------------
#
# Project created by QtCreator 2013-12-02T13:40:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Lab2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    videocontainer.cpp

HEADERS  += mainwindow.h \
    videocontainer.h

FORMS    += mainwindow.ui

unix:!macx: CONFIG += link_pkgconfig
unix:!macx: PKGCONFIG += gstreamer-0.10
unix:!macx: PKGCONFIG += gstreamer-interfaces-0.10

mac: LIBS += -F/Library/Frameworks/ -framework GStreamer
mac: INCLUDEPATH += /Library/Frameworks/GStreamer.framework/Headers
mac: DEPENDPATH += /Library/Frameworks/GStreamer.framework/Headers
