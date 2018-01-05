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
#include "view/StrokeIntersector.h"
#include <osgViewer/Viewer>
#include <osg/Material>
#include "facade/ApplicationFacade.h"

Controller::PickHandler::PickHandler(double aDevicePixelRatio):
    mDevicePixelRatio(aDevicePixelRatio)
{
}

Controller::PickHandler::~PickHandler()
{
}

osg::ref_ptr<StrokeIntersector> Controller::PickHandler::getStrokeIntersector(const osgGA::GUIEventAdapter& aEventAdapter)
{
    double centerX = aEventAdapter.getXnormalized();
    double centerY = aEventAdapter.getYnormalized();
    osg::ref_ptr<StrokeIntersector> picker(
        new StrokeIntersector(
                osgUtil::Intersector::PROJECTION,
                centerX,
                centerY));
    return picker;
}

bool Controller::PickHandler::handle(const osgGA::GUIEventAdapter& aEventAdapter,
                                     osgGA::GUIActionAdapter& aActionAdapter)
{
    if(aEventAdapter.getEventType() != osgGA::GUIEventAdapter::RELEASE ||
       aEventAdapter.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
    {
        return false;
    }

    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aActionAdapter);

    if(viewer != nullptr)
    {
        osg::ref_ptr<StrokeIntersector> strokeIntersector = getStrokeIntersector(aEventAdapter);
        osgUtil::IntersectionVisitor intersectionVisitor(strokeIntersector);
        osg::Camera* aCamera = viewer->getCamera();
        if(aCamera == nullptr)
        {
            return false;
        }

        aCamera->accept(intersectionVisitor);

        std::vector<osg::Node*> nodeList;
        if(!strokeIntersector->containsIntersections())
        {
            ApplicationFacade::SendNotification(ApplicationFacade::SELECT_NODE, &nodeList);
            return false;
        }

        for(auto intersection : strokeIntersector->getIntersections())
        {
           const osg::NodePath& intersectionNodes = intersection.nodePath;
           for(osg::Node* node : intersectionNodes)
           {
               nodeList.push_back(node);
           }
        }

        ApplicationFacade::SendNotification(ApplicationFacade::SELECT_NODE, &nodeList);
    }

    return true;
}
