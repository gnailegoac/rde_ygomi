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

#include "WebRoadEditor.h"

WebGlobeChannelObject::WebGlobeChannelObject(QObject *aParent) :
    QObject(aParent)
{

}

WebGlobeChannelObject::~WebGlobeChannelObject()
{

}

WebRoadEditor::WebRoadEditor():
    mWebChannelObject(new WebGlobeChannelObject())
{
    page()->setWebChannel(&mWebChannel);
    mWebChannel.registerObject(QStringLiteral("widget"), mWebChannelObject.data());
}
