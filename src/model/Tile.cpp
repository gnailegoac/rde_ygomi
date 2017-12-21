/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Tile.cpp
 * @brief
 *******************************************************************************
 */

#include "Tile.h"

Model::Tile::Tile():
    mTileId(0),
    mReferencePoint(std::make_shared<Point3D>()),
    mTrafficSignMap(std::make_shared<TrafficSignMap>()),
    mRoadMap(std::make_shared<RoadMap>()),
    mLaneMap(std::make_shared<LaneMap>()),
    mLineMap(std::make_shared<LineMap>()),
    mJunctionMap(std::make_shared<JunctionMap>())
{

}

Model::Tile::~Tile()
{

}

const int64_t& Model::Tile::GetTileId() const
{
    return mTileId;
}

void Model::Tile::SetTileId(const int64_t& aTileId)
{
    mTileId = aTileId;
}

const Model::Point3DPtr& Model::Tile::GetReferencePoint() const
{
    return mReferencePoint;
}

void Model::Tile::SetReferencePoint(const Point3DPtr& aReferencePoint)
{
    mReferencePoint = aReferencePoint;
}

const Model::TrafficSignMapPtr& Model::Tile::GetTrafficSignMap() const
{
    return mTrafficSignMap;
}

Model::TrafficSignMapPtr Model::Tile::GetMutableTrafficSignMap()
{
    return mTrafficSignMap;
}

Model::TrafficSignPtr Model::Tile::GetTrafficSign(const uint64_t& aId)
{
    if (0 != mTrafficSignMap->count(aId))
    {
        return mTrafficSignMap->at(aId);
    }

    return nullptr;
}

Model::TrafficSignPtr Model::Tile::GetMutableTrafficSign(const uint64_t& aId)
{
    if (0 == mTrafficSignMap->count(aId))
    {
        TrafficSignPtr trafficSign = std::make_shared<TrafficSign>();
        trafficSign->SetTrafficSignId(aId);
        (*mTrafficSignMap)[aId] = trafficSign;
    }

    return mTrafficSignMap->at(aId);
}

const Model::RoadMapPtr& Model::Tile::GetRoadMap() const
{
    return mRoadMap;
}

Model::RoadMapPtr Model::Tile::GetMutableRoadMap()
{
    return mRoadMap;
}

Model::RoadPtr Model::Tile::GetRoad(const uint64_t& aId)
{
    if (0 != mRoadMap->count(aId))
    {
        return mRoadMap->at(aId);
    }

    return nullptr;
}

Model::RoadPtr Model::Tile::GetMutableRoad(const uint64_t& aId)
{
    if (0 == mRoadMap->count(aId))
    {
        RoadPtr road = std::make_shared<Road>();
        road->SetRoadId(aId);
        (*mRoadMap)[aId] = road;
    }

    return mRoadMap->at(aId);
}

const Model::LaneMapPtr& Model::Tile::GetLaneMap() const
{
    return mLaneMap;
}

Model::LaneMapPtr Model::Tile::GetMutableLaneMap()
{
    return mLaneMap;
}

Model::LanePtr Model::Tile::GetLane(const uint64_t& aId)
{
    if (0 != mLaneMap->count(aId))
    {
        return mLaneMap->at(aId);
    }

    return nullptr;
}

Model::LanePtr Model::Tile::GetMutableLane(const uint64_t& aId)
{
    if (0 == mLaneMap->count(aId))
    {
        LanePtr lane = std::make_shared<Lane>();
        lane->SetLaneId(aId);
        (*mLaneMap)[aId] = lane;
    }

    return mLaneMap->at(aId);
}

const Model::LineMapPtr& Model::Tile::GetLineMap() const
{
    return mLineMap;
}

Model::LineMapPtr Model::Tile::GetMutableLineMap()
{
    return mLineMap;
}

Model::LinePtr Model::Tile::GetLine(const uint64_t& aId)
{
    if (0 != mLineMap->count(aId))
    {
        return mLineMap->at(aId);
    }

    return nullptr;
}

Model::LinePtr Model::Tile::GetMutableLine(const uint64_t& aId)
{
    if (0 == mLineMap->count(aId))
    {
        LinePtr line = std::make_shared<Line>();
        line->SetLineId(aId);
        (*mLineMap)[aId] = line;
    }

    return mLineMap->at(aId);
}

const Model::JunctionMapPtr& Model::Tile::GetJunctionMap() const
{
    return mJunctionMap;
}

Model::JunctionMapPtr Model::Tile::GetMutableJunctionMap()
{
    return mJunctionMap;
}

Model::JunctionPtr Model::Tile::GetJunction(const uint64_t& aId)
{
    if (0 != mJunctionMap->count(aId))
    {
        return mJunctionMap->at(aId);
    }

    return nullptr;
}

Model::JunctionPtr Model::Tile::GetMutableJunction(const uint64_t& aId)
{
    if (0 == mJunctionMap->count(aId))
    {
        JunctionPtr jucntion = std::make_shared<Junction>();
        jucntion->SetJunctionId(aId);
        (*mJunctionMap)[aId] = jucntion;
    }

    return mJunctionMap->at(aId);
}
