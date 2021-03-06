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
#include <osg/ShapeDrawable>
#include "facade/ApplicationFacade.h"
#include "CommonFunction.h"
#include "model/Utilities.h"
#include "model/MemoryModel.h"
#include "model/SceneModel.h"
#include "proxy/MainProxy.h"
#include "service/RoadEditParameters.h"

Controller::NodeHighlightCommand::NodeHighlightCommand() :
    mSelectType(Model::SelectType::Line),
    mSelectNodeName("")
{

}

std::vector<osg::Node*> Controller::NodeHighlightCommand::getLineNodesByLaneName(const std::string& aLaneNodeName)
{
    std::vector<osg::Node*> nodeList;
    uint64_t laneId = Model::SceneModel::GetIdByNodeName(aLaneNodeName);
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
            nodeList = Model::SceneModel::GetLineNodesByRoadNode(node);
            mSelectNodeName = nodeName;
            break;
        }
        else if(mSelectType == Model::SelectType::Lane && nodeName.find("Lane") != std::string::npos)
        {
            nodeList = getLineNodesByLaneName(nodeName);
            mSelectNodeName = nodeName;
            break;
        }
        else if(mSelectType == Model::SelectType::Line && nodeName.find("Line") != std::string::npos)
        {
            nodeList.push_back(node);
            mSelectNodeName = nodeName;
            break;
        }
        else if(mSelectType == Model::SelectType::TrafficSign && nodeName.find("Sign") != std::string::npos)
        {
            nodeList.push_back(node);
            mSelectNodeName = nodeName;
            break;
        }
    }

    return nodeList;
}

void Controller::NodeHighlightCommand::highlightNodeOnTreeView()
{
    uint64_t id = Model::SceneModel::GetIdByNodeName(mSelectNodeName);
    if(mSelectType == Model::SelectType::Road)
    {
        ApplicationFacade::SendNotification(ApplicationFacade::SELECT_ROAD_ON_TREE, &id);
    }
    else if(mSelectType == Model::SelectType::Lane)
    {
        ApplicationFacade::SendNotification(ApplicationFacade::SELECT_LANE_ON_TREE, &id);
    }
    else if(mSelectType == Model::SelectType::Line)
    {
        ApplicationFacade::SendNotification(ApplicationFacade::SELECT_LINE_ON_TREE, &id);
    }
    else if(mSelectType == Model::SelectType::TrafficSign)
    {
        ApplicationFacade::SendNotification(ApplicationFacade::SELECT_SIGN_ON_TREE, &id);
    }
}

void Controller::NodeHighlightCommand::clearSelectNodes(bool aNeedClearNodeOnTree)
{
    dehighlightNode();
    if(aNeedClearNodeOnTree)
    {
        ApplicationFacade::SendNotification(ApplicationFacade::UNSELECT_NODE_ON_TREE);
    }

    mSelectNodes.clear();
    mSelectNodeName = "";
}

void Controller::NodeHighlightCommand::execute(PureMVC::Interfaces::INotification const& aNotification)
{
    if (aNotification.getName() == ApplicationFacade::SELECT_NODE)
    {
        std::vector<osg::Node*> nodeList = *CommonFunction::ConvertToNonConstType<std::vector<osg::Node*>>(aNotification.getBody());
        if (Service::RoadEditParameters::Instance()->IsInMultiSelectMode())
        {
            std::vector<osg::Node*> selectNodes = findNode(nodeList);
            if (selectNodes.size() > 0)
            {
                mSelectNodes.insert(mSelectNodes.end(), selectNodes.begin(), selectNodes.end());
                Service::RoadEditParameters::Instance()->AddSelectedElementId(Model::SceneModel::GetIdByNodeName(mSelectNodeName));
            }
        }
        else
        {
            clearSelectNodes();
            mSelectNodes = findNode(nodeList);
            Service::RoadEditParameters::Instance()->SetSelectedElementId(Model::SceneModel::GetIdByNodeName(mSelectNodeName));
        }
        if(mSelectNodes.size() > 0)
        {
            highlightNode();
            highlightNodeOnTreeView();
        }
        else
        {
            Service::RoadEditParameters::Instance()->ClearSelectedElement();
        }
    }
    else if(aNotification.getName() == ApplicationFacade::CHANGE_SELECT_TYPE)
    {
        Model::SelectType selectType = *CommonFunction::ConvertToNonConstType<Model::SelectType>(aNotification.getBody());
        mSelectType = selectType;
    }
    else if(aNotification.getName() == ApplicationFacade::DEHIGHLIGHT_ALL_NODE)
    {
        clearSelectNodes();
    }
    else if(aNotification.getName() == ApplicationFacade::SELECT_NODE_IN_3DVIEW)
    {
        std::pair<QString, QString> selectPair =
                *CommonFunction::ConvertToNonConstType<std::pair<QString, QString>>(aNotification.getBody());
        selectNodeIn3DView(selectPair.first.toStdString(), selectPair.second.toStdString());
    }
}

void Controller::NodeHighlightCommand::jumpToNode(const osg::Node* aNode)
{
    const osg::BoundingSphere& sphere = aNode->getBound();
    osg::Vec3d sphereCenter = sphere.center();
    ApplicationFacade::SendNotification(ApplicationFacade::JUMP_TO_CENTER, &sphereCenter);
}

void Controller::NodeHighlightCommand::selectNodeIn3DView(const std::string& aName, const std::string& aValue)
{
    clearSelectNodes(false);
    if(aName == "Road")
    {
        std::shared_ptr<Model::MemoryModel> memoryModel = getMainProxy()->GetMemoryModel();
        Model::RoadPtr road = memoryModel->GetRoadById(QString::fromStdString(aValue).toULongLong());
        for(const auto& lane : *road->GetLaneList())
        {
            std::vector<osg::Node*> nodeList = getLineNodesByLaneName("Lane:" + std::to_string(lane->GetLaneId()));
            mSelectNodes.insert(mSelectNodes.end(), nodeList.begin(), nodeList.end());
        }
    }
    else if(aName == "Lane")
    {
        mSelectNodes = getLineNodesByLaneName("Lane:" + aValue);
    }
    else if(aName == "LeftLine" || aName == "RightLine")
    {
        std::shared_ptr<Model::SceneModel> sceneModel = getMainProxy()->GetSceneModel();
        mSelectNodes.push_back(sceneModel->GetLineNodeById(QString::fromStdString(aValue).toULongLong()));
    }
    else if(aName == "TrafficSign")
    {
        std::shared_ptr<Model::SceneModel> sceneModel = getMainProxy()->GetSceneModel();
        mSelectNodes.push_back(sceneModel->GetTrafficSignNodeById(QString::fromStdString(aValue).toULongLong()));
    }
    else
    {
        return;
    }

    highlightNode();
    if(mSelectNodes.size() > 0)
    {
        jumpToNode(mSelectNodes[0]);
    }
}

std::string Controller::NodeHighlightCommand::GetCommandName()
{
    return "NodeHighlightCommand";
}

void Controller::NodeHighlightCommand::setSignNodeColor(osg::Node* aSignNode, const osg::Vec4& aColor)
{
    osg::Geode* geode = dynamic_cast<osg::Geode*>(aSignNode);
    osg::Drawable* drawable = geode->getDrawable(0);
    osg::ShapeDrawable* shapeDrawable = dynamic_cast<osg::ShapeDrawable*>(drawable);
    shapeDrawable->setColor(aColor);
}

void Controller::NodeHighlightCommand::highlightNode()
{
    for(auto node : mSelectNodes)
    {
        std::string name = node->getName();
        if(name.find("Sign") != std::string::npos)
        {
            setSignNodeColor(node, osg::Vec4(1.0, 0.0, 0.0, 0.9));
        }
        else
        {
            osg::ref_ptr<osg::Material> material = new osg::Material;
            material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
            material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
            material->setTransparency(osg::Material::FRONT_AND_BACK, 0.9);
            node->getOrCreateStateSet()->setAttributeAndModes(material,
                                                              osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
        }
    }
}

void Controller::NodeHighlightCommand::dehighlightNode()
{
    for(auto node : mSelectNodes)
    {
        std::string name = node->getName();
        if(name.find("Sign") != std::string::npos)
        {
             setSignNodeColor(node, osg::Vec4(1.0, 1.0, 1.0, 0.9));
        }
        else
        {
            osg::ref_ptr<osg::Material> material = new osg::Material;
            material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0, 1.0, 1.0, 1.0));
            material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0, 1.0, 1.0, 1.0));
            material->setTransparency(osg::Material::FRONT_AND_BACK, 0.9);
            node->getOrCreateStateSet()->setAttributeAndModes(material,
                                                              osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
        }
    }
}
