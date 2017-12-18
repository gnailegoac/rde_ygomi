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

#include "Road.h"
#include "Line.h"

Model::Lane::Lane():
    Element(),
    mLaneId(0),
    mLeftLine(nullptr),
    mRightLine(nullptr),
    mCenterLine(nullptr),
    mRoad(nullptr),
    mConnectionMap(nullptr)
{

}

Model::Lane::~Lane()
{

}

const std::uint64_t& Model::Lane::GetLaneId() const
{
    return mLaneId;
}

void Model::Lane::SetLaneId(const uint64_t& aLaneId)
{
    mLaneId = aLaneId;
}

Model::LinePtr Model::Lane::GetLeftLine() const
{
    return mLeftLine;
}

void Model::Lane::SetLeftLine(Model::LinePtr aLeftLine)
{
    mLeftLine = aLeftLine;
}

Model::LinePtr Model::Lane::GetRightLine() const
{
    return mRightLine;
}

void Model::Lane::SetRightLine(Model::LinePtr aRightLine)
{
    mRightLine = aRightLine;
}

Model::LinePtr Model::Lane::GetCenterLine() const
{
    return mCenterLine;
}

void Model::Lane::SetCenterLine(Model::LinePtr aCenterLine)
{
    mCenterLine = aCenterLine;
}

Model::ConnectionMapConstPtr Model::Lane::GetConnectionMap()
{
    return mConnectionMap;
}

Model::ConnectionMapPtr Model::Lane::GetMutableConnectionMap()
{
    return mConnectionMap;
}

std::uint8_t Model::Lane::GetConnectionById(const uint64_t& aId)
{
    if (0 != mConnectionMap->count(aId))
    {
        return mConnectionMap->at(aId);
    }

    return 0;
}

Model::RoadPtr Model::Lane::GetRoad() const
{
    return mRoad;
}

void Model::Lane::SetRoad(Model::RoadPtr aRoad)
{
    mRoad = aRoad;
}
