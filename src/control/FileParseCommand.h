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
#include "model/data_handler/IParser.h"

namespace Controller
{
class FileParseCommand : public BasicCommand
{
public:
    void execute(PureMVC::Interfaces::INotification const& aNotification) override;
    static std::string GetCommandName();
private:
    void setMemoryModel(const Model::MemoryModelPtr& aMemoryModel);
    void ceateSceneModel();
    void createTreeModel(const Model::MemoryModelPtr& aMemoryModel);
};
}

