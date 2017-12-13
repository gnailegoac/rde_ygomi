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

class MemoryModel
{
public:
    MemoryModel();
    ~MemoryModel();

    const std::unordered_map<std::int64_t, std::shared_ptr<Tile>>& GetTileMap() const;
    std::unordered_map<std::int64_t, std::shared_ptr<Tile>>* GetMutableTileMap();
    std::shared_ptr<Tile> GetTile(std::int64_t aId);

private:
    std::unordered_map<std::int64_t, std::shared_ptr<Tile>> mTileMap;
};
