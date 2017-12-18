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

#include "Tile.h"

Model::MemoryModel::MemoryModel():
    mTileMap(std::make_shared<std::unordered_map<std::int64_t, std::shared_ptr<Tile>>>())
{

}

Model::MemoryModel::~MemoryModel()
{

}

Model::TileMapConstPtr Model::MemoryModel::GetTileMap() const
{
    return mTileMap;
}

Model::TileMapPtr Model::MemoryModel::GetMutableTileMap()
{
    return mTileMap;
}

Model::TilePtr Model::MemoryModel::GetTile(std::int64_t aId)
{
    if (0 != mTileMap->count(aId))
    {
        return mTileMap->at(aId);
    }

    return nullptr;
}
