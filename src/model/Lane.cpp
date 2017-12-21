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
    mConnectionMap(std::make_shared<ConnectionMap>())
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

Model::RoadPtr Model::Lane::GetMutableRoad()
{
    return mRoad;
}

void Model::Lane::SetRoad(Model::RoadPtr aRoad)
{
    mRoad = aRoad;
}

const Model::RoadPtr& Model::Lane::GetRoad() const
{
    return mRoad;
}

const Model::ConnectionMapPtr& Model::Lane::GetConnectionMap() const
{
    return mConnectionMap;
}

void Model::Lane::SetPredecessorLaneId(const uint64_t& aId)
{
    (*mConnectionMap)[0] = aId;
}

const std::uint64_t& Model::Lane::GetPredecessorLaneId() const
{
    if (0 == mConnectionMap->count(0))
    {
        (*mConnectionMap)[0] = 0;
    }

    return mConnectionMap->at(0);
}

void Model::Lane::SetSuccessorLaneId(const uint64_t& aId)
{
    (*mConnectionMap)[1] = aId;
}

const std::uint64_t& Model::Lane::GetSuccessorLaneId() const
{
    if (0 == mConnectionMap->count(1))
    {
        (*mConnectionMap)[1] = 0;
    }

    return mConnectionMap->at(1);
}

void Model::Lane::SetLeftLaneId(const uint64_t& aId)
{
    (*mConnectionMap)[2] = aId;
}

const std::uint64_t& Model::Lane::GetLeftLaneId() const
{
    if (0 == mConnectionMap->count(2))
    {
        (*mConnectionMap)[2] = 0;
    }

    return mConnectionMap->at(2);
}

void Model::Lane::SetRightLaneId(const uint64_t& aId)
{
    (*mConnectionMap)[3] = aId;
}

const std::uint64_t& Model::Lane::GetRightLaneId() const
{
    if (0 == mConnectionMap->count(3))
    {
        (*mConnectionMap)[3] = 0;
    }

    return mConnectionMap->at(3);
}
