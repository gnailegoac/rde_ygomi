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

class Tile
{
public:
    Tile();
    ~Tile();

    std::int64_t GetTileId() const;
    void SetTileId(std::int64_t aTileId);

    const std::unordered_map<std::uint64_t, std::shared_ptr<TrafficSign>>& GetTrafficSignMap() const;
    std::unordered_map<std::uint64_t, std::shared_ptr<TrafficSign>>* GetMutableTrafficSignMap();
    std::shared_ptr<TrafficSign> GetTrafficSign(std::uint64_t aId);

    const std::unordered_map<std::uint64_t, std::shared_ptr<Road>>& GetRoadMap() const;
    std::unordered_map<std::uint64_t, std::shared_ptr<Road>>* GetMutableRoadMap();
    std::shared_ptr<Road> GetRoad(std::uint64_t aId);

    const std::unordered_map<std::uint64_t, std::shared_ptr<Lane>>& GetLaneMap() const;
    std::unordered_map<std::uint64_t, std::shared_ptr<Lane>>* GetMutableLaneMap();
    std::shared_ptr<Lane> GetLane(std::uint64_t aId);

    const std::unordered_map<std::uint64_t, std::shared_ptr<Line>>& GetLineMap() const;
    std::unordered_map<std::uint64_t, std::shared_ptr<Line>>* GetMutableLineMap();
    std::shared_ptr<Line> GetLine(std::uint64_t aId);

    const std::unordered_map<std::uint64_t, std::shared_ptr<Junction>>& GetJunctionMap() const;
    std::unordered_map<std::uint64_t, std::shared_ptr<Junction>>* GetMutableJunctionMap();
    std::shared_ptr<Junction> GetJunction(std::uint64_t aId);

private:
    std::int64_t mTileId;
    std::unordered_map<std::uint64_t, std::shared_ptr<TrafficSign>> mTrafficSignMap;
    std::unordered_map<std::uint64_t, std::shared_ptr<Road>> mRoadMap;
    std::unordered_map<std::uint64_t, std::shared_ptr<Lane>> mLaneMap;
    std::unordered_map<std::uint64_t, std::shared_ptr<Line>> mLineMap;
    std::unordered_map<std::uint64_t, std::shared_ptr<Junction>> mJunctionMap;
};
