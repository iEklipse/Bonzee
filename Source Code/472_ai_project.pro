#-------------------------------------------------
#
# Project created by QtCreator 2017-09-25T19:33:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 472_ai_project
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        board.cpp \
        game.cpp \
        player.cpp \
    ai.cpp \
    integer.cpp

HEADERS += \
        mainwindow.h \
        board.h \
        game.h \
        player.h \
    ai.h \
    integer.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    resources.qrc
