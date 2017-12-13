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

Model::Lane::Lane():
    Element(),
    mLaneId(0),
    mLeftLine(nullptr),
    mRightLine(nullptr),
    mCenterLine(nullptr),
    mConnectionMap(),
    mRoad(nullptr)
{

}

Model::Lane::~Lane()
{

}

std::uint64_t Model::Lane::GetLaneId() const
{
    return mLaneId;
}

void Model::Lane::SetLaneId(std::uint64_t aLaneId)
{
    mLaneId = aLaneId;
}

std::shared_ptr<Model::Line> Model::Lane::GetLeftLine() const
{
    return mLeftLine;
}

void Model::Lane::SetLeftLine(std::shared_ptr<Model::Line> aLeftLine)
{
    mLeftLine = aLeftLine;
}

std::shared_ptr<Model::Line> Model::Lane::GetRightLine() const
{
    return mRightLine;
}

void Model::Lane::SetRightLine(std::shared_ptr<Model::Line> aRightLine)
{
    mRightLine = aRightLine;
}

std::shared_ptr<Model::Line> Model::Lane::GetCenterLine() const
{
    return mCenterLine;
}

void Model::Lane::SetCenterLine(std::shared_ptr<Model::Line> aCenterLine)
{
    mCenterLine = aCenterLine;
}

const std::unordered_map<std::uint64_t, std::uint64_t>& Model::Lane::GetConnectionMap()
{
    return mConnectionMap;
}

std::unordered_map<std::uint64_t, std::uint64_t>* Model::Lane::GetMutableConnectionMap()
{
    return &mConnectionMap;
}

std::uint64_t Model::Lane::GetConnectionById(uint64_t aId)
{
    if (0 != mConnectionMap.count(aId))
    {
        return mConnectionMap[aId];
    }

    return 0;
}

std::shared_ptr<Model::Road> Model::Lane::GetRoad() const
{
    return mRoad;
}

void Model::Lane::SetRoad(std::shared_ptr<Model::Road> aRoad)
{
    mRoad = aRoad;
}
