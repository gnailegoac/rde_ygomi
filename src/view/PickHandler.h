/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    PickHandler.h
 * @brief   To handle pick event
 *******************************************************************************
 */

#pragma once

#include <osgGA/GUIEventHandler>

class StrokeIntersector;
namespace Controller
{
class PickHandler : public osgGA::GUIEventHandler
{
public:
    PickHandler(double aDevicePixelRatio = 1.0);
    virtual ~PickHandler();

    bool handle(const osgGA::GUIEventAdapter& aEventAdapter,
                        osgGA::GUIActionAdapter& aActionAdapter) override;

private:
    osg::ref_ptr<StrokeIntersector> getStrokeIntersector(const osgGA::GUIEventAdapter& aEventAdapter);
    double mDevicePixelRatio;
};
}
