/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    FileParseCommand.cpp
 *******************************************************************************
 */

#include <PureMVC/PureMVC.hpp>
#include "FileParseCommand.h"
#include "proxy/MainProxy.h"
#include "view/MainWindow.h"
#include "facade/ApplicationFacade.h"
#include "CommonFunction.h"
#include "model/data_handler/Factory.h"
#include "model/MemoryModel.h"

void Controller::FileParseCommand::execute(PureMVC::Interfaces::INotification const& aNotification)
{
    if (aNotification.getName() == ApplicationFacade::FILE_OPEN_SUCCESS)
    {
        std::string filePath = *CommonFunction::ConvertToNonConstType<std::string>(aNotification.getBody());
        Model::IFactoryPtr factory = Model::Factory::CreateLogicDbFactory(filePath);
        Model::IParserPtr parser = factory->CreateParser();
        Model::MemoryModelPtr memoryModel = parser->Parse();
    }
    else if(aNotification.getName() == ApplicationFacade::FOLDER_OPEN_SUCCESS)
    {
        std::vector<std::string> databaseFilePathList =
                *CommonFunction::ConvertToNonConstType<std::vector<std::string>>(aNotification.getBody());
        Model::IFactoryPtr factory = Model::Factory::CreateLogicDbFactory(databaseFilePathList);
        Model::IParserPtr parser = factory->CreateParser();
        Model::MemoryModelPtr memoryModel = parser->Parse();
    }
}

std::string Controller::FileParseCommand::GetCommandName()
{
    return "FileParseCommand";
}
