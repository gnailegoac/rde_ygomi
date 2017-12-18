/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Lane.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Common.h"
#include "Element.h"

namespace Model
{

class Road;
class Line;

typedef std::shared_ptr<Road> RoadPtr;
typedef std::shared_ptr<Line> LinePtr;

typedef std::shared_ptr<std::unordered_map<std::uint64_t, std::uint8_t>> ConnectionMapPtr;
typedef std::shared_ptr<const std::unordered_map<std::uint64_t, std::uint8_t>> ConnectionMapConstPtr;

class Lane : public Element
{
public:
    Lane();
    ~Lane();

    std::uint64_t GetLaneId() const;
    void SetLaneId(std::uint64_t aLaneId);

    LinePtr GetLeftLine() const;
    void SetLeftLine(LinePtr aLeftLine);

    LinePtr GetRightLine() const;
    void SetRightLine(LinePtr aRightLine);

    LinePtr GetCenterLine() const;
    void SetCenterLine(LinePtr aCenterLine);

    RoadPtr GetRoad() const;
    void SetRoad(RoadPtr aRoad);

    ConnectionMapConstPtr GetConnectionMap();
    ConnectionMapPtr GetMutableConnectionMap();
    std::uint8_t GetConnectionById(uint64_t aId);

private:
    std::uint64_t mLaneId;
    LinePtr mLeftLine;
    LinePtr mRightLine;
    LinePtr mCenterLine;
    RoadPtr mRoad;
    ConnectionMapPtr mConnectionMap;
};

typedef std::shared_ptr<Lane> LanePtr;
typedef std::shared_ptr<const Lane> LaneConstPtr;
typedef std::vector<LanePtr> LaneList;
typedef std::shared_ptr<LaneList> LaneListPtr;
typedef std::shared_ptr<const LaneList> LaneListConstPtr;

typedef std::unordered_map<std::uint64_t, LanePtr> LaneMap;
typedef std::shared_ptr<LaneMap> LaneMapPtr;
typedef std::shared_ptr<const LaneMap> LaneMapConstPtr;

}
