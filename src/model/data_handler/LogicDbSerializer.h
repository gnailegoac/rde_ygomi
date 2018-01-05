/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    LogicDbSerializer.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "../Common.h"
#include "ISerializer.h"

namespace Model
{

class Tile;
typedef std::shared_ptr<Tile> TilePtr;


class LogicDbSerializer : public ISerializer
{
public:
    LogicDbSerializer(const std::string& aOuputFolder, const std::string aVersion = "0");
    LogicDbSerializer();
    ~LogicDbSerializer();

    bool Serialize(const MemoryModelPtr& aMemoryModel) override;

private:
    std::string createDbPath(const TilePtr& aTile) const;

private:
    std::string mOuputFolder;
    std::string mVersion;
};

}
