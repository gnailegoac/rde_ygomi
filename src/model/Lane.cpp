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

Model::LinePtr Model::Lane::GetAvgSlamTrace() const
{
    return mAvgSlamTrace;
}

void Model::Lane::SetAvgSlamTrace(Model::LinePtr aAvgSlamTrace)
{
    mAvgSlamTrace = aAvgSlamTrace;
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
    std::uint8_t key = (std::uint8_t)LaneConnectionType::LaneConnectionPredecessor;
    (*mConnectionMap)[key] = aId;
}

const std::uint64_t& Model::Lane::GetPredecessorLaneId() const
{
    std::uint8_t key = (std::uint8_t)LaneConnectionType::LaneConnectionPredecessor;

    if (0 == mConnectionMap->count(key))
    {
        (*mConnectionMap)[key] = 0;
    }

    return mConnectionMap->at(key);
}

void Model::Lane::SetSuccessorLaneId(const uint64_t& aId)
{
    std::uint8_t key = (std::uint8_t)LaneConnectionType::LaneConnectionSuccessor;
    (*mConnectionMap)[key] = aId;
}

const std::uint64_t& Model::Lane::GetSuccessorLaneId() const
{
    std::uint8_t key = (std::uint8_t)LaneConnectionType::LaneConnectionSuccessor;

    if (0 == mConnectionMap->count(key))
    {
        (*mConnectionMap)[key] = 0;
    }

    return mConnectionMap->at(key);
}

void Model::Lane::SetLeftLaneId(const uint64_t& aId)
{
    std::uint8_t key = (std::uint8_t)LaneConnectionType::LaneConnectionLeft;
    (*mConnectionMap)[key] = aId;
}

const std::uint64_t& Model::Lane::GetLeftLaneId() const
{
    std::uint8_t key = (std::uint8_t)LaneConnectionType::LaneConnectionLeft;

    if (0 == mConnectionMap->count(key))
    {
        (*mConnectionMap)[key] = 0;
    }

    return mConnectionMap->at(key);
}

void Model::Lane::SetRightLaneId(const uint64_t& aId)
{
    std::uint8_t key = (std::uint8_t)LaneConnectionType::LaneConnectionRight;
    (*mConnectionMap)[key] = aId;
}

const std::uint64_t& Model::Lane::GetRightLaneId() const
{
    std::uint8_t key = (std::uint8_t)LaneConnectionType::LaneConnectionRight;

    if (0 == mConnectionMap->count(key))
    {
        (*mConnectionMap)[key] = 0;
    }

    return mConnectionMap->at(key);
}
