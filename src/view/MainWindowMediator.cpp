/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    MainWindowMediator.cpp
 *******************************************************************************
 */

#include "MainWindowMediator.h"
#include "facade/ApplicationFacade.h"
#include "proxy/MainProxy.h"

const std::string View::MainWindowMediator::NAME = "MainWindowMediator";

View::MainWindowMediator::MainWindowMediator(const void* aViewComponent)
    : Mediator(NAME, aViewComponent)
{

}

PureMVC::Patterns::Mediator::NotificationNames View::MainWindowMediator::listNotificationInterests() const
{
    PureMVC::Patterns::StdContainerAggregate<std::list<NotificationNames::element_type::type>>* result =
            new PureMVC::Patterns::StdContainerAggregate<std::list<NotificationNames::element_type::type>>;
    return NotificationNames(result);
}

MainProxy* View::MainWindowMediator::getMainProxy()
{
    MainProxy& mainProxy = dynamic_cast<MainProxy&>(ApplicationFacade::RetriveProxy(MainProxy::NAME));
    return &mainProxy;
}

void View::MainWindowMediator::handleNotification(PureMVC::Patterns::INotification const& aNotification)
{

}

void View::MainWindowMediator::onRemove()
{

}

void View::MainWindowMediator::onRegister()
{

}
