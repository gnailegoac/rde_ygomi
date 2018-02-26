/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file   RoadModelUtilities.cpp
 * @brief  Definition of RoadModelUtilities.cpp
 *******************************************************************************
 */
#include "RoadModelUtilities.h"
#include "Lane.h"
#include "Road.h"
#include "Tile.h"

Model::LanePtr Model::GetLeftMostLane(const Model::RoadPtr& aRoad)
{
    for (size_t i = 0; i < aRoad->GetLaneListSize(); ++i)
    {
        LanePtr lanePtr = aRoad->GetLane(i);
        // After data validation, there will be only one lane in a road has no left-lane or right-lane.
        if (0 == lanePtr->GetLeftLaneId())
        {
            return lanePtr;
        }
    }

    return nullptr;
}

Model::LanePtr Model::GetRightMostLane(const Model::RoadPtr& aRoad)
{
    for (size_t i = 0; i < aRoad->GetLaneListSize(); ++i)
    {
        LanePtr lanePtr = aRoad->GetLane(i);
        // After data validation, there will be only one lane in a road has no right-lane.
        if (0 == lanePtr->GetRightLaneId())
        {
            return lanePtr;
        }
    }

    return nullptr;
}

Model::LanePtr Model::GetLeftLane(const Model::LanePtr& aLane)
{
    std::uint64_t leftLaneId = aLane->GetLeftLaneId();

    if (0 != leftLaneId)
    {
        return aLane->GetRoad()->GetTile()->GetLane(leftLaneId);
    }

    return nullptr;
}

Model::LanePtr Model::GetRightLane(const Model::LanePtr& aLane)
{
    std::uint64_t rightLaneId = aLane->GetRightLaneId();

    if (0 != rightLaneId)
    {
        return aLane->GetRoad()->GetTile()->GetLane(rightLaneId);
    }

    return nullptr;
}

