#-------------------------------------------------
#
# Project created by QtCreator 2019-05-07T14:47:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = scenario_editor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14
INCLUDEPATH += /home/ratnavali/workspace/libcarla/install-folder/include/
INCLUDEPATH += /home/ratnavali/workspace/libcarla/install-folder/include/system/
INCLUDEPATH += /home/ratnavali/workspace/libcarla/install-folder/include/traffic_manager/
INCLUDEPATH += /home/ratnavali/workspace/libcarla/install-folder/include/traffic_manager/src/
INCLUDEPATH += /home/ratnavali/workspace/libcarla/install-folder/include/traffic_manager/include/
INCLUDEPATH += /home/ratnavali/anaconda2/include/python2.7
LIBS += -L/home/ratnavali/workspace/libcarla/install-folder/lib/ -lcarla_client -lrpc -lboost_system -lboost_filesystem -lpython2.7


SOURCES += \
    coordinates.cpp \
        main.cpp \
        mainwindow.cpp \
    carlastreamthread.cpp

HEADERS += \
    coordinates.h \
    helper.h \
        mainwindow.h \
    carlastreamthread.h

FORMS += \
        mainwindow.ui
