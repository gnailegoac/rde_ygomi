/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    MemoryModel.h
 *******************************************************************************
 */

#pragma once

#include "Common.h"
#include "Tile.h"

namespace Model
{

typedef std::unordered_map<std::int64_t, std::shared_ptr<Tile>> TileMap;
typedef std::shared_ptr<TileMap> TileMapPtr;
typedef std::shared_ptr<const TileMap> TileMapConstPtr;

class MemoryModel
{
public:
    MemoryModel();
    ~MemoryModel();

    const TileMapPtr& GetTileMap() const;
    TileMapPtr GetMutableTileMap();
    TilePtr GetTile(const std::int64_t& aId);

private:
    TileMapPtr mTileMap;
};

}

