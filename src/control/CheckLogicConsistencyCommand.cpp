/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    CheckLogicConsistencyCommand.cpp
 *******************************************************************************
 */

#include "CheckLogicConsistencyCommand.h"
#include "facade/ApplicationFacade.h"
#include "proxy/MainProxy.h"
#include "model/MemoryModel.h"
#include "model/QIModel.h"

Controller::CheckLogicConsistencyCommand::CheckLogicConsistencyCommand()
{
}

void Controller::CheckLogicConsistencyCommand::execute(PureMVC::Interfaces::INotification const& aNotification)
{
    if (aNotification.getName() == ApplicationFacade::CHECK_LOGIC_CONSISTENCY_INIT)
    {
        MainProxy& mainProxy = dynamic_cast<MainProxy&>(ApplicationFacade::RetriveProxy(MainProxy::NAME));
        mainProxy.GetQIModel()->moveToThread(&thread_);

        connect(this, SIGNAL(operate()), mainProxy.GetQIModel().get(), SLOT(process()));
        connect(mainProxy.GetQIModel().get(), SIGNAL(resultReady()), this, SLOT(handleResult()));

        thread_.start();
    }
    else if (aNotification.getName() == ApplicationFacade::CHECK_LOGIC_CONSISTENCY)
    {
        emit operate();

        ApplicationFacade::SendNotification(ApplicationFacade::CHECK_LOGIC_CONSISTENCY_START);
    }
}

std::string Controller::CheckLogicConsistencyCommand::GetCommandName()
{
    return "CheckLogicConsistencyCommand";
}

void Controller::CheckLogicConsistencyCommand::handleResult()
{
    MainProxy& mainProxy = dynamic_cast<MainProxy&>(ApplicationFacade::RetriveProxy(MainProxy::NAME));
    if (mainProxy.GetQIModel()->isSuccess())
        ApplicationFacade::SendNotification(ApplicationFacade::CHECK_LOGIC_CONSISTENCY_SUCCESS);
    else
        ApplicationFacade::SendNotification(ApplicationFacade::CHECK_LOGIC_CONSISTENCY_FAIL);
}
