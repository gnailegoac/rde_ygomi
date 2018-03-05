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
    Tile(const std::int64_t& aTileId);
    ~Tile();

    const std::int64_t& GetTileId() const;
    void SetTileId(const std::int64_t& aTileId);

    const Point3DPtr& GetReferencePoint() const;
    void SetReferencePoint(const Point3DPtr& aReferencePoint);

    const TrafficSignMapPtr& GetTrafficSignMap() const;
    TrafficSignMapPtr GetMutableTrafficSignMap();
    TrafficSignPtr GetTrafficSign(const std::uint64_t& aId);
    TrafficSignPtr GetMutableTrafficSign(const std::uint64_t& aId);

    const RoadMapPtr& GetRoadMap() const;
    RoadMapPtr GetMutableRoadMap();
    RoadPtr GetRoad(const std::uint64_t& aId);
    RoadPtr GetMutableRoad(const std::uint64_t& aId);

    const LaneMapPtr& GetLaneMap() const;
    LaneMapPtr GetMutableLaneMap();
    LanePtr GetLane(const std::uint64_t& aId);
    LanePtr GetMutableLane(const std::uint64_t& aId);

    const LineMapPtr& GetLineMap() const;
    LineMapPtr GetMutableLineMap();
    LinePtr GetLine(const std::uint64_t& aId);
    LinePtr GetMutableLine(const std::uint64_t& aId);

    const JunctionMapPtr& GetJunctionMap() const;
    JunctionMapPtr GetMutableJunctionMap();
    JunctionPtr GetJunction(const std::uint64_t& aId);
    JunctionPtr GetMutableJunction(const std::uint64_t& aId);

    void RemoveRoad(const Model::RoadPtr& aRoad);

private:
    std::int32_t getBit(std::int32_t aInteger, std::uint8_t aIndex);
    void setBit(std::int32_t& aInteger, std::uint8_t aIndex);
    Point3DPtr tileIdToReferencePoint(const std::int64_t& aTileId);
    std::int32_t referencePointToTileId(const double& aLontitude, const double& aLatitude);

private:
    std::int64_t mTileId;
    Point3DPtr mReferencePoint;
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
