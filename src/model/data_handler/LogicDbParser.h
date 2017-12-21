/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    LogicDbParser.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "../Common.h"
#include "IParser.h"

namespace Model
{

class Curve;
class Line;
class Lane;

class LogicDbParser : public IParser
{
public:
    LogicDbParser(const PathList& aLogicDbFileList);
    LogicDbParser(const PathListPtr& aLogicDbFileList);
    ~LogicDbParser();

    virtual MemoryModelPtr Parse() override;

private:
    bool createTopology(MemoryModelPtr& aMemoryModel);
    bool createViewMap(MemoryModelPtr& aMemoryModel);

private:
    PathListPtr mLogicDbFileList;
};

}
