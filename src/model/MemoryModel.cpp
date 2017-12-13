/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    MemoryModel.cpp
 *******************************************************************************
 */

#include "MemoryModel.h"

MemoryModel::MemoryModel():
    mTileMap()
{

}

MemoryModel::~MemoryModel()
{

}

const std::unordered_map<std::int64_t, std::shared_ptr<Tile>>& MemoryModel::GetTileMap() const
{
    return mTileMap;
}

std::unordered_map<std::int64_t, std::shared_ptr<Tile>>* MemoryModel::GetMutableTileMap()
{
    return &mTileMap;
}

std::shared_ptr<Tile> MemoryModel::GetTile(std::int64_t aId)
{
    if (0 != mTileMap.count(aId))
    {
        return mTileMap[aId];
    }

    return nullptr;
}
