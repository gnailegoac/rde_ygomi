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

#include <sys/stat.h>
#include <unistd.h>

#include "../Common.h"
#include "LogicDbFactory.h"

Model::IFactoryPtr Model::Factory::CreateLogicDbFactory(const std::vector<std::string>& aDbPathList)
{
   std::unique_ptr<LogicDbFactory> logicDbFactory(new LogicDbFactory);
   logicDbFactory->SetInputPathList(aDbPathList);
   return std::move(logicDbFactory);
}

Model::IFactoryPtr Model::Factory::CreateLogicDbFactory(const std::string& aDbPath)
{
    std::unique_ptr<LogicDbFactory> logicDbFactory(new LogicDbFactory);
    struct stat st;
    stat(aDbPath.c_str(),&st);

    if (S_ISDIR(st.st_mode))
    {
        logicDbFactory->SetOutputFolder(aDbPath);
    }
    else
    {
        logicDbFactory->SetInputPath(aDbPath);
    }

    return std::move(logicDbFactory);
}
