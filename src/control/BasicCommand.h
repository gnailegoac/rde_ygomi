/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    BasicCommand.h
 *******************************************************************************
 */

#pragma once

#include <string>
#include <PureMVC/PureMVC.hpp>

class MainProxy;

namespace View
{
class MainWindow;
}

namespace Controller
{
class BasicCommand : public PureMVC::Patterns::SimpleCommand
{
public:
    BasicCommand();
    void execute(PureMVC::Interfaces::INotification const& aNotification) override;

protected:
    MainProxy* getMainProxy();
    View::MainWindow* getMainWindow();
};
}

