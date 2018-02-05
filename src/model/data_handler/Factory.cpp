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
#include "KMLFactory.h"
#include "ProtoBufferFactory.h"
#include "DLMFactory.h"

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

Model::IFactoryPtr Model::Factory::CreateKMLFactory(const std::string& aKMLFolder,
                                                    const double &aInterval)
{
    std::unique_ptr<KMLFactory> kmlFactory(new KMLFactory(aKMLFolder));
    kmlFactory->SetOutputInterval(aInterval);
    return std::move(kmlFactory);
}

Model::IFactoryPtr Model::Factory::CreateProtoBufferFactory(const std::string& aFileName,
                                                            const double& aInterval)
{
    std::unique_ptr<ProtoBufferFactory> protoBufferFactory(new ProtoBufferFactory());
    protoBufferFactory->SetOutputFileName(aFileName);
    protoBufferFactory->SetOutputInterval(aInterval);
    return std::move(protoBufferFactory);
}

Model::IFactoryPtr Model::Factory::CreateDLMFactory(const std::string& aFileName,
                                                    const double& aInterval)
{
    std::unique_ptr<DLMFactory> dlmFactory(new DLMFactory());
    dlmFactory->SetOutputFileName(aFileName);
    dlmFactory->SetOutputInterval(aInterval);
    return std::move(dlmFactory);
}
