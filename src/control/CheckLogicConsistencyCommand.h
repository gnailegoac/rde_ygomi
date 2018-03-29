/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    CheckLogicConsistencyCommand.h
 *******************************************************************************
 */

#pragma once

#include "BasicCommand.h"
#include <QObject>
#include <QThread>

namespace Controller {

class CheckLogicConsistencyCommand : public QObject, public BasicCommand
{
    Q_OBJECT

public:
    CheckLogicConsistencyCommand();
    void execute(PureMVC::Interfaces::INotification const& aNotification) override;
    static std::string GetCommandName();

public slots:
    void handleResult();

signals:
    void operate();

private:
    QThread thread_;
};

}
