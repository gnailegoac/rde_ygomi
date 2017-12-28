/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    LogicDbFactory.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "../Common.h"
#include "IFactory.h"

namespace Model
{

class LogicDbFactory : public IFactory
{
public:
    LogicDbFactory();
    ~LogicDbFactory();

    void SetInputPathList(const PathListPtr& aInputPathList);
    void SetInputPathList(const PathList& aInputPathList);
    void SetInputPath(const std::string& aInputPath);
    void SetOutputFolder(const std::string& aOutputFolder, std::string aVersion = "0");

    IParserPtr CreateParser() override;
    ISerializerPtr CreateSerializer() override;

private:
    std::string getCurrentTime() const;

private:
    PathListPtr mInputPathList;
    std::string mOutputFolder;
    std::string mVersion;
};

}
