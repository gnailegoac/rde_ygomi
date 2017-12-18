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
#include <QWebChannel>
#include <QWebEngineView>

class WebGlobeChannelObject : public QObject
{
public:
    explicit WebGlobeChannelObject(QObject* aParent = nullptr);
    ~WebGlobeChannelObject();
};

class WebRoadEditor : public QWebEngineView
{
public:
    WebRoadEditor();

private:
    QWebChannel mWebChannel;
    QScopedPointer<WebGlobeChannelObject> mWebChannelObject;
};

