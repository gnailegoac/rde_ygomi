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

#include "WebRoadEditor.h"

namespace
{
const QString scCameraMatrixChange = "dataHandler.setCameraMatrix(%1)";
}

WebGlobeChannelObject::WebGlobeChannelObject(QObject* aParent) :
    QObject(aParent)
{

}

WebGlobeChannelObject::~WebGlobeChannelObject()
{

}

void WebGlobeChannelObject::setCameraMatrix(QJsonArray aMatrix)
{
    osg::Matrixd matrix(aMatrix[0].toDouble(), aMatrix[1].toDouble(), aMatrix[2].toDouble(), aMatrix[3].toDouble(),
            aMatrix[4].toDouble(), aMatrix[5].toDouble(), aMatrix[6].toDouble(), aMatrix[7].toDouble(),
            aMatrix[8].toDouble(), aMatrix[9].toDouble(), aMatrix[10].toDouble(), aMatrix[11].toDouble(),
            aMatrix[12].toDouble(), aMatrix[13].toDouble(), aMatrix[14].toDouble(), aMatrix[15].toDouble());
    emit cameraMatrixChanged(matrix);
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

}

void WebRoadEditor::ChangeCameraMatrix(const QJsonArray& aMatrix)
{
    QString matrixString(QJsonDocument(aMatrix).toJson());
    QString command = scCameraMatrixChange.arg(matrixString);
    page()->runJavaScript(command);
}

void WebRoadEditor::setupConnections()
{
    connect(mWebChannelObject.data(), &WebGlobeChannelObject::cameraMatrixChanged,
            [=](const osg::Matrixd& aMatrix)
    {
        emit cameraMatrixChanged(aMatrix);
    });
}
