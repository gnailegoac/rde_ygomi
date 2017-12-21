/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Factory.cpp
 * @brief
 *******************************************************************************
 */

#include "Factory.h"

#include "../Common.h"
#include "LogicDbFactory.h"

Model::IFactoryPtr Model::Factory::CreateLogicDbFactory(const std::vector<std::string>& aDbPathList)
{
   return std::unique_ptr<LogicDbFactory>(new LogicDbFactory(aDbPathList));
}

Model::IFactoryPtr Model::Factory::CreateLogicDbFactory(const std::string& aDbPath)
{
    return std::unique_ptr<LogicDbFactory>(new LogicDbFactory(aDbPath));
}
