/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Junction.cpp
 * @brief
 *******************************************************************************
 */

#include "Junction.h"

Junction::Junction():
    Element(),
    mJunctionId(0)
{

}

Junction::Junction(std::uint64_t aJunctionId):
    Element(),
    mJunctionId(aJunctionId)
{

}

Junction::~Junction()
{

}

std::uint64_t Junction::GetJunctionId() const
{
    return mJunctionId;
}

void Junction::SetJunctionId(std::uint64_t aJunctionId)
{
    mJunctionId = aJunctionId;
}


const std::unordered_map<uint8_t, std::vector<Point3D>>& Junction::GetPointListMap() const
{
    return mPointListMap;
}

std::unordered_map<uint8_t, std::vector<Point3D>>* Junction::GetMutablePointListMap()
{
    return &mPointListMap;
}

std::vector<Point3D>* Junction::GetPointListByLevel(uint8_t aLevel)
{
    if (0 != mPointListMap.count(aLevel))
    {
        return &(mPointListMap[aLevel]);
    }

    return nullptr;
}

const std::vector<std::shared_ptr<Line>>& Junction::GetBorderLineList() const
{
    return mBorderLineList;
}

std::vector<std::shared_ptr<Line>>* Junction::GetMutableBorderLineList()
{
    return &mBorderLineList;
}

size_t Junction::GetBorderLineListSize() const
{
    return mBorderLineList.size();
}

std::shared_ptr<Line> Junction::GetBorderLine(size_t aIndex)
{
    if (aIndex < mBorderLineList.size())
    {
        return mBorderLineList[aIndex];
    }

    return nullptr;
}

const std::vector<std::shared_ptr<ConnectionRelation>>& Junction::GetConnectionRelationList() const
{
    return mConnectionRelationList;
}

std::vector<std::shared_ptr<ConnectionRelation>>* Junction::GetMutableConnectionRelationList()
{
    return &mConnectionRelationList;
}

size_t Junction::GetConnectionRelationListSize() const
{
    return mConnectionRelationList.size();
}

std::shared_ptr<ConnectionRelation> Junction::GetConnectionRelation(size_t aIndex)
{
    if (aIndex < mConnectionRelationList.size())
    {
        return mConnectionRelationList[aIndex];
    }

    return nullptr;
}
