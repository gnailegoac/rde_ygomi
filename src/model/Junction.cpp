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
    mJunctionId(0)
{

}

Model::Junction::Junction(std::uint64_t aJunctionId):
    Element(),
    mJunctionId(aJunctionId)
{

}

Model::Junction::~Junction()
{

}

std::uint64_t Model::Junction::GetJunctionId() const
{
    return mJunctionId;
}

void Model::Junction::SetJunctionId(std::uint64_t aJunctionId)
{
    mJunctionId = aJunctionId;
}


const std::unordered_map<uint8_t, std::vector<Model::Point3D>>& Model::Junction::GetPointListMap() const
{
    return mPointListMap;
}

std::unordered_map<uint8_t, std::vector<Model::Point3D>>* Model::Junction::GetMutablePointListMap()
{
    return &mPointListMap;
}

std::vector<Model::Point3D>* Model::Junction::GetPointListByLevel(uint8_t aLevel)
{
    if (0 != mPointListMap.count(aLevel))
    {
        return &(mPointListMap[aLevel]);
    }

    return nullptr;
}

const std::vector<std::shared_ptr<Model::Line>>& Model::Junction::GetBorderLineList() const
{
    return mBorderLineList;
}

std::vector<std::shared_ptr<Model::Line>>* Model::Junction::GetMutableBorderLineList()
{
    return &mBorderLineList;
}

size_t Model::Junction::GetBorderLineListSize() const
{
    return mBorderLineList.size();
}

std::shared_ptr<Model::Line> Model::Junction::GetBorderLine(size_t aIndex)
{
    if (aIndex < mBorderLineList.size())
    {
        return mBorderLineList[aIndex];
    }

    return nullptr;
}

const std::vector<std::shared_ptr<Model::ConnectionRelation>>& Model::Junction::GetConnectionRelationList() const
{
    return mConnectionRelationList;
}

std::vector<std::shared_ptr<Model::ConnectionRelation>>* Model::Junction::GetMutableConnectionRelationList()
{
    return &mConnectionRelationList;
}

size_t Model::Junction::GetConnectionRelationListSize() const
{
    return mConnectionRelationList.size();
}

std::shared_ptr<Model::ConnectionRelation> Model::Junction::GetConnectionRelation(size_t aIndex)
{
    if (aIndex < mConnectionRelationList.size())
    {
        return mConnectionRelationList[aIndex];
    }

    return nullptr;
}
