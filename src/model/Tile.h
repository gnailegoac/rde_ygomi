/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Tile.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Common.h"
#include "TrafficSign.h"
#include "Road.h"
#include "Lane.h"
#include "Line.h"
#include "Junction.h"

namespace Model
{

class Tile
{
public:
    Tile();
    ~Tile();

    const std::int64_t& GetTileId() const;
    void SetTileId(const std::int64_t& aTileId);

    TrafficSignMapConstPtr GetTrafficSignMap() const;
    TrafficSignMapPtr GetMutableTrafficSignMap();
    TrafficSignPtr GetTrafficSign(const std::uint64_t& aId);

    RoadMapConstPtr GetRoadMap() const;
    RoadMapPtr GetMutableRoadMap();
    RoadPtr GetRoad(const std::uint64_t& aId);

    LaneMapConstPtr GetLaneMap() const;
    LaneMapPtr GetMutableLaneMap();
    LanePtr GetLane(const std::uint64_t& aId);

    LineMapConstPtr GetLineMap() const;
    LineMapPtr GetMutableLineMap();
    LinePtr GetLine(const std::uint64_t& aId);

    JunctionMapConstPtr GetJunctionMap() const;
    JunctionMapPtr GetMutableJunctionMap();
    JunctionPtr GetJunction(const std::uint64_t& aId);

private:
    std::int64_t mTileId;
    TrafficSignMapPtr mTrafficSignMap;
    RoadMapPtr mRoadMap;
    LaneMapPtr mLaneMap;
    LineMapPtr mLineMap;
    JunctionMapPtr mJunctionMap;
};

typedef std::shared_ptr<Tile> TilePtr;
typedef std::shared_ptr<const Tile> TileConstPtr;
typedef std::vector<TilePtr> TileList;
typedef std::shared_ptr<TileList> TileListPtr;
typedef std::shared_ptr<const TileList> TileListConstPtr;

}
