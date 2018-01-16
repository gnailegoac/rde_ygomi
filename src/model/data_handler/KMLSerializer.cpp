/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    KMLSerializer.cpp
 * @brief
 *******************************************************************************
 */

#include "KMLSerializer.h"

#include <QDebug>

#include "../MemoryModel.h"
#include "KMLInterpreter.h"

Model::KMLSerializer::KMLSerializer():
    mOuputFolder("."),
    mInterval(0.1)
{

}

Model::KMLSerializer::KMLSerializer(const std::string& aOuputFolder,
                                    const double& aInterval):
    mOuputFolder(aOuputFolder),
    mInterval(aInterval)
{

}

Model::KMLSerializer::~KMLSerializer()
{

}

bool Model::KMLSerializer::Serialize(const Model::MemoryModelPtr& aMemoryModel)
{
    KMLInterpreterPtr kmlInterpreter = std::make_shared<KMLInterpreter>(mOuputFolder, 0.1);
    const TileMapPtr& tiles = aMemoryModel->GetTileMap();

    // create directory trees
    if (!kmlInterpreter->TouchKMLDirectories())
    {
        return false;
    }

    for (const auto& iterTile : * tiles)
    {
        const TilePtr tile = iterTile.second;

        // serialize all kinds of elements to each directory
        if (!kmlInterpreter->SaveKMLPaint(tile)
            || !kmlInterpreter->SaveKMLLaneBoundary(tile)
            || !kmlInterpreter->SaveKMLTrafficSign(tile))
        {
            return false;
        }
    }

    return true;
}
