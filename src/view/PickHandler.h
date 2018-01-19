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
#include <osgUtil/PolytopeIntersector>
#include "model/Common.h"

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

    void SetSelectType(const Model::SelectType& aSelectType);

private:
    osg::ref_ptr<StrokeIntersector> getStrokeIntersector(const osgGA::GUIEventAdapter& aEventAdapter);
    osg::ref_ptr<osgUtil::PolytopeIntersector> getPolytopeIntersector(
        const osgGA::GUIEventAdapter& aEventAdapter,
        const double& aWidth = 0.01,
        const double& aHeight = 0.01);
    std::vector<osg::Node*> getNodesFromPolytopeIntersector(const osg::ref_ptr<osgUtil::PolytopeIntersector>& aPolytopeIntersector);
    std::vector<osg::Node*> getNodesFromStrokeIntersector(const osg::ref_ptr<StrokeIntersector>& aStrokeIntersector);
private:
    double mDevicePixelRatio;
    Model::SelectType mSelectType;
};
}
