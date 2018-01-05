/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    NodeHighlightCommand.cpp
 *******************************************************************************
 */

#include "NodeHighlightCommand.h"
#include <QString>
#include <osg/Node>
#include <osg/Material>
#include "facade/ApplicationFacade.h"
#include "CommonFunction.h"
#include "model/Utilities.h"
#include "model/MemoryModel.h"
#include "model/SceneModel.h"
#include "proxy/MainProxy.h"

Controller::NodeHighlightCommand::NodeHighlightCommand() :
    mSelectType(Controller::NodeHighlightCommand::SelectType::Lane)
{

}

std::vector<osg::Node*> Controller::NodeHighlightCommand::getLaneNodesByName(const std::string& aLaneNodeName)
{
    std::vector<osg::Node*> nodeList;
    std::vector<std::string> results;
    results = strings::Split(aLaneNodeName, ":");
    if(results.size() != 2)
    {
        return nodeList;
    }

    QString laneIdString = QString::fromStdString(results[1]);
    uint64_t laneId = laneIdString.toULong();
    MainProxy* mainProxy = getMainProxy();
    std::shared_ptr<Model::MemoryModel> memoryModel = mainProxy->GetMemoryModel();
    std::shared_ptr<Model::SceneModel> sceneModel = mainProxy->GetSceneModel();
    if(memoryModel != nullptr && sceneModel != nullptr)
    {
        std::shared_ptr<Model::Lane> lane = memoryModel->GetLaneById(laneId);
        if(lane != nullptr)
        {
            Model::LinePtr leftLine = lane->GetLeftLine();
            if(leftLine != nullptr)
            {
                osg::ref_ptr<osg::Node> leftLineNode = sceneModel->GetLineNodeById(leftLine->GetLineId());
                if(leftLineNode != nullptr)
                {
                    nodeList.push_back(leftLineNode.get());
                }
            }

            Model::LinePtr rightLine = lane->GetRightLine();
            if(rightLine != nullptr)
            {
                osg::ref_ptr<osg::Node> rightLineNode = sceneModel->GetLineNodeById(rightLine->GetLineId());
                if(rightLineNode != nullptr)
                {
                    nodeList.push_back(rightLineNode.get());
                }
            }
        }
    }

    return nodeList;
}

std::vector<osg::Node*> Controller::NodeHighlightCommand::findNode(const std::vector<osg::Node*>& aNodeList)
{
    std::vector<osg::Node*> nodeList;
    for(const auto& node : aNodeList)
    {
        std::string nodeName = node->getName();
        if(mSelectType == Controller::NodeHighlightCommand::SelectType::Road && nodeName.find("Road") != std::string::npos)
        {
            nodeList.push_back(node);
        }
        else if(mSelectType == Controller::NodeHighlightCommand::SelectType::Lane && nodeName.find("Lane") != std::string::npos)
        {
            nodeList = getLaneNodesByName(nodeName);
        }
        else if(mSelectType == Controller::NodeHighlightCommand::SelectType::Line && nodeName.find("Line") != std::string::npos)
        {
            nodeList.push_back(node);
        }
    }

    return nodeList;
}

void Controller::NodeHighlightCommand::execute(PureMVC::Interfaces::INotification const& aNotification)
{
    if (aNotification.getName() == ApplicationFacade::SELECT_NODE)
    {
        std::vector<osg::Node*> nodeList = *CommonFunction::ConvertToNonConstType<std::vector<osg::Node*>>(aNotification.getBody());
        dehighlightNode();
        mSelectNodes.clear();
        mSelectNodes = findNode(nodeList);
        if(mSelectNodes.size() > 0)
        {
            highlightNode();
        }
    }
}

std::string Controller::NodeHighlightCommand::GetCommandName()
{
    return "NodeHighlightCommand";
}

void Controller::NodeHighlightCommand::highlightNode()
{
    for(auto& node : mSelectNodes)
    {
        osg::ref_ptr<osg::Material> material = new osg::Material;
        material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
        material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
        material->setTransparency(osg::Material::FRONT_AND_BACK, 0.9);
        node->getOrCreateStateSet()->setAttributeAndModes(material,
                                                           osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    }
}

void Controller::NodeHighlightCommand::dehighlightNode()
{
    for(auto& node : mSelectNodes)
    {
        osg::ref_ptr<osg::Material> material = new osg::Material;
        material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
        material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
        material->setTransparency(osg::Material::FRONT_AND_BACK, 0.9);
        node->getOrCreateStateSet()->setAttributeAndModes(material,
                                                           osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    }
}
