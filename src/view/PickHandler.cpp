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
    mDevicePixelRatio(aDevicePixelRatio),
    mSelectType(Model::SelectType::Line)
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

void Controller::PickHandler::SetSelectType(const Model::SelectType& aSelectType)
{
    mSelectType = aSelectType;
}

osg::ref_ptr<osgUtil::PolytopeIntersector> Controller::PickHandler::getPolytopeIntersector(
    const osgGA::GUIEventAdapter& aEventAdapter,
    const double& aWidth,
    const double& aHeight)
{
    float centerX = aEventAdapter.getXnormalized();
    float centerY = aEventAdapter.getYnormalized();
    osg::ref_ptr<osgUtil::PolytopeIntersector> picker(
        new osgUtil::PolytopeIntersector(
                osgUtil::Intersector::PROJECTION,
                centerX - aWidth,
                centerY - aHeight,
                centerX + aWidth,
                centerY + aHeight));
    return picker;
}

std::vector<osg::Node*> Controller::PickHandler::getNodesFromStrokeIntersector(const osg::ref_ptr<StrokeIntersector>& aStrokeIntersector)
{
    std::vector<osg::Node*> nodeList;
    if(!aStrokeIntersector->containsIntersections())
    {
        return nodeList;
    }

    for(auto intersection : aStrokeIntersector->getIntersections())
    {
       const osg::NodePath& intersectionNodes = intersection.nodePath;
       for(osg::Node* node : intersectionNodes)
       {
           nodeList.push_back(node);
       }
    }

    return nodeList;
}

std::vector<osg::Node*> Controller::PickHandler::getNodesFromPolytopeIntersector(const osg::ref_ptr<osgUtil::PolytopeIntersector>& aPolytopeIntersector)
{
    std::vector<osg::Node*> nodeList;
    if(!aPolytopeIntersector->containsIntersections())
    {
        return nodeList;
    }

    for(auto intersection : aPolytopeIntersector->getIntersections())
    {
       const osg::NodePath& intersectionNodes = intersection.nodePath;
       for(osg::Node* node : intersectionNodes)
       {
           nodeList.push_back(node);
       }
    }

    return nodeList;
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
        osg::ref_ptr<osgUtil::IntersectionVisitor> intersectionVisitor;
        osg::ref_ptr<StrokeIntersector> strokeIntersector;
        osg::ref_ptr<osgUtil::PolytopeIntersector> polytopeIntersector;
        if(mSelectType != Model::SelectType::TrafficSign)
        {
            strokeIntersector = getStrokeIntersector(aEventAdapter);
            intersectionVisitor = new osgUtil::IntersectionVisitor(strokeIntersector);
        }
        else
        {
            polytopeIntersector = getPolytopeIntersector(aEventAdapter);
            intersectionVisitor = new osgUtil::IntersectionVisitor(polytopeIntersector);
        }

        osg::Camera* aCamera = viewer->getCamera();
        if(aCamera == nullptr)
        {
            return false;
        }

        aCamera->accept(*intersectionVisitor);

        std::vector<osg::Node*> nodeList;
        if(strokeIntersector != nullptr)
        {
            nodeList = getNodesFromStrokeIntersector(strokeIntersector);
        }
        else if(polytopeIntersector != nullptr)
        {
            nodeList = getNodesFromPolytopeIntersector(polytopeIntersector);
        }

        ApplicationFacade::SendNotification(ApplicationFacade::SELECT_NODE, &nodeList);

        if(nodeList.empty())
        {
            return false;
        }
    }

    return true;
}
