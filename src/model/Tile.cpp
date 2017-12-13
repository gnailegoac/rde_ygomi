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
    mTrafficSignMap(),
    mRoadMap(),
    mLaneMap(),
    mLineMap(),
    mJunctionMap()
{

}

Model::Tile::~Tile()
{

}

std::int64_t Model::Tile::GetTileId() const
{
    return mTileId;
}

void Model::Tile::SetTileId(std::int64_t aTileId)
{
    mTileId = aTileId;
}

const std::unordered_map<std::uint64_t, std::shared_ptr<Model::TrafficSign>>& Model::Tile::GetTrafficSignMap() const
{
    return mTrafficSignMap;
}

std::unordered_map<std::uint64_t, std::shared_ptr<Model::TrafficSign>>* Model::Tile::GetMutableTrafficSignMap()
{
    return &mTrafficSignMap;
}

std::shared_ptr<Model::TrafficSign> Model::Tile::GetTrafficSign(std::uint64_t aId)
{
    if (0 != mTrafficSignMap.count(aId))
    {
        return mTrafficSignMap[aId];
    }

    return nullptr;
}

const std::unordered_map<std::uint64_t, std::shared_ptr<Model::Road>>& Model::Tile::GetRoadMap() const
{
    return mRoadMap;
}

std::unordered_map<std::uint64_t, std::shared_ptr<Model::Road>>* Model::Tile::GetMutableRoadMap()
{
    return &mRoadMap;
}

std::shared_ptr<Model::Road> Model::Tile::GetRoad(std::uint64_t aId)
{
    if (0 != mRoadMap.count(aId))
    {
        return mRoadMap[aId];
    }

    return nullptr;
}

const std::unordered_map<std::uint64_t, std::shared_ptr<Model::Lane>>& Model::Tile::GetLaneMap() const
{
    return mLaneMap;
}

std::unordered_map<std::uint64_t, std::shared_ptr<Model::Lane>>* Model::Tile::GetMutableLaneMap()
{
    return &mLaneMap;
}

std::shared_ptr<Model::Lane> Model::Tile::GetLane(std::uint64_t aId)
{
    if (0 != mLaneMap.count(aId))
    {
        return mLaneMap[aId];
    }

    return nullptr;
}

const std::unordered_map<std::uint64_t, std::shared_ptr<Model::Line>>& Model::Tile::GetLineMap() const
{
    return mLineMap;
}

std::unordered_map<std::uint64_t, std::shared_ptr<Model::Line>>* Model::Tile::GetMutableLineMap()
{
    return &mLineMap;
}

std::shared_ptr<Model::Line> Model::Tile::GetLine(std::uint64_t aId)
{
    if (0 != mLineMap.count(aId))
    {
        return mLineMap[aId];
    }

    return nullptr;
}

const std::unordered_map<std::uint64_t, std::shared_ptr<Model::Junction>>& Model::Tile::GetJunctionMap() const
{
    return mJunctionMap;
}

std::unordered_map<std::uint64_t, std::shared_ptr<Model::Junction>>* Model::Tile::GetMutableJunctionMap()
{
    return &mJunctionMap;
}

std::shared_ptr<Model::Junction> Model::Tile::GetJunction(std::uint64_t aId)
{
    if (0 != mJunctionMap.count(aId))
    {
        return mJunctionMap[aId];
    }

    return nullptr;
}
