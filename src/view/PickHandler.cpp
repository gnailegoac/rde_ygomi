/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    PickHandler.cpp
 * @brief   To handle pick event
 *******************************************************************************
 */

#include "PickHandler.h"

#include <iostream>

#include <osg/io_utils>

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>

#include <osgViewer/Viewer>

Controller::PickHandler::PickHandler(double aDevicePixelRatio)
  : mDevicePixelRatio(aDevicePixelRatio)
{
}

Controller::PickHandler::~PickHandler()
{
}

bool Controller::PickHandler::Handle(const osgGA::GUIEventAdapter& aEventAdapter,
                                     osgGA::GUIActionAdapter& aActionAdapter)
{
    if(aEventAdapter.getEventType() != osgGA::GUIEventAdapter::RELEASE &&
       aEventAdapter.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
    {
        return false;
    }

    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aActionAdapter);

    if(viewer)
    {
        osgUtil::LineSegmentIntersector* aIntersector =
                new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW,
                                                    aEventAdapter.getX() * mDevicePixelRatio,
                                                    aEventAdapter.getY() * mDevicePixelRatio);
        osgUtil::IntersectionVisitor aIntersectionVisitor(aIntersector);
        osg::Camera* aCamera = viewer->getCamera();
        if(!aCamera)
        {
            return false;
        }

        aCamera->accept(aIntersectionVisitor);

        if(!aIntersector->containsIntersections())
        {
            return false;
        }

        auto aIntersections = aIntersector->getIntersections();
        std::cout << "Got " << aIntersections.size() << " intersections:\n";
        for(auto&& intersection : aIntersections)
        {
            std::cout << "  - Local intersection point = " << intersection.localIntersectionPoint << "\n";
        }
    }

    return true;
}
