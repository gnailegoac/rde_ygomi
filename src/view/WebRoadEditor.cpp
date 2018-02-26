/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    WebRoadEditor.cpp
 * @brief
 *******************************************************************************
 */

#include <QJsonDocument>

#include "facade/ApplicationFacade.h"
#include "WebRoadEditor.h"

namespace
{
const QString scCameraMatrixChange = "dataHandler.setCameraMatrix(%1)";
const QString scPushRoadTile = "dataHandler.pushRoadTile(%1,%2)";
const QString scEntireRoadTiles = "dataHandler.pushEntireRoadTilesExtent(%1)";
const QString scEditRoad = "dataHandler.editRoad(%1)";
}

WebGlobeChannelObject::WebGlobeChannelObject(QObject* aParent) :
    QObject(aParent)
{

}

WebGlobeChannelObject::~WebGlobeChannelObject()
{

}

void WebGlobeChannelObject::setCameraMatrix(const QJsonArray& aMatrix)
{
    osg::Matrixd matrix(aMatrix[0].toDouble(), aMatrix[1].toDouble(), aMatrix[2].toDouble(), aMatrix[3].toDouble(),
            aMatrix[4].toDouble(), aMatrix[5].toDouble(), aMatrix[6].toDouble(), aMatrix[7].toDouble(),
            aMatrix[8].toDouble(), aMatrix[9].toDouble(), aMatrix[10].toDouble(), aMatrix[11].toDouble(),
            aMatrix[12].toDouble(), aMatrix[13].toDouble(), aMatrix[14].toDouble(), aMatrix[15].toDouble());
    emit cameraMatrixChanged(matrix);
}

void WebGlobeChannelObject::fetchRoadListByTile(const QJsonValue& aLevel, const QJsonValue& aTileId)
{
    int level = aLevel.toInt();
    std::uint64_t tileId = static_cast<std::uint64_t>(aTileId.toDouble());
    emit requestRoadsInTile(level, tileId);
}

void WebGlobeChannelObject::addLineData(const QJsonObject& aData)
{
    emit addLineToRoad(aData);
}

WebRoadEditor::WebRoadEditor() :
    mWebChannelObject(new WebGlobeChannelObject())
{
    page()->setWebChannel(&mWebChannel);
    mWebChannel.registerObject(QStringLiteral("widget"), mWebChannelObject.data());
    setupConnections();
}

WebRoadEditor::~WebRoadEditor()
{
    mWebChannel.deregisterObject(mWebChannelObject.data());
}

void WebRoadEditor::ChangeCameraMatrix(const QJsonArray& aMatrix)
{
    QString matrixString(QJsonDocument(aMatrix).toJson());
    QString command = scCameraMatrixChange.arg(matrixString);
    page()->runJavaScript(command);
}

void WebRoadEditor::SendRoadsInTile(int aLevel, const QJsonArray& aRoadArray)
{
    QString roadString(QJsonDocument(aRoadArray).toJson());
    QString command = scPushRoadTile.arg(aLevel).arg(roadString);
    page()->runJavaScript(command);
}

void WebRoadEditor::PushEntireRoadTilesExtent(const QJsonArray& aTileArray)
{
    QString tileArray(QJsonDocument(aTileArray).toJson());
    QString command = scEntireRoadTiles.arg(tileArray);
    page()->runJavaScript(command);
}

void WebRoadEditor::SendRoadToEdit(const QJsonObject& aRoad)
{
    QString roadString(QJsonDocument(aRoad).toJson());
    QString command = scEditRoad.arg(roadString);
    page()->runJavaScript(command);
}

void WebRoadEditor::setupConnections()
{
    connect(mWebChannelObject.data(), &WebGlobeChannelObject::cameraMatrixChanged,
            [=](const osg::Matrixd& aMatrix)
    {
        emit cameraMatrixChanged(aMatrix);
    });

    connect(mWebChannelObject.data(), &WebGlobeChannelObject::requestRoadsInTile,
            [=](const std::uint64_t& aLevel, const std::uint64_t& aTileId)
    {
        std::pair<std::uint64_t, std::uint64_t> tileInfo = std::make_pair(aLevel, aTileId);
        ApplicationFacade::SendNotification(ApplicationFacade::REQUEST_ROADS_IN_TILE, &tileInfo);
    });
}
