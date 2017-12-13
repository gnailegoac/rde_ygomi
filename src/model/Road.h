/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Road.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Common.h"
#include "Element.h"
#include "Lane.h"

class Tile;

class Road : public Element
{
public:
    Road();
    ~Road();

    std::uint64_t GetRoadId() const;
    void SetRoadId(std::uint64_t aRoadId);

    const std::vector<std::shared_ptr<Lane>>& GetLaneList() const;
    std::vector<std::shared_ptr<Lane>>* GetMutableLaneList();
    size_t GetLaneListSize() const;
    std::shared_ptr<Lane> GetLane(size_t aIndex);

    std::shared_ptr<Element> GetPredecessor() const;
    void SetPredecessor(std::shared_ptr<Element> aPredecessor);

    std::shared_ptr<Element> GetSuccessor() const;
    void SetSuccessor(std::shared_ptr<Element> aSuccessor);

    std::shared_ptr<Tile> GetTile() const;
    void SetTile(std::shared_ptr<Tile> aTile);

private:
    std::uint64_t mRoadId;
    std::vector<std::shared_ptr<Lane>> mLaneList;
    std::shared_ptr<Element> mPredecessor;
    std::shared_ptr<Element> mSuccessor;
    std::shared_ptr<Tile> mTile;
};
