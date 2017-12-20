QT -= gui core
TARGET = unit_test
CONFIG += console c++11
TEMPLATE = app

CONFIG += conan_basic_setup
include(../build_ut/conanbuildinfo.pri)

SOURCES += \
    UtDouglasPeucker.cpp \
    ../src/model/Point3D.cpp \
    ../src/model/DouglasPeucker.cpp

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src
