#-------------------------------------------------
#
# Project created by QtCreator 2013-11-10T11:42:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = reconstruct
TEMPLATE = app

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    comboboxdelegate.cpp

HEADERS  += \
    mainwindow.h \
    comboboxdelegate.h

FORMS    += \
    mainwindow.ui

OTHER_FILES += \
    README.md \
    LICENSE.md

include(QHexEdit/src/qhexedit_widget.pri)
