/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Road.cpp
 * @brief
 *******************************************************************************
 */

#include "Road.h"

Model::Road::Road():
    Element(),
    mRoadId(0),
    mLaneList(std::make_shared<LaneList>()),
    mPredecessor(nullptr),
    mSuccessor(nullptr),
    mTile(nullptr)
{

}

Model::Road::~Road()
{

}

std::uint64_t Model::Road::GetRoadId() const
{
    return mRoadId;
}

void Model::Road::SetRoadId(std::uint64_t aRoadId)
{
    mRoadId = aRoadId;
}

Model::LaneListConstPtr Model::Road::GetLaneList() const
{
    return mLaneList;
}

Model::LaneListPtr Model::Road::GetMutableLaneList()
{
    return mLaneList;
}

size_t Model::Road::GetLaneListSize() const
{
    return mLaneList->size();
}

Model::LanePtr Model::Road::GetLane(size_t aIndex)
{
    if (aIndex < mLaneList->size())
    {
        return mLaneList->at(aIndex);
    }

    return nullptr;
}

Model::ElementPtr Model::Road::GetPredecessor() const
{
    return mPredecessor;
}

void Model::Road::SetPredecessor(Model::ElementPtr aPredecessor)
{
    mPredecessor = aPredecessor;
}

Model::ElementPtr Model::Road::GetSuccessor() const
{
    return mSuccessor;
}

void Model::Road::SetSuccessor(Model::ElementPtr aSuccessor)
{
    mSuccessor = aSuccessor;
}

Model::TilePtr Model::Road::GetTile() const
{
    return mTile;
}

void Model::Road::SetTile(Model::TilePtr aTile)
{
    mTile = aTile;
}
