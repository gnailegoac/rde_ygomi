/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Factory.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "IFactory.h"

namespace Model
{

class Factory
{
public:
    static IFactoryPtr CreateLogicDbFactory(const std::vector<std::string>& aDbPathList);

    static IFactoryPtr CreateLogicDbFactory(const std::string& aDbPath);

    static IFactoryPtr CreateKMLFactory(const std::string& aKMLFolder, const double& aInterval = 1.0);
};

}
