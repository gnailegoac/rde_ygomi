/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file   RoadModelUtilities.h
 * @brief  Definition of RoadModelUtilities.h
 *******************************************************************************
 */
#pragma once

#include <memory>

namespace Model
{
class Lane;
class Road;
typedef std::shared_ptr<Lane> LanePtr;
typedef std::shared_ptr<Road> RoadPtr;

LanePtr GetLeftMostLane(const RoadPtr& aRoad);
LanePtr GetRightMostLane(const RoadPtr& aRoad);
LanePtr GetLeftLane(const LanePtr& aLane);
LanePtr GetRightLane(const LanePtr& aLane);
}
