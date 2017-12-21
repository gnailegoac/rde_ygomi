/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    SceneManageCommand.cpp
 *******************************************************************************
 */

#include "SceneManageCommand.h"
#include "facade/ApplicationFacade.h"

void Controller::SceneManageCommand::execute(PureMVC::Interfaces::INotification const& aNotification)
{
    if (aNotification.getName() == ApplicationFacade::INIT_SCENE)
    {
        //use memory model create quadtree
    }
}

std::string Controller::SceneManageCommand::GetCommandName()
{
    return "SceneManageCommand";
}
