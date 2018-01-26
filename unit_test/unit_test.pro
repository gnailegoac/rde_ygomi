QT -= gui
TARGET = unit_test
CONFIG += console c++11
TEMPLATE = app

CONFIG += conan_basic_setup
include(../build_ut/conanbuildinfo.pri)

SOURCES += \
    UtDouglasPeucker.cpp \
    UtLine.cpp \
    UtNurbsCurve.cpp \
    UtFitNurbs.cpp \
    ../src/model/Point3D.cpp \
    ../src/model/DouglasPeucker.cpp \
    ../src/model/Line.cpp \
    ../src/model/Element.cpp \
    ../src/model/BoundingBox.cpp \
    ../src/model/Curve.cpp \
    ../src/model/NurbsCurve.cpp \
    ../src/model/Poly.cpp \
    ../src/model/external/geo_alg.cpp \
    ../src/model/FitNurbs.cpp \
    ../src/model/Tile.cpp \
    ../src/model/TrafficSign.cpp \
    ../src/model/Road.cpp \
    ../src/model/Lane.cpp \
    ../src/model/Junction.cpp \
    ../src/model/LookUpTable.cpp \
    ../src/model/IdGenerator.cpp \
    UtTileId.cpp

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src

