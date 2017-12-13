/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    ConnectionRelation.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Common.h"

namespace Model
{

class ConnectionRelation
{
public:
    ConnectionRelation();
    ConnectionRelation(std::uint64_t aFromRoadId,
                       std::int8_t aFromLaneId,
                       std::uint64_t aToRoadId,
                       std::int8_t aToLaneId);
    ~ConnectionRelation();

    std::uint64_t GetFromRoadId() const;
    void SetFromRoadId(std::uint64_t aFromRoadId);

    std::int8_t GetFromLaneId() const;
    void SetFromLaneId(std::int8_t aFromLaneId);

    std::uint64_t GetToRoadId() const;
    void SetToRoadId(std::uint64_t aToRoadId);

    std::int8_t GetToLaneId() const;
    void SetToLaneId(std::int8_t aToLaneId);

private:
    std::uint64_t mFromRoadId;
    std::int8_t mFromLaneId;
    std::uint64_t mToRoadId;
    std::int8_t mToLaneId;
};

}
