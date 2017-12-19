/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Road.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Common.h"
#include "Element.h"
#include "Lane.h"

namespace Model
{

class Tile;

typedef std::shared_ptr<Tile> TilePtr;

class Road : public Element
{
public:
    Road();
    ~Road();

    const std::uint64_t& GetRoadId() const;
    void SetRoadId(const uint64_t& aRoadId);

    const LaneListPtr& GetLaneList() const;
    LaneListPtr GetMutableLaneList();
    size_t GetLaneListSize() const;
    LanePtr GetLane(const size_t& aIndex);

    const ElementPtr& GetPredecessor() const;
    void SetPredecessor(const ElementPtr& aPredecessor);

    const ElementPtr& GetSuccessor() const;
    void SetSuccessor(const ElementPtr& aSuccessor);

    const TilePtr& GetTile() const;
    void SetTile(const TilePtr& aTile);

private:
    std::uint64_t mRoadId;
    LaneListPtr mLaneList;
    ElementPtr mPredecessor;
    ElementPtr mSuccessor;
    TilePtr mTile;
};

typedef std::shared_ptr<Road> RoadPtr;
typedef std::shared_ptr<const Road> RoadConstPtr;
typedef std::vector<RoadPtr> RoadList;
typedef std::shared_ptr<RoadList> RoadListPtr;
typedef std::shared_ptr<const RoadList> RoadListConstPtr;

typedef std::unordered_map<std::uint64_t, RoadPtr> RoadMap;
typedef std::shared_ptr<RoadMap> RoadMapPtr;
typedef std::shared_ptr<const RoadMap> RoadMapConstPtr;

}
