/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    WebRoadEditor.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include <QJsonArray>
#include <QWebChannel>
#include <QWebEngineView>
#include <osg/Matrixd>

class WebGlobeChannelObject : public QObject
{
    Q_OBJECT
public:
    explicit WebGlobeChannelObject(QObject* aParent = nullptr);
    ~WebGlobeChannelObject();

public slots:
    void setCameraMatrix(QJsonArray aMatrix);
    void fetchRoadListByTile(QJsonValue aLevel, QJsonValue aTileId);

signals:
    void cameraMatrixChanged(const osg::Matrixd& aMatrix);
    void requestRoadsInTile(const int& aLevel, const std::uint64_t& aTileId);
};

class WebRoadEditor : public QWebEngineView
{
    Q_OBJECT
public:
    WebRoadEditor();
    ~WebRoadEditor();

    void ChangeCameraMatrix(const QJsonArray& aMatrix);
    void SendRoadsInTile(const QJsonArray& aRoadArray);

signals:
    void cameraMatrixChanged(const osg::Matrixd& aMatrix);

private:
    void setupConnections();

private:
    QWebChannel mWebChannel;
    QScopedPointer<WebGlobeChannelObject> mWebChannelObject;
};

