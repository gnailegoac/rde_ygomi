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

typedef std::shared_ptr<std::unordered_map<std::int64_t, std::shared_ptr<Tile>>> TileMapPtr;
typedef std::shared_ptr<const std::unordered_map<std::int64_t, std::shared_ptr<Tile>>> TileMapConstPtr;

class MemoryModel
{
public:
    MemoryModel();
    ~MemoryModel();

    TileMapConstPtr GetTileMap() const;
    TileMapPtr GetMutableTileMap();
    TilePtr GetTile(std::int64_t aId);

private:
    TileMapPtr mTileMap;
};

}

