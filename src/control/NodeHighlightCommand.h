/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    NodeHighlightCommand.h
 *******************************************************************************
 */

#pragma once

#include "BasicCommand.h"
#include "model/Common.h"

namespace osg
{
    class Node;
}

namespace Controller
{
class NodeHighlightCommand : public BasicCommand
{
public:
    NodeHighlightCommand();
    void execute(PureMVC::Interfaces::INotification const& aNotification) override;
    static std::string GetCommandName();
private:
    void highlightNode();
    void dehighlightNode();
    std::vector<osg::Node*> findNode(const std::vector<osg::Node*>& aNodeList);
    std::vector<osg::Node*> getLineNodesByLaneName(const std::string& aLaneNodeName);
    std::vector<osg::Node*> getLineNodesByRoadNode(osg::Node* aNode);
    void highlightNodeOnTreeView();
    uint64_t getIdByNodeName(const std::string& aNodeName);
    void clearSelectNodes(bool aNeedClearNodeOnTree = true);
    void selectNodeIn3DView(const std::string& aName, const std::string& aValue);
    void jumpToNode(const osg::Node* aNode);
private:
    std::vector<osg::Node*> mSelectNodes;
    Model::SelectType mSelectType;
    std::string mSelectNodeName;
};
}
