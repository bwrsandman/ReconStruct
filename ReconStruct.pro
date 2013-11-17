#-------------------------------------------------
#
# Project created by QtCreator 2013-11-10T11:42:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = restruct
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    comboboxdelegate.cpp \
    sizefielddelegate.cpp \
    sizefield.cpp

HEADERS  += mainwindow.h \
    comboboxdelegate.h \
    sizefielddelegate.h \
    sizefield.h

FORMS    += mainwindow.ui \
    sizefield.ui

include(QHexEdit/src/qhexedit_widget.pri)
