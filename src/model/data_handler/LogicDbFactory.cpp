/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    LogicDbFactory.cpp
 * @brief
 *******************************************************************************
 */

#include "LogicDbFactory.h"

#include "LogicDbParser.h"

Model::LogicDbFactory::LogicDbFactory(const PathListPtr& aInputPathList)
{
    mInputPathList = std::make_shared<PathList>();
    mInputPathList->insert(mInputPathList->begin(), aInputPathList->begin(), aInputPathList->end());
}

Model::LogicDbFactory::LogicDbFactory(const PathList& aInputPathList)
{
    mInputPathList = std::make_shared<PathList>();
    mInputPathList->insert(mInputPathList->begin(), aInputPathList.begin(), aInputPathList.end());
}

Model::LogicDbFactory::LogicDbFactory(const std::string& aInputPath)
{
    mInputPathList = std::make_shared<PathList>();
    mInputPathList->push_back(aInputPath);
}

Model::LogicDbFactory::~LogicDbFactory()
{

}

Model::IParserPtr Model::LogicDbFactory::CreateParser()
{
    return std::unique_ptr<IParser>(new LogicDbParser(mInputPathList));
}

Model::ISerializerPtr Model::LogicDbFactory::CreateSerializer()
{
    return nullptr;
}
