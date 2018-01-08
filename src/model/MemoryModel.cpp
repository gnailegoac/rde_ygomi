/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    MemoryModel.cpp
 *******************************************************************************
 */

#include "MemoryModel.h"

#include "Tile.h"
#include "Lane.h"
#include "IdGenerator.h"

Model::MemoryModel::MemoryModel():
    mTileMap(std::make_shared<std::unordered_map<std::int64_t, std::shared_ptr<Tile>>>()),
    mCurveIdMap(std::make_shared<IdGeneratorMap<Curve>>()),
    mLineIdMap(std::make_shared<IdGeneratorMap<Line>>()),
    mLaneIdMap(std::make_shared<IdGeneratorMap<Lane>>()),
    mTrafficSignIdMap(std::make_shared<IdGeneratorMap<TrafficSign>>())
{

}

Model::MemoryModel::~MemoryModel()
{

}

const Model::TileMapPtr& Model::MemoryModel::GetTileMap() const
{
    return mTileMap;
}

Model::TileMapPtr Model::MemoryModel::GetMutableTileMap()
{
    return mTileMap;
}

Model::TileConstPtr Model::MemoryModel::GetTile(const std::int64_t& aId)
{
    if (0 != mTileMap->count(aId))
    {
        return mTileMap->at(aId);
    }

    return nullptr;
}

Model::TilePtr Model::MemoryModel::GetMutableTile(const std::int64_t& aId)
{
    if (0 == mTileMap->count(aId))
    {
        TilePtr tile = std::make_shared<Tile>();
        tile->SetTileId(aId);
        (*mTileMap)[aId] = tile;
    }

    return mTileMap->at(aId);
}

const uint64_t& Model::MemoryModel::GetCurveIntId(const std::string& aId)
{
    return mCurveIdMap->GetId(aId);
}

const std::string& Model::MemoryModel::GetCurveTextId(const std::uint64_t& aId) const
{
    return mCurveIdMap->GetTextId(aId);
}

const uint64_t& Model::MemoryModel::GetLineIntId(const std::string& aId)
{
    return mLineIdMap->GetId(aId);
}

const std::string& Model::MemoryModel::GetLineTextId(const std::uint64_t& aId) const
{
    return mLineIdMap->GetTextId(aId);
}

const uint64_t& Model::MemoryModel::GetLaneIntId(const std::string& aId)
{
    return mLaneIdMap->GetId(aId);
}

const std::string& Model::MemoryModel::GetLaneTextId(const std::uint64_t& aId) const
{
    return mLaneIdMap->GetTextId(aId);
}

const uint64_t& Model::MemoryModel::GetTrafficSignIntId(const std::string& aId)
{
    return mTrafficSignIdMap->GetId(aId);
}

const std::string& Model::MemoryModel::GetTrafficSignTextId(const std::uint64_t& aId) const
{
    return mTrafficSignIdMap->GetTextId(aId);
}

Model::TilePtr Model::MemoryModel::GetTileByLaneId(const std::uint64_t& aLaneId)
{
    for (auto& itor : *mTileMap)
    {
        TilePtr tile = itor.second;
        const LaneMapPtr& laneMap = tile->GetLaneMap();

        if (0 != laneMap->count(aLaneId))
        {
            return tile;
        }
    }

    return nullptr;
}

std::shared_ptr<Model::Lane> Model::MemoryModel::GetLaneById(const std::uint64_t& aLaneId)
{
    for (auto& itor : *mTileMap)
    {
        TilePtr tile = itor.second;
        const LaneMapPtr& laneMap = tile->GetLaneMap();

        if (laneMap->end() != laneMap->find(aLaneId))
        {
            return laneMap->at(aLaneId);
        }
    }

    return nullptr;
}
