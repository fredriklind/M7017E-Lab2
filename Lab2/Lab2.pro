#-------------------------------------------------
#
# Project created by QtCreator 2013-12-02T13:40:29
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Lab2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        videocontainer.cpp \
    client.cpp \
    server.cpp \
    videoserver.cpp

HEADERS  += mainwindow.h \
         videocontainer.h \
    client.h \
    server.h \
    videoserver.h

FORMS    += mainwindow.ui

unix:!macx: CONFIG += link_pkgconfig
unix:!macx: PKGCONFIG += gstreamer-0.10
unix:!macx: PKGCONFIG += gstreamer-interfaces-0.10
unix:!macx: INCLUDEPATH += /usr/include/gstreamer-0.10

mac: LIBS += -F/Library/Frameworks/ -framework GStreamer
mac: INCLUDEPATH += /Library/Frameworks/GStreamer.framework/Headers
mac: DEPENDPATH += /Library/Frameworks/GStreamer.framework/Headers
