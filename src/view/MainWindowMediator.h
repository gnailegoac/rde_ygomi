/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    MainWindowMediator.h
 *******************************************************************************
 */

#pragma once

#include <PureMVC/PureMVC.hpp>

class MainWindow;
class MainProxy;

namespace View
{
class MainWindowMediator: public PureMVC::Patterns::Mediator
{
public:
    static const std::string NAME;
    MainWindowMediator(const void* aViewComponent);
    NotificationNames listNotificationInterests() const;
    void handleNotification(PureMVC::Patterns::INotification const& aNotification);
    void onRegister();
    void onRemove();
private:
    MainWindow* getMainWindow();
    MainProxy* getMainProxy();
};
}

