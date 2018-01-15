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

signals:
    void cameraMatrixChanged(const osg::Matrixd& aMatrix);
};

class WebRoadEditor : public QWebEngineView
{
    Q_OBJECT
public:
    WebRoadEditor();
    ~WebRoadEditor();

    void ChangeCameraMatrix(const QJsonArray& aMatrix);

signals:
    void cameraMatrixChanged(const osg::Matrixd& aMatrix);

private:
    void setupConnections();

private:
    QWebChannel mWebChannel;
    QScopedPointer<WebGlobeChannelObject> mWebChannelObject;
};

