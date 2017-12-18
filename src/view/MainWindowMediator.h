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

namespace View
{
class MainWindow;
}

class MainProxy;

namespace View
{
class MainWindowMediator: public PureMVC::Patterns::Mediator
{
public:
    static const std::string NAME;
    MainWindowMediator(const void* aViewComponent);
    Mediator::NotificationNames listNotificationInterests(void) const override;
    void handleNotification(PureMVC::Patterns::INotification const& aNotification);
    void onRegister();
    void onRemove();
private:
    std::vector<std::string> searchDatabaseFileList(const QString& aFolderpath);

    View::MainWindow* getMainWindow();
    MainProxy* getMainProxy();
};
}

