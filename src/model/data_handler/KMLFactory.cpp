/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    KMLFactory.cpp
 * @brief
 *******************************************************************************
 */

#include "KMLFactory.h"

#include <ctime>
#include <sys/stat.h>

#include "KMLSerializer.h"

Model::KMLFactory::KMLFactory(std::string aKMLFolder):
    mOutputInterval(0.1),
    mKMLFolder(aKMLFolder)
{
    SetKMLFolder(aKMLFolder);
}

Model::KMLFactory::~KMLFactory()
{

}

void Model::KMLFactory::SetKMLFolder(const std::string& aKMLFolder)
{
    mKMLFolder = aKMLFolder;
    std::string folder = mKMLFolder + "/" + getCurrentTime();

    if(0 == mkdir(folder.c_str(), 0755))
    {
        mKMLFolder = folder;
    }
}

const std::string& Model::KMLFactory::GetKMLFolder() const
{
    return mKMLFolder;
}

void Model::KMLFactory::SetOutputInterval(const double& aOutputInterval)
{
    mOutputInterval = aOutputInterval;
}

const double& Model::KMLFactory::GetOutputInterval() const
{
    return mOutputInterval;
}

Model::IParserPtr Model::KMLFactory::CreateParser()
{
    return nullptr;
}

Model::ISerializerPtr Model::KMLFactory::CreateSerializer()
{
    return std::unique_ptr<ISerializer>(new KMLSerializer(mKMLFolder, mOutputInterval));
}

std::string Model::KMLFactory::getCurrentTime() const
{
    time_t t = time(nullptr);
    struct tm* tmNow = localtime(&t);
    char timeStr[sizeof("yyyy-mm-dd_hh:mm:ss")] = {'\0'};
    std::strftime(timeStr, sizeof(timeStr), "%Y-%2m-%2d_%2H:%2M:%2S", tmNow);
    return timeStr;
}
