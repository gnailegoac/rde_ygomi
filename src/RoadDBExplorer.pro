#-------------------------------------------------
#
# Project created by QtCreator 2017-12-06T14:13:52
#
#-------------------------------------------------

QT       += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RoadDBExplorer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
include(../external.pri)
LIBS += -losg -losgDB -losgGA -losgUtil -losgViewer -losgText -lOpenThreads -lPureMVC

SOURCES +=\
    RoadDBExplorer.cpp \
    view/MainWindow.cpp \
    view/PickHandler.cpp \
    view/OsgWidget.cpp \
    facade/ApplicationFacade.cpp \
    proxy/MainProxy.cpp \
    model/MemoryModel.cpp \
    control/FileOpenCommand.cpp \
    control/StartUpCommand.cpp \
    control/BasicCommand.cpp \
    view/MainWindowMediator.cpp \
    CommonFunction.cpp

HEADERS  += view/MainWindow.h \
    view/PickHandler.h \
    view/OsgWidget.h \
    facade/ApplicationFacade.h \
    proxy/MainProxy.h \
    model/MemoryModel.h \
    control/FileOpenCommand.h \
    control/StartUpCommand.h \
    control/BasicCommand.h \
    view/MainWindowMediator.h \
    CommonFunction.h

FORMS    += view/MainWindow.ui
