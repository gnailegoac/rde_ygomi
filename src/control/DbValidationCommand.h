/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DbValidationCommand.h
 *******************************************************************************
 */

#pragma once

#include "BasicCommand.h"
#include<QString>

namespace Controller
{
class DbValidationCommand : public BasicCommand
{
public:
    void execute(PureMVC::Interfaces::INotification const& aNotification) override;
    static std::string GetCommandName();
private:
    QString parseValidationFile(const QString& filePath);
};
}
