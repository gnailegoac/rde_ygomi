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

Road::Road():
    Element(),
    mRoadId(0),
    mLaneList(),
    mPredecessor(nullptr),
    mSuccessor(nullptr),
    mTile(nullptr)
{

}

Road::~Road()
{

}

std::uint64_t Road::GetRoadId() const
{
    return mRoadId;
}

void Road::SetRoadId(std::uint64_t aRoadId)
{
    mRoadId = aRoadId;
}

const std::vector<std::shared_ptr<Lane>>& Road::GetLaneList() const
{
    return mLaneList;
}

std::vector<std::shared_ptr<Lane>>* Road::GetMutableLaneList()
{
    return &mLaneList;
}

size_t Road::GetLaneListSize() const
{
    return mLaneList.size();
}

std::shared_ptr<Lane> Road::GetLane(size_t aIndex)
{
    if (aIndex < mLaneList.size())
    {
        return mLaneList[aIndex];
    }

    return nullptr;
}

std::shared_ptr<Element> Road::GetPredecessor() const
{
    return mPredecessor;
}

void Road::SetPredecessor(std::shared_ptr<Element> aPredecessor)
{
    mPredecessor = aPredecessor;
}

std::shared_ptr<Element> Road::GetSuccessor() const
{
    return mSuccessor;
}

void Road::SetSuccessor(std::shared_ptr<Element> aSuccessor)
{
    mSuccessor = aSuccessor;
}

std::shared_ptr<Tile> Road::GetTile() const
{
    return mTile;
}

void Road::SetTile(std::shared_ptr<Tile> aTile)
{
    mTile = aTile;
}
