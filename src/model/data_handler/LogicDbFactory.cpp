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

#include <ctime>
#include <sys/stat.h>

#include "LogicDbParser.h"
#include "LogicDbSerializer.h"

Model::LogicDbFactory::LogicDbFactory():
    mInputPathList(std::make_shared<PathList>()),
    mOutputFolder("."),
    mVersion("0")
{
}

Model::LogicDbFactory::~LogicDbFactory()
{

}

void Model::LogicDbFactory::SetInputPathList(const PathListPtr& aInputPathList)
{
    mInputPathList->insert(mInputPathList->begin(), aInputPathList->begin(), aInputPathList->end());
}

void Model::LogicDbFactory::SetInputPathList(const PathList& aInputPathList)
{
    mInputPathList->insert(mInputPathList->begin(), aInputPathList.begin(), aInputPathList.end());
}

void Model::LogicDbFactory::SetInputPath(const std::string& aInputPath)
{
    mInputPathList->push_back(aInputPath);
}

void Model::LogicDbFactory::SetOutputFolder(const std::string& aOutputFolder, std::string aVersion)
{
    mOutputFolder = aOutputFolder;
    mVersion = aVersion;

    std::string folder = aOutputFolder + "/" +getCurrentTime();

    if(0 == mkdir(folder.c_str(), 0755))
    {
        mOutputFolder = folder;
    }
}

Model::IParserPtr Model::LogicDbFactory::CreateParser()
{
    return std::unique_ptr<IParser>(new LogicDbParser(mInputPathList));
}

Model::ISerializerPtr Model::LogicDbFactory::CreateSerializer()
{
    return std::unique_ptr<ISerializer>(new LogicDbSerializer(mOutputFolder, mVersion));
}


std::string Model::LogicDbFactory::getCurrentTime() const
{
    time_t t = time(nullptr);
    struct tm* tmNow = localtime(&t);
    char timeStr[sizeof("yyyy-mm-dd_hh:mm:ss")] = {'\0'};
    std::strftime(timeStr, sizeof(timeStr), "%Y-%2m-%2d_%2H:%2M:%2S", tmNow);
    return timeStr;
}
