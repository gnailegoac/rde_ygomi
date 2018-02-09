/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    RoadEditCommand.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "BasicCommand.h"
#include "model/Road.h"

namespace Controller
{

class RoadEditCommand : public BasicCommand
{
public:
    void execute(PureMVC::Interfaces::INotification const& aNotification) override;
    static std::string GetCommandName();

private:
    void mergeRoad(const uint64_t& aFromRoadId, const uint64_t& aToRoadId);
    bool isLaneInRoad(const Model::RoadPtr& aRoad, const uint64_t& aLaneId);
    bool isRoadConnected(const Model::RoadPtr& aFromRoad, const Model::RoadPtr& aToRoad);
    bool canRoadsBeMerged(Model::RoadPtr& aFromRoad, Model::RoadPtr& aToRoad);
    void updateRoadConnection(Model::RoadPtr& aFromRoad, Model::RoadPtr& aToRoad);
};

}
