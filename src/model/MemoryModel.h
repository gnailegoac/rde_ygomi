/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    MemoryModel.h
 *******************************************************************************
 */

#pragma once

#include "Common.h"
#include "Tile.h"

namespace Model
{

template <class T>
class IdGeneratorMap;

typedef std::unordered_map<std::int64_t, std::shared_ptr<Tile>> TileMap;
typedef std::shared_ptr<TileMap> TileMapPtr;
typedef std::shared_ptr<const TileMap> TileMapConstPtr;

typedef std::shared_ptr<IdGeneratorMap<Curve>> CurveIdMapPtr;
typedef std::shared_ptr<IdGeneratorMap<Line>> LineIdMapPtr;
typedef std::shared_ptr<IdGeneratorMap<Lane>> LaneIdMapPtr;
typedef std::shared_ptr<IdGeneratorMap<TrafficSign>> TrafficSignIdMapPtr;

class MemoryModel
{
public:
    MemoryModel();
    ~MemoryModel();

    const TileMapPtr& GetTileMap() const;
    TileMapPtr GetMutableTileMap();
    TileConstPtr GetTile(const std::int64_t& aId);
    TilePtr GetMutableTile(const std::int64_t& aId);

    uint64_t GetCurveIntId(const std::string& aId);
    uint64_t GetLineIntId(const std::string& aId);
    uint64_t GetLaneIntId(const std::string& aId);
    uint64_t GetTrafficSignIntId(const std::string& aId);

    TilePtr GetTileByLaneId(const std::uint64_t& aLaneId);

private:
    TileMapPtr mTileMap;
    CurveIdMapPtr mCurveIdMap;
    LineIdMapPtr mLineIdMap;
    LaneIdMapPtr mLaneIdMap;
    TrafficSignIdMapPtr mTrafficSignIdMap;
};

}

