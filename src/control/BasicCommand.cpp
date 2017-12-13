/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    BasicCommand.cpp
 *******************************************************************************
 */

#include "BasicCommand.h"
#include "facade/ApplicationFacade.h"
#include "view/MainWindow.h"
#include "proxy/MainProxy.h"
#include "view/MainWindowMediator.h"

MainProxy* Controller::BasicCommand::getMainProxy()
{
    return ApplicationFacade::GetMainProxy();
}

View::MainWindow* Controller::BasicCommand::getMainWindow()
{
    PureMVC::Patterns::IMediator& mediator = ApplicationFacade::RetriveMediator(View::MainWindowMediator::NAME);
    View::MainWindowMediator& mainWindowMediator = dynamic_cast<View::MainWindowMediator&>(mediator);

    const void* viewComponent = mainWindowMediator.getViewComponent();
    View::MainWindow* mainWindow = static_cast<View::MainWindow*>(const_cast<void*>(viewComponent));
    return mainWindow;
}

Controller::BasicCommand::BasicCommand()
{

}

void Controller::BasicCommand::execute(PureMVC::Interfaces::INotification const& aNotification)
{

}
