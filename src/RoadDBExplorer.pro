#-------------------------------------------------
#
# Project created by QtCreator 2017-12-06T14:13:52
#
#-------------------------------------------------

QT       += core gui opengl webenginewidgets xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RoadDatabaseExplorer
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

LIBS += -losg -losgDB -losgGA -losgUtil -losgViewer -losgText -lOpenThreads -lPureMVC -ldl

SOURCES +=\
    RoadDBExplorer.cpp \
    view/MainWindow.cpp \
    view/PickHandler.cpp \
    view/OsgWidget.cpp \
    facade/ApplicationFacade.cpp \
    proxy/MainProxy.cpp \
    model/MemoryModel.cpp \
    model/ConnectionRelation.cpp \
    model/Element.cpp \
    model/Junction.cpp \
    model/BoundingBox.cpp \
    model/Poly3Curve.cpp \
    model/NurbsCurve.cpp \
    model/Curve.cpp \
    model/Line.cpp \
    model/Road.cpp \
    model/Lane.cpp \
    model/TrafficSign.cpp \
    model/Tile.cpp \
    model/Point3D.cpp \
    control/FileParseCommand.cpp \
    control/StartUpCommand.cpp \
    control/BasicCommand.cpp \
    view/MainWindowMediator.cpp \
    CommonFunction.cpp \
    service/NetworkPreferenceProvider.cpp \
    view/WebRoadEditor.cpp \
    view/NetworkPreferenceDialog.cpp \
    model/data_handler/LogicDbFactory.cpp \
    model/data_handler/LogicDbParser.cpp \
    model/data_handler/DbRepository.cpp \
    model/data_handler/DbParseException.cpp \
    model/IdGenerator.cpp \
    model/data_handler/Factory.cpp \
    model/external/geo_alg.cpp \
    model/Poly.cpp \
    model/SceneModel.cpp \
    control/SceneManageCommand.cpp \
    model/DouglasPeucker.cpp \
    model/data_handler/LogicDbSerializer.cpp \
    view/StrokeIntersector.cpp \
    control/NodeHighlightCommand.cpp \
    model/TreeItem.cpp \
    model/TreeModel.cpp \
    model/FitNurbs.cpp \
    model/GeoJsonConverter.cpp \
    model/data_handler/KMLFactory.cpp \
    model/data_handler/KMLSerializer.cpp \
    model/data_handler/KMLInterpreter.cpp \
    model/LookUpTable.cpp \
    control/FileSerializeCommand.cpp \
    model/data_handler/RoadSection.pb.cpp \
    model/data_handler/ProtoBufferFactory.cpp \
    model/data_handler/ProtoBufferSerializer.cpp \
    model/data_handler/ProtoBufferInterpreter.cpp \
    model/data_handler/DLMFactory.cpp \
    model/data_handler/DLMSerializer.cpp \
    model/data_handler/DLMInterpreter.cpp \
    model/data_handler/DLMSetting.cpp \
    service/RoadEditParameters.cpp \
    control/RoadEditCommand.cpp \
    model/RoadModelUtilities.cpp \
    view/DbValidationDialog.cpp \
    view/AboutDialog.cpp \
    service/PointCloudLoader.cpp \
    service/LogicDbLoader.cpp \
    service/AlgoPointDataConverter.cpp \
    control/CheckLogicConsistencyCommand.cpp \
    model/QIModel.cpp \
    view/LogicCheckDialog.cpp

HEADERS  += view/MainWindow.h \
    view/PickHandler.h \
    view/OsgWidget.h \
    facade/ApplicationFacade.h \
    proxy/MainProxy.h \
    model/MemoryModel.h \
    model/Common.h \
    model/ConnectionRelation.h \
    model/Curve.h \
    model/Element.h \
    model/Junction.h \
    model/Lane.h \
    model/Line.h \
    model/NurbsCurve.h \
    model/Point3D.h \
    model/Poly3Curve.h \
    model/Road.h \
    model/Tile.h \
    model/TrafficSign.h \
    model/BoundingBox.h \
    control/FileParseCommand.h \
    control/StartUpCommand.h \
    control/BasicCommand.h \
    view/MainWindowMediator.h \
    CommonFunction.h \
    service/NetworkPreferenceProvider.h \
    view/WebRoadEditor.h \
    view/NetworkPreferenceDialog.h \
    model/data_handler/IFactory.h \
    model/data_handler/IParser.h \
    model/data_handler/ISerializer.h \
    model/data_handler/LogicDbFactory.h \
    model/data_handler/LogicDbParser.h \
    model/data_handler/DbRepository.h \
    model/Utilities.h \
    model/data_handler/DbParseException.h \
    model/IdGenerator.h \
    model/data_handler/Factory.h \
    model/external/geo_alg.h \
    model/Poly.h \
    model/SceneModel.h \
    control/SceneManageCommand.h \
    model/DouglasPeucker.h \
    model/FitNurbs.h \
    model/data_handler/LogicDbSerializer.h \
    view/StrokeIntersector.h \
    control/NodeHighlightCommand.h \
    model/TreeItem.h \
    model/GeoJsonConverter.h \
    control/FileSerializeCommand.h \
    model/TreeModel.h \
    model/GeoJsonConverter.h \
    model/data_handler/KMLFactory.h \
    model/data_handler/KMLSerializer.h \
    model/data_handler/KMLInterpreter.h \
    model/LookUpTable.h \
    model/data_handler/RoadSection.pb.h \
    model/data_handler/ProtoBufferFactory.h \
    model/data_handler/ProtoBufferSerializer.h \
    model/data_handler/ProtoBufferInterpreter.h \
    model/data_handler/DLMFactory.h \
    model/data_handler/DLMSerializer.h \
    model/data_handler/DLMInterpreter.h \
    model/data_handler/DLMSetting.h \
    service/RoadEditParameters.h \
    control/RoadEditCommand.h \
    model/RoadModelUtilities.h \
    view/DbValidationDialog.h \
    view/AboutDialog.h \
    AppMeta.h \
    service/PointCloudLoader.h \
    service/LogicDbLoader.h \
    service/AlgoPointDataConverter.h \
    control/CheckLogicConsistencyCommand.h \
    model/QIModel.h \
    view/LogicCheckDialog.h

FORMS    += view/MainWindow.ui \
    view/NetworkPreferenceDialog.ui \
    view/DbValidationDialog.ui \
    view/AboutDialog.ui \
    view/LogicCheckDialog.ui

config_files.path = $$OUT_PWD/resource/configurationfile
config_files.files += $$files(resource/configurationfile/*)

target.path = $$OUT_PWD/../deploy/release/

INSTALLS += config_files target

RESOURCES += \
    resources.qrc

