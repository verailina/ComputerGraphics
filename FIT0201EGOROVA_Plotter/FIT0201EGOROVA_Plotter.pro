#-------------------------------------------------
#
# Project created by QtCreator 2013-03-08T15:42:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FIT0201EGOROVA_Plotter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    curveplotter.cpp \
    curvewidget.cpp \
    cassiniwidget.cpp \
    cassiniplotter.cpp

HEADERS  += mainwindow.h \
    curveplotter.h \
    curvewidget.h \
    cassiniwidget.h \
    cassiniplotter.h

FORMS    += mainwindow.ui \
    curvewidget.ui \
    cassiniwidget.ui
