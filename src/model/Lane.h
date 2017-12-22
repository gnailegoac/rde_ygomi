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

typedef std::unordered_map<std::uint8_t, std::uint64_t> ConnectionMap;
typedef std::shared_ptr<ConnectionMap> ConnectionMapPtr;
typedef std::shared_ptr<const ConnectionMap> ConnectionMapConstPtr;

class Lane : public Element
{
public:
    Lane();
    ~Lane();

    const std::uint64_t& GetLaneId() const;
    void SetLaneId(const std::uint64_t& aLaneId);

    LinePtr GetLeftLine() const;
    void SetLeftLine(LinePtr aLeftLine);

    LinePtr GetRightLine() const;
    void SetRightLine(LinePtr aRightLine);

    LinePtr GetCenterLine() const;
    void SetCenterLine(LinePtr aCenterLine);

    const RoadPtr& GetRoad() const;
    RoadPtr GetMutableRoad();
    void SetRoad(RoadPtr aRoad);

    const ConnectionMapPtr& GetConnectionMap() const;

    void SetPredecessorLaneId(const uint64_t& aId);
    const std::uint64_t& GetPredecessorLaneId() const;

    void SetSuccessorLaneId(const uint64_t& aId);
    const std::uint64_t& GetSuccessorLaneId() const;

    void SetLeftLaneId(const uint64_t& aId);
    const std::uint64_t& GetLeftLaneId() const;

    void SetRightLaneId(const uint64_t& aId);
    const std::uint64_t& GetRightLaneId() const;

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
