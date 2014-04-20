QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = reconstruct
TEMPLATE = app
CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
    main.cpp

HEADERS  +=

FORMS    +=

OTHER_FILES += \
    README.md \
    LICENSE.md
