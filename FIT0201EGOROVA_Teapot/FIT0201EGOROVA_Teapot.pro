#-------------------------------------------------
#
# Project created by QtCreator 2013-04-02T00:17:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FIT0201EGOROVA_Teapot
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    teapotwidget.cpp \
    teapothandler.cpp \
    graphics.cpp \
    bezierplotter.cpp

HEADERS  += mainwindow.h \
    teapotwidget.h \
    teapothandler.h \
    graphics.h \
    bezierplotter.h

FORMS    += mainwindow.ui \
    teapotwidget.ui

RESOURCES += \
    Resourses.qrc
