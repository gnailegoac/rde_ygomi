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

Tile::Tile():
    mTileId(0),
    mTrafficSignMap(),
    mRoadMap(),
    mLaneMap(),
    mLineMap(),
    mJunctionMap()
{

}

Tile::~Tile()
{

}

std::int64_t Tile::GetTileId() const
{
    return mTileId;
}

void Tile::SetTileId(std::int64_t aTileId)
{
    mTileId = aTileId;
}

const std::unordered_map<std::uint64_t, std::shared_ptr<TrafficSign>>& Tile::GetTrafficSignMap() const
{
    return mTrafficSignMap;
}

std::unordered_map<std::uint64_t, std::shared_ptr<TrafficSign>>* Tile::GetMutableTrafficSignMap()
{
    return &mTrafficSignMap;
}

std::shared_ptr<TrafficSign> Tile::GetTrafficSign(std::uint64_t aId)
{
    if (0 != mTrafficSignMap.count(aId))
    {
        return mTrafficSignMap[aId];
    }

    return nullptr;
}

const std::unordered_map<std::uint64_t, std::shared_ptr<Road>>& Tile::GetRoadMap() const
{
    return mRoadMap;
}

std::unordered_map<std::uint64_t, std::shared_ptr<Road>>* Tile::GetMutableRoadMap()
{
    return &mRoadMap;
}

std::shared_ptr<Road> Tile::GetRoad(std::uint64_t aId)
{
    if (0 != mRoadMap.count(aId))
    {
        return mRoadMap[aId];
    }

    return nullptr;
}

const std::unordered_map<std::uint64_t, std::shared_ptr<Lane>>& Tile::GetLaneMap() const
{
    return mLaneMap;
}

std::unordered_map<std::uint64_t, std::shared_ptr<Lane>>* Tile::GetMutableLaneMap()
{
    return &mLaneMap;
}

std::shared_ptr<Lane> Tile::GetLane(std::uint64_t aId)
{
    if (0 != mLaneMap.count(aId))
    {
        return mLaneMap[aId];
    }

    return nullptr;
}

const std::unordered_map<std::uint64_t, std::shared_ptr<Line>>& Tile::GetLineMap() const
{
    return mLineMap;
}

std::unordered_map<std::uint64_t, std::shared_ptr<Line>>* Tile::GetMutableLineMap()
{
    return &mLineMap;
}

std::shared_ptr<Line> Tile::GetLine(std::uint64_t aId)
{
    if (0 != mLineMap.count(aId))
    {
        return mLineMap[aId];
    }

    return nullptr;
}

const std::unordered_map<std::uint64_t, std::shared_ptr<Junction>>& Tile::GetJunctionMap() const
{
    return mJunctionMap;
}

std::unordered_map<std::uint64_t, std::shared_ptr<Junction>>* Tile::GetMutableJunctionMap()
{
    return &mJunctionMap;
}

std::shared_ptr<Junction> Tile::GetJunction(std::uint64_t aId)
{
    if (0 != mJunctionMap.count(aId))
    {
        return mJunctionMap[aId];
    }

    return nullptr;
}
