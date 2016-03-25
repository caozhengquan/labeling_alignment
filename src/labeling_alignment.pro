#-------------------------------------------------
#
# Project created by QtCreator 2016-01-26T14:07:22
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = labeling_alignment
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    labelingscreen.cpp \
    showscreen.cpp \
    common.cpp \
    helperscreen.cpp

HEADERS  += mainwindow.h \
    labelingscreen.h \
    showscreen.h \
    common.h \
    helperscreen.h

FORMS    += mainwindow.ui

OPENCV = D:/opencv/opencv3.0.0/build
CONFIG(debug,debug|release): LIBS += -L$$OPENCV/x64/vc12/lib/ -lopencv_world300d -lopencv_ts300d
CONFIG(release,debug|release): LIBS += -L$$OPENCV/x64/vc12/lib/ -lopencv_world300 -lopencv_ts300
INCLUDEPATH += $$OPENCV/include
