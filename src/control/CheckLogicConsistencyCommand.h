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
#include "PeqiDataDefine.h"
#include "service/LogicDbLoader.h"

namespace Controller
{
class CheckLogicConsistencyCommand : public BasicCommand
{
public:
    void execute(PureMVC::Interfaces::INotification const& aNotification) override;
    static std::string GetCommandName();
private:
    void checkLogicConsistency(PeqiDataDefine::PointData& sensorData);
    void createQIModel();
    void saveResult(PeqiDataDefine::ResultInfo& result);
};
}
