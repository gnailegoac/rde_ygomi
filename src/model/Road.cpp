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
    mLaneList(),
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

const std::vector<std::shared_ptr<Model::Lane>>& Model::Road::GetLaneList() const
{
    return mLaneList;
}

std::vector<std::shared_ptr<Model::Lane>>* Model::Road::GetMutableLaneList()
{
    return &mLaneList;
}

size_t Model::Road::GetLaneListSize() const
{
    return mLaneList.size();
}

std::shared_ptr<Model::Lane> Model::Road::GetLane(size_t aIndex)
{
    if (aIndex < mLaneList.size())
    {
        return mLaneList[aIndex];
    }

    return nullptr;
}

std::shared_ptr<Model::Element> Model::Road::GetPredecessor() const
{
    return mPredecessor;
}

void Model::Road::SetPredecessor(std::shared_ptr<Model::Element> aPredecessor)
{
    mPredecessor = aPredecessor;
}

std::shared_ptr<Model::Element> Model::Road::GetSuccessor() const
{
    return mSuccessor;
}

void Model::Road::SetSuccessor(std::shared_ptr<Model::Element> aSuccessor)
{
    mSuccessor = aSuccessor;
}

std::shared_ptr<Model::Tile> Model::Road::GetTile() const
{
    return mTile;
}

void Model::Road::SetTile(std::shared_ptr<Model::Tile> aTile)
{
    mTile = aTile;
}
