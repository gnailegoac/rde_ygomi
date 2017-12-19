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

Model::Junction::Junction():
    Element(),
    mJunctionId(0),
    mPointListMap(std::make_shared<ViewPointMap>()),
    mBorderLineList(std::make_shared<LineList>()),
    mConnectionRelationList(std::make_shared<ConnectionRelationList>())
{

}

Model::Junction::Junction(const std::uint64_t& aJunctionId):
    Element(),
    mJunctionId(aJunctionId),
    mPointListMap(std::make_shared<ViewPointMap>()),
    mBorderLineList(std::make_shared<LineList>()),
    mConnectionRelationList(std::make_shared<ConnectionRelationList>())
{

}

Model::Junction::~Junction()
{

}

const uint64_t& Model::Junction::GetJunctionId() const
{
    return mJunctionId;
}

void Model::Junction::SetJunctionId(const std::uint64_t& aJunctionId)
{
    mJunctionId = aJunctionId;
}

const Model::ViewPointMapPtr& Model::Junction::GetPointListMap() const
{
    return mPointListMap;
}

Model::ViewPointMapPtr Model::Junction::GetMutablePointListMap()
{
    return mPointListMap;
}

Model::Point3DListPtr Model::Junction::GetPointListByLevel(uint8_t aLevel)
{
    if (0 != mPointListMap->count(aLevel))
    {
        return mPointListMap->at(aLevel);
    }

    return nullptr;
}

const Model::LineListPtr& Model::Junction::GetBorderLineList() const
{
    return mBorderLineList;
}

Model::LineListPtr Model::Junction::GetMutableBorderLineList()
{
    return mBorderLineList;
}

size_t Model::Junction::GetBorderLineListSize() const
{
    return mBorderLineList->size();
}

Model::LinePtr Model::Junction::GetBorderLine(const size_t& aIndex)
{
    if (aIndex < mBorderLineList->size())
    {
        return mBorderLineList->at(aIndex);
    }

    return nullptr;
}

const Model::ConnectionRelationListPtr& Model::Junction::GetConnectionRelationList() const
{
    return mConnectionRelationList;
}

Model::ConnectionRelationListPtr Model::Junction::GetMutableConnectionRelationList()
{
    return mConnectionRelationList;
}

size_t Model::Junction::GetConnectionRelationListSize() const
{
    return mConnectionRelationList->size();
}

Model::ConnectionRelationPtr Model::Junction::GetConnectionRelation(const size_t& aIndex)
{
    if (aIndex < mConnectionRelationList->size())
    {
        return mConnectionRelationList->at(aIndex);
    }

    return nullptr;
}
