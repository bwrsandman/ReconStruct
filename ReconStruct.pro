QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = reconstruct
TEMPLATE = app
CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

SOURCES += \
    main.cpp \
    datatypebase.cpp \
    datatypestr.cpp \
    datatypecustom.cpp

HEADERS  += \
    datatypebase.h \
    datatypestr.h \
    datatypecustom.h

FORMS    +=

OTHER_FILES += \
    README.md \
    LICENSE.md

test {
    message(Test build)
    include(tests/tests.pri)
} else {
    message(Normal build)
}
