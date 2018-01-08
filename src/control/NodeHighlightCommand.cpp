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
    mSelectType(Model::SelectType::Line)
{

}

std::vector<osg::Node*> Controller::NodeHighlightCommand::getLineNodesByRoadNode(osg::Node* aNode)
{
    std::vector<osg::Node*> lineNodeList;
    osg::Group* roadNode = dynamic_cast<osg::Group*>(aNode);
    int childrenNum = roadNode->getNumChildren();
    for(int childIndex = 0; childIndex < childrenNum; ++childIndex)
    {
        osg::Node* laneNode = roadNode->getChild(childIndex);
        std::string laneName = laneNode->getName();
        if(laneName.find("Lane") != std::string::npos)
        {
            std::vector<osg::Node*> nodeList = getLineNodesByLaneName(laneName);
            lineNodeList.insert(lineNodeList.end(), nodeList.begin(), nodeList.end());
        }
    }

    return lineNodeList;
}

std::vector<osg::Node*> Controller::NodeHighlightCommand::getLineNodesByLaneName(const std::string& aLaneNodeName)
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
                osg::Node* leftLineNode = sceneModel->GetLineNodeById(leftLine->GetLineId()).get();
                if(leftLineNode != nullptr)
                {
                    nodeList.push_back(leftLineNode);
                }
            }

            Model::LinePtr rightLine = lane->GetRightLine();
            if(rightLine != nullptr)
            {
                osg::Node* rightLineNode = sceneModel->GetLineNodeById(rightLine->GetLineId()).get();
                if(rightLineNode != nullptr)
                {
                    nodeList.push_back(rightLineNode);
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
        if(mSelectType == Model::SelectType::Road && nodeName.find("Road") != std::string::npos)
        {
            nodeList = getLineNodesByRoadNode(node);
            break;
        }
        else if(mSelectType == Model::SelectType::Lane && nodeName.find("Lane") != std::string::npos)
        {
            nodeList = getLineNodesByLaneName(nodeName);
            break;
        }
        else if(mSelectType == Model::SelectType::Line && nodeName.find("Line") != std::string::npos)
        {
            nodeList.push_back(node);
            break;
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
    else if(aNotification.getName() == ApplicationFacade::CHANGE_SELECT_TYPE)
    {
        Model::SelectType selectType = *CommonFunction::ConvertToNonConstType<Model::SelectType>(aNotification.getBody());
        mSelectType = selectType;
    }
    else if(aNotification.getName() == ApplicationFacade::DEHIGHLIGHT_ALL_NODE)
    {
        dehighlightNode();
        mSelectNodes.clear();
    }
}

std::string Controller::NodeHighlightCommand::GetCommandName()
{
    return "NodeHighlightCommand";
}

void Controller::NodeHighlightCommand::highlightNode()
{
    for(auto node : mSelectNodes)
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
    for(auto node : mSelectNodes)
    {
        osg::ref_ptr<osg::Material> material = new osg::Material;
        material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
        material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
        material->setTransparency(osg::Material::FRONT_AND_BACK, 0.9);
        node->getOrCreateStateSet()->setAttributeAndModes(material,
                                                          osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    }
}
