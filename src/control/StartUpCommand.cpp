/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    StartUpCommand.cpp
 *******************************************************************************
 */

#include "StartUpCommand.h"
#include "proxy/MainProxy.h"
#include "view/MainWindow.h"
#include "view/MainWindowMediator.h"
#include "facade/ApplicationFacade.h"
#include "CommonFunction.h"

void Controller::StartUpCommand::execute(PureMVC::Interfaces::INotification const& aNotification)
{
    if (aNotification.getName() == ApplicationFacade::START_UP)
    {
        MainProxy* mainProxy = new MainProxy();
        ApplicationFacade::RegisterProxy(mainProxy);
        View::MainWindow* window = CommonFunction::ConvertToNonConstType<View::MainWindow>(aNotification.getBody());
        View::MainWindowMediator* mainWindowMediator = new View::MainWindowMediator(window);
        ApplicationFacade::RegisterMediator(mainWindowMediator);
        window->show();
        ApplicationFacade::SendNotification(ApplicationFacade::CHECK_LOGIC_CONSISTENCY_INIT);
    }
}

std::string Controller::StartUpCommand::GetCommandName()
{
    return "StartUpCommand";
}
