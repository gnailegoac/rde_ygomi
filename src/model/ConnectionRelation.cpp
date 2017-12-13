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

ConnectionRelation::ConnectionRelation():
    mFromRoadId(0),
    mFromLaneId(0),
    mToRoadId(0),
    mToLaneId(0)
{

}

ConnectionRelation::ConnectionRelation(std::uint64_t aFromRoadId,
                                       std::int8_t aFromLaneId,
                                       std::uint64_t aToRoadId,
                                       std::int8_t aToLaneId):
    mFromRoadId(aFromRoadId),
    mFromLaneId(aFromLaneId),
    mToRoadId(aToRoadId),
    mToLaneId(aToLaneId)
{

}

ConnectionRelation::~ConnectionRelation()
{

}

std::uint64_t ConnectionRelation::GetFromRoadId() const
{
    return mFromRoadId;
}

void ConnectionRelation::SetFromRoadId(std::uint64_t aFromRoadId)
{
    mFromRoadId = aFromRoadId;
}

std::int8_t ConnectionRelation::GetFromLaneId() const
{
    return mFromLaneId;
}

void ConnectionRelation::SetFromLaneId(std::int8_t aFromLaneId)
{
    mFromLaneId = aFromLaneId;
}

std::uint64_t ConnectionRelation::GetToRoadId() const
{
    return mToRoadId;
}

void ConnectionRelation::SetToRoadId(std::uint64_t aToRoadId)
{
    mToRoadId = aToRoadId;
}

std::int8_t ConnectionRelation::GetToLaneId() const
{
    return mToLaneId;
}

void ConnectionRelation::SetToLaneId(std::int8_t aToLaneId)
{
    mToLaneId = aToLaneId;
}
