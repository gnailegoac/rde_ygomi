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
    : pThread_(nullptr)
{}

void Controller::CheckLogicConsistencyCommand::execute(PureMVC::Interfaces::INotification const& aNotification)
{
    if (aNotification.getName() == ApplicationFacade::CHECK_LOGIC_CONSISTENCY)
    {
        if (pThread_ == nullptr)
        {
            pThread_ = new QThread();

            MainProxy& mainProxy = dynamic_cast<MainProxy&>(ApplicationFacade::RetriveProxy(MainProxy::NAME));
            mainProxy.GetQIModel()->moveToThread(pThread_);

            connect(pThread_, SIGNAL(started()), mainProxy.GetQIModel().get(), SLOT(process()));
            connect(pThread_, SIGNAL(finished()), this, SLOT(onThreadEnd()));
            pThread_->start();

            ApplicationFacade::SendNotification(ApplicationFacade::CHECK_LOGIC_CONSISTENCY_START);
        }
        else
        {
            ApplicationFacade::SendNotification(ApplicationFacade::CHECK_LOGIC_CONSISTENCY_BUSY);
        }
    }
}

std::string Controller::CheckLogicConsistencyCommand::GetCommandName()
{
    return "CheckLogicConsistencyCommand";
}

void Controller::CheckLogicConsistencyCommand::onThreadEnd()
{
    if (pThread_ != nullptr)
    {
        delete pThread_;
        pThread_ = nullptr;

        MainProxy& mainProxy = dynamic_cast<MainProxy&>(ApplicationFacade::RetriveProxy(MainProxy::NAME));
        if (mainProxy.GetQIModel()->isSuccess())
            ApplicationFacade::SendNotification(ApplicationFacade::CHECK_LOGIC_CONSISTENCY_SUCCESS);
        else
            ApplicationFacade::SendNotification(ApplicationFacade::CHECK_LOGIC_CONSISTENCY_FAIL);
    }
}
