/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DLMFactory.cpp
 * @brief
 *******************************************************************************
 */

#include "DLMFactory.h"
#include "DLMSerializer.h"

Model::DLMFactory::DLMFactory() :
    mFileName("dlm.xml"),
    mInterval(0.5)
{

}

Model::DLMFactory::~DLMFactory()
{

}

void Model::DLMFactory::SetOutputFileName(const std::string& aFileName)
{
    mFileName = aFileName;
}

const std::string& Model::DLMFactory::GetOutputFileName() const
{
    return mFileName;
}

void Model::DLMFactory::SetOutputInterval(const double& aOutputInterval)
{
    mInterval = aOutputInterval;
}

const double& Model::DLMFactory::GetOutputInterval() const
{
    return mInterval;
}

Model::IParserPtr Model::DLMFactory::CreateParser()
{
    return nullptr;
}

Model::ISerializerPtr Model::DLMFactory::CreateSerializer()
{
    return std::unique_ptr<ISerializer>(new DLMSerializer(mFileName, mInterval));
}
