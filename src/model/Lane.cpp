/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Lane.cpp
 * @brief
 *******************************************************************************
 */

#include "Lane.h"

Lane::Lane():
    Element(),
    mLaneId(0),
    mLeftLine(nullptr),
    mRightLine(nullptr),
    mCenterLine(nullptr),
    mConnectionMap(),
    mRoad(nullptr)
{

}

Lane::~Lane()
{

}

std::uint64_t Lane::GetLaneId() const
{
    return mLaneId;
}

void Lane::SetLaneId(std::uint64_t aLaneId)
{
    mLaneId = aLaneId;
}

std::shared_ptr<Line> Lane::GetLeftLine() const
{
    return mLeftLine;
}

void Lane::SetLeftLine(std::shared_ptr<Line> aLeftLine)
{
    mLeftLine = aLeftLine;
}

std::shared_ptr<Line> Lane::GetRightLine() const
{
    return mRightLine;
}

void Lane::SetRightLine(std::shared_ptr<Line> aRightLine)
{
    mRightLine = aRightLine;
}

std::shared_ptr<Line> Lane::GetCenterLine() const
{
    return mCenterLine;
}

void Lane::SetCenterLine(std::shared_ptr<Line> aCenterLine)
{
    mCenterLine = aCenterLine;
}

const std::unordered_map<std::uint64_t, std::uint64_t>& Lane::GetConnectionMap()
{
    return mConnectionMap;
}

std::unordered_map<std::uint64_t, std::uint64_t>* Lane::GetMutableConnectionMap()
{
    return &mConnectionMap;
}

std::uint64_t Lane::GetConnectionById(uint64_t aId)
{
    if (0 != mConnectionMap.count(aId))
    {
        return mConnectionMap[aId];
    }

    return 0;
}

std::shared_ptr<Road> Lane::GetRoad() const
{
    return mRoad;
}

void Lane::SetRoad(std::shared_ptr<Road> aRoad)
{
    mRoad = aRoad;
}
