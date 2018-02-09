/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DbValidationCommand.cpp
 *******************************************************************************
 */

#include <PureMVC/PureMVC.hpp>
#include "DbValidationCommand.h"
#include "facade/ApplicationFacade.h"
#include "CommonFunction.h"
#include <QString>


void Controller::DbValidationCommand::execute(PureMVC::Interfaces::INotification const& aNotification)
{
    if (aNotification.getName() == ApplicationFacade::DB_VALIDATION_START)
    {
        //do validation
        std::string fileType = aNotification.getType();
        if(fileType.find("file") != std::string::npos)
        {
            QString filePath = *CommonFunction::ConvertToNonConstType<QString>(aNotification.getBody());
            ApplicationFacade::SendNotification(ApplicationFacade::DB_VALIDATION_FINISH, aNotification.getBody(), fileType);
        }
        if(fileType.find("folder") != std::string::npos)
        {
            QString folderPath = *CommonFunction::ConvertToNonConstType<QString>(aNotification.getBody());
            ApplicationFacade::SendNotification(ApplicationFacade::DB_VALIDATION_FINISH, aNotification.getBody(), fileType);
        }
    }
}

std::string Controller::DbValidationCommand::GetCommandName()
{
    return "DbValidationCommand";
}

QString Controller::DbValidationCommand::parseValidationFile(const QString& filePath)
{

}
