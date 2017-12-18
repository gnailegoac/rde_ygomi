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
                       std::uint64_t aFromLaneId,
                       std::uint64_t aToRoadId,
                       std::uint64_t aToLaneId);
    ~ConnectionRelation();

    std::uint64_t GetFromRoadId() const;
    void SetFromRoadId(std::uint64_t aFromRoadId);

    std::uint64_t GetFromLaneId() const;
    void SetFromLaneId(std::uint64_t aFromLaneId);

    std::uint64_t GetToRoadId() const;
    void SetToRoadId(std::uint64_t aToRoadId);

    std::uint64_t GetToLaneId() const;
    void SetToLaneId(std::uint64_t aToLaneId);

private:
    std::uint64_t mFromRoadId;
    std::uint64_t mFromLaneId;
    std::uint64_t mToRoadId;
    std::uint64_t mToLaneId;
};

typedef std::shared_ptr<ConnectionRelation> ConnectionRelationPtr;
typedef std::shared_ptr<const ConnectionRelation> ConnectionRelationConstPtr;
typedef std::vector<ConnectionRelationPtr> ConnectionRelationList;
typedef std::shared_ptr<ConnectionRelationList> ConnectionRelationListPtr;
typedef std::shared_ptr<const ConnectionRelationList> ConnectionRelationListConstPtr;

}
