/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    LogicDbParser.cpp
 * @brief
 *******************************************************************************
 */

#include "LogicDbParser.h"

#include "../MemoryModel.h"
#include "DbRepository.h"

Model::LogicDbParser::LogicDbParser(const PathList& aLogicDbFileList)
{
    mLogicDbFileList = std::make_shared<PathList>();
    mLogicDbFileList->insert(mLogicDbFileList->begin(), aLogicDbFileList.begin(), aLogicDbFileList.end());
}

Model::LogicDbParser::LogicDbParser(const PathListPtr& aLogicDbFileList)
{
    mLogicDbFileList = aLogicDbFileList;
}

Model::LogicDbParser::~LogicDbParser()
{

}

Model::MemoryModelPtr Model::LogicDbParser::Parse()
{
    MemoryModelPtr memoryModel = std::make_shared<MemoryModel>();

    for (size_t i = 0; i < mLogicDbFileList->size(); i++)
    {
        DbRepositoryPtr dbRepository = std::make_shared<DbRepository>(mLogicDbFileList->at(i));

        if (!dbRepository->InitializeDataBase())
        {
            return nullptr;
        }

        if (!dbRepository->QueryRoadElement(memoryModel))
        {
            return nullptr;
        }
    }

    if (!createTopology(memoryModel)
            || !createViewMap(memoryModel))
    {
        return nullptr;
    }

    return memoryModel;
}

bool Model::LogicDbParser::createTopology(MemoryModelPtr& aMemoryModel)
{
    for (size_t i = 0; i < mLogicDbFileList->size(); i++)
    {
         DbRepositoryPtr dbRepository = std::make_shared<DbRepository>(mLogicDbFileList->at(i));

         if (!dbRepository->InitializeDataBase())
         {
             return false;
         }

         if(!dbRepository->QueryTopology(aMemoryModel))
         {
             return false;
         }
    }

    return true;
}

bool Model::LogicDbParser::createViewMap(MemoryModelPtr& aMemoryModel)
{
    const double samplingInterval = 0.5;
    TileMapPtr tileMap = aMemoryModel->GetMutableTileMap();

    for (auto itorTile : *tileMap)
    {
        TilePtr tile = itorTile.second;
        const Point3DPtr& referencePoint = tile->GetReferencePoint();
        LineMapPtr lineMap = tile->GetMutableLineMap();

        for (auto itorLine : *lineMap)
        {
            LinePtr line = itorLine.second;
            line->SortCurve();
            line->CreateGeodeticPointsList(referencePoint, samplingInterval);
            line->GenerateViewPaintMap();
        }
    }

    return true;
}
