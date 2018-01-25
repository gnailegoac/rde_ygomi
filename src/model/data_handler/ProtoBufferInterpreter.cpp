/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    ProtoBufferInterpreter.cpp
 * @brief
 *******************************************************************************
 */

#include "ProtoBufferInterpreter.h"

#include <QDebug>
#include <iostream>

#include "RoadSection.pb.h"
#include "../MemoryModel.h"
#include "../Tile.h"

Model::ProtoBufferInterpreter::ProtoBufferInterpreter(const std::string& aFileName,
                                                      const double& aInterval):
    mFileName(aFileName),
    mInterval(aInterval)
{

}

Model::ProtoBufferInterpreter::~ProtoBufferInterpreter()
{

}

bool Model::ProtoBufferInterpreter::SaveRoadSections(const Model::MemoryModelPtr& aMemoryModel)
{
    RoadSections roadSections;
    const TileMapPtr& tileMap = aMemoryModel->GetTileMap();
    for (const auto& tile : *tileMap)
    {
        const RoadMapPtr& roadMap = tile.second->GetRoadMap();
        for (const auto& road : *roadMap)
        {
            saveRoad(roadSections.add_roadsections(), road.second, tileMap);
        }
    }

    return true;
}

void Model::ProtoBufferInterpreter::saveRoad(RoadSection* aRoadSection,
                                             const Model::RoadPtr& aRoad,
                                             const Model::TileMapPtr& aTileMap)
{
    aRoadSection->set_sectionid(aRoad->GetRoadId());

    LaneModel *laneModel = aRoadSection->mutable_lanemodel();
    LanePtr lane = getLeftMostLane(aRoad);

    if (lane)
    {
        // Currently, we use left line of left-most lane as the centerline of the road.
        // Maybe we could use the center line of the left-most line and right-most line.
        saveGeometry(aRoadSection->mutable_roadsectioncenterline(), lane->GetLeftLine());

        // TODO: LandMark. Now we have TrafficSign, but it belongs to a tile not a road.
        // so can not add LandMark, will add if TrafficSign belongs to a road.

        // number of lane_boundaries should be number of lanes plus 1.
        saveLane(laneModel->add_lanes(), lane, aTileMap);
        saveLaneBoundary(laneModel->add_boundaries(), lane->GetLeftLine());
        saveLaneBoundary(laneModel->add_boundaries(), lane->GetRightLine());

        while ((lane = getRightLane(lane)) != nullptr)
        {
            saveLane(laneModel->add_lanes(), lane, aTileMap);
            saveLaneBoundary(laneModel->add_boundaries(), lane->GetRightLine());
        }
    }
}

void Model::ProtoBufferInterpreter::saveLane(::Lane* aLanePb,
                                             const Model::LanePtr& aLane,
                                             const Model::TileMapPtr& aTileMap)
{
    // TODO: Set Lane Type when we have

    aLanePb->set_laneid(getLaneIndex(aLane));

    // Set predecessor lane id, actually it is lane index in predecessor road
    // as predecessor lane may not in the same tile as current lane, search through tileMap.
    LanePtr predecessorLane = getPredecessorLane(aLane, aTileMap);
    if (predecessorLane != nullptr)
    {
        aLanePb->add_predecessorlaneids(getLaneIndex(predecessorLane));
        // Set predecessor road section id, first get predecessor lane, then use its road id.
        aLanePb->set_predecessorsectionid(predecessorLane->GetRoad()->GetRoadId());
    }

}

void Model::ProtoBufferInterpreter::saveLaneBoundary(LaneBoundary* aBoundary, const Model::LinePtr& aLine)
{

}

void Model::ProtoBufferInterpreter::saveGeometry(Geometry* aGeometry, const Model::LinePtr& aLine)
{

}

Model::LanePtr Model::ProtoBufferInterpreter::getLeftMostLane(const Model::RoadPtr& aRoad)
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

Model::LanePtr Model::ProtoBufferInterpreter::getRightLane(const Model::LanePtr& aLane)
{
    std::uint64_t rightLaneId = aLane->GetRightLaneId();
    if (0 != rightLaneId)
    {
        return aLane->GetRoad()->GetTile()->GetLane(rightLaneId);
    }

    return nullptr;
}

uint32_t Model::ProtoBufferInterpreter::getLaneIndex(const Model::LanePtr& aLane)
{
    std::uint32_t index = 0;
    RoadPtr road = aLane->GetRoad();
    LanePtr lane = getLeftMostLane(road);
    while (nullptr != lane)
    {
        if (lane == aLane)
        {
            break;
        }
        ++index;
        lane = getRightLane(lane);
    }

    return index;
}

Model::LanePtr Model::ProtoBufferInterpreter::getPredecessorLane(const Model::LanePtr& aLane,
                                                                 const Model::TileMapPtr& aTileMap)
{
    std::uint64_t predecessorLaneId = aLane->GetPredecessorLaneId();

    for (const auto& tile : *aTileMap)
    {
        LanePtr lane = tile.second->GetLane(predecessorLaneId);
        if (lane != nullptr)
        {
            return lane;
        }
    }

    return nullptr;
}

