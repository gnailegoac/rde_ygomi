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
    LogicDbFactory(const PathListPtr& aInputPathList);
    LogicDbFactory(const PathList& aInputPathList);
    LogicDbFactory(const std::string& aInputPath);
    ~LogicDbFactory();

    IParserPtr CreateParser() override;
    ISerializerPtr CreateSerializer() override;

private:
    PathListPtr mInputPathList;
};

}
