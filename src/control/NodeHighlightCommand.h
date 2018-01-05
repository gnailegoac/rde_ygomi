/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    FileParseCommand.h
 *******************************************************************************
 */

#pragma once

#include "BasicCommand.h"

namespace osg
{
    class Node;
}

namespace Controller
{
class NodeHighlightCommand : public BasicCommand
{
public:

    enum class SelectType
    {
        Road,
        Lane,
        Line
    };

    NodeHighlightCommand();
    void execute(PureMVC::Interfaces::INotification const& aNotification) override;
    static std::string GetCommandName();
private:
    void highlightNode();
    void dehighlightNode();
    std::vector<osg::Node*> findNode(const std::vector<osg::Node*>& aNodeList);
    std::vector<osg::Node*> getLaneNodesByName(const std::string& aLaneNodeName);

    std::vector<osg::Node*> mSelectNodes;
    SelectType mSelectType;
};
}
