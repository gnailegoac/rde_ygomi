/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file   FileSerializeCommand.h
 * @brief  Definition of FileSerializeCommand.h
 *******************************************************************************
 */
#pragma once

#include "BasicCommand.h"

namespace Controller
{
class FileSerializeCommand : public BasicCommand
{
public:
    void execute(PureMVC::Interfaces::INotification const& aNotification) override;
    static std::string GetCommandName();
};
}
