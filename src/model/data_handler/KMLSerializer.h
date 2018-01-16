/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    KMLSerializer.h
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

class KMLSerializer : public ISerializer
{
public:
    KMLSerializer();
    KMLSerializer(const std::string& aOuputFolder, const double& aInterval);
    ~KMLSerializer();

    bool Serialize(const MemoryModelPtr& aMemoryModel) override;

private:
    std::string mOuputFolder;
    double mInterval;
};

}
