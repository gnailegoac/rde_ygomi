/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    ConnectionRelation.cpp
 * @brief
 *******************************************************************************
 */

#include "ConnectionRelation.h"

Model::ConnectionRelation::ConnectionRelation():
    mFromRoadId(0),
    mFromLaneId(0),
    mToRoadId(0),
    mToLaneId(0)
{

}

Model::ConnectionRelation::ConnectionRelation(const std::uint64_t& aFromRoadId,
                                              const std::uint64_t& aFromLaneId,
                                              const std::uint64_t& aToRoadId,
                                              const std::uint64_t& aToLaneId):
    mFromRoadId(aFromRoadId),
    mFromLaneId(aFromLaneId),
    mToRoadId(aToRoadId),
    mToLaneId(aToLaneId)
{

}

Model::ConnectionRelation::~ConnectionRelation()
{

}

const uint64_t& Model::ConnectionRelation::GetFromRoadId() const
{
    return mFromRoadId;
}

void Model::ConnectionRelation::SetFromRoadId(std::uint64_t aFromRoadId)
{
    mFromRoadId = aFromRoadId;
}

const std::uint64_t& Model::ConnectionRelation::GetFromLaneId() const
{
    return mFromLaneId;
}

void Model::ConnectionRelation::SetFromLaneId(std::uint64_t aFromLaneId)
{
    mFromLaneId = aFromLaneId;
}

const uint64_t& Model::ConnectionRelation::GetToRoadId() const
{
    return mToRoadId;
}

void Model::ConnectionRelation::SetToRoadId(std::uint64_t aToRoadId)
{
    mToRoadId = aToRoadId;
}

const uint64_t& Model::ConnectionRelation::GetToLaneId() const
{
    return mToLaneId;
}

void Model::ConnectionRelation::SetToLaneId(std::uint64_t aToLaneId)
{
    mToLaneId = aToLaneId;
}
