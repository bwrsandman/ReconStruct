#-------------------------------------------------
#
# Project created by QtCreator 2013-11-10T11:42:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = reconstruct
TEMPLATE = app
CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    comboboxdelegate.cpp \
    datatypebase.cpp \
    datatypeint.cpp \
    datatypebytes.cpp \
    datatypebool.cpp \
    datatypestr.cpp \
    deconstructview.cpp \
    datatypecustom.cpp

HEADERS  += \
    mainwindow.h \
    comboboxdelegate.h \
    datatypebase.h \
    datatypeint.h \
    datatypebytes.h \
    datatypebool.h \
    datatypestr.h \
    deconstructview.h \
    datatypecustom.h \
    misc.h

FORMS    += \
    mainwindow.ui

OTHER_FILES += \
    README.md \
    LICENSE.md

include(QHexEdit/src/qhexedit_widget.pri)
