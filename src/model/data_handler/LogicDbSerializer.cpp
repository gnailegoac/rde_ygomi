/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    LogicDbSerializer.cpp
 * @brief
 *******************************************************************************
 */

#include "LogicDbSerializer.h"

#include "../MemoryModel.h"
#include "DbRepository.h"

Model::LogicDbSerializer::LogicDbSerializer(const std::string& aOuputFolder,
                                            const std::string aVersion):
    mOuputFolder(aOuputFolder),
    mVersion(aVersion)
{

}

Model::LogicDbSerializer::LogicDbSerializer():
    mOuputFolder("."),
    mVersion("0")
{

}

Model::LogicDbSerializer::~LogicDbSerializer()
{

}

bool Model::LogicDbSerializer::Serialize(const Model::MemoryModelPtr& aMemoryModel)
{
    const Model::TileMapPtr& tileMap = aMemoryModel->GetTileMap();

    for (auto& itorTile : *tileMap)
    {
        const TilePtr tile = itorTile.second;
        DbRepositoryPtr dbRepository = std::make_shared<DbRepository>(createDbPath(tile));

        if (!dbRepository->GenerateDataBase(tile))
        {
            return false;
        }

        if (!dbRepository->StoreRoadElement(aMemoryModel))
        {
            return false;
        }

        if (!dbRepository->StoreTopology(aMemoryModel))
        {
            return false;
        }
    }

    return true;
}

std::string Model::LogicDbSerializer::createDbPath(const TilePtr& aTile) const
{
    std::string path("");

    path += (mOuputFolder + "/logic_");
    path += (mVersion + "_");
    path += (std::to_string(aTile->GetTileId()));
    path += ".db";

    return path;
}



























