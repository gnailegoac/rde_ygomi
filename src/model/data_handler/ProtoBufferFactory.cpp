/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    ProtoBufferFactory.cpp
 * @brief
 *******************************************************************************
 */

#include "ProtoBufferFactory.h"

#include "ProtoBufferSerializer.h"

Model::ProtoBufferFactory::ProtoBufferFactory() :
    mFileName("out.bin"),
    mInterval(0.5)
{

}

Model::ProtoBufferFactory::~ProtoBufferFactory()
{

}

void Model::ProtoBufferFactory::SetOutputFileName(const std::string& aFileName)
{
    mFileName = aFileName;
}

const std::string& Model::ProtoBufferFactory::GetOutputFileName() const
{
    return mFileName;
}

void Model::ProtoBufferFactory::SetOutputInterval(const double& aOutputInterval)
{
    mInterval = aOutputInterval;
}

const double& Model::ProtoBufferFactory::GetOutputInterval() const
{
    return mInterval;
}

Model::IParserPtr Model::ProtoBufferFactory::CreateParser()
{
    return nullptr;
}

Model::ISerializerPtr Model::ProtoBufferFactory::CreateSerializer()
{
    return std::unique_ptr<ISerializer>(new ProtoBufferSerializer(mFileName, mInterval));
}
