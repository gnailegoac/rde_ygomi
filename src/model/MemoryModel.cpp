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
    mRoadIdMap(std::make_shared<IdGeneratorMap<Road>>()),
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
        TilePtr tile = std::make_shared<Tile>(aId);
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

const uint64_t& Model::MemoryModel::GetRoadIntId(const std::string& aId)
{
    return mRoadIdMap->GetId(aId);
}

const std::string& Model::MemoryModel::GetRoadTextId(const std::uint64_t& aId) const
{
    return mRoadIdMap->GetTextId(aId);
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

std::shared_ptr<Model::Road> Model::MemoryModel::GetRoadById(const std::uint64_t& aRoadId)
{
    for (auto& itor : *mTileMap)
    {
        TilePtr tile = itor.second;
        const RoadMapPtr& roadMap = tile->GetRoadMap();

        if (roadMap->end() != roadMap->find(aRoadId))
        {
            return roadMap->at(aRoadId);
        }
    }

    return nullptr;
}

std::shared_ptr<Model::Line> Model::MemoryModel::GetLineById(const std::uint64_t& aLineId)
{
    for (auto& itor : *mTileMap)
    {
        TilePtr tile = itor.second;
        const LineMapPtr& lineMap = tile->GetLineMap();

        if (lineMap->end() != lineMap->find(aLineId))
        {
            return lineMap->at(aLineId);
        }
    }

    return nullptr;
}

std::shared_ptr<Model::TrafficSign> Model::MemoryModel::GetTrafficSignById(const uint64_t& aTrafficSignId)
{
    for (const auto& itor : *mTileMap)
    {
        TilePtr tile = itor.second;
        const TrafficSignMapPtr& trafficSignMap = tile->GetTrafficSignMap();

        if (trafficSignMap->count(aTrafficSignId) > 0)
        {
            return trafficSignMap->at(aTrafficSignId);
        }
    }
    return nullptr;
}

void Model::MemoryModel::SaveRoadId(const std::uint64_t& aId)
{
    mRoadIdMap->SetIdAndText(aId, std::to_string(aId));
}

std::uint64_t Model::MemoryModel::GenerateNewTrafficSignId(const std::int64_t& aTileId)
{
    (void)GetMutableTile(aTileId);
    return mTrafficSignIdMap->GetRandomId(static_cast<std::int32_t>(aTileId));
}

std::uint64_t Model::MemoryModel::GenerateNewCurveId(const std::int64_t& aLineId)
{
    std::string lineText = GetLineTextId(aLineId);
    return mCurveIdMap->GenerateNewId(lineText);
}

std::uint64_t Model::MemoryModel::GenerateNewLineId(const std::int64_t& aRoadId)
{
    return mLineIdMap->GenerateNewId(std::to_string(aRoadId));
}

std::uint64_t Model::MemoryModel::GenerateNewLaneId(const int64_t &aRoadId)
{
    return mLaneIdMap->GenerateNewId(std::to_string(aRoadId));
}

std::uint64_t Model::MemoryModel::GenerateNewRoadId(const std::int64_t& aTileId)
{
    (void)GetMutableTile(aTileId);
    return mRoadIdMap->GetRandomId(static_cast<std::int32_t>(aTileId));
}

void Model::MemoryModel::RemoveRoad(const Model::RoadPtr& aRoad)
{
    aRoad->GetTile()->RemoveRoad(aRoad);
}
