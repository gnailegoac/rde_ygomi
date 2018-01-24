/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    ProtoBufferSerializer.cpp
 * @brief
 *******************************************************************************
 */

#include "ProtoBufferSerializer.h"

Model::ProtoBufferSerializer::ProtoBufferSerializer() :
    mFileName("out.bin"),
    mInterval(0.5)
{

}

Model::ProtoBufferSerializer::ProtoBufferSerializer(const std::string& aFileName,
                                                    const double& aInterval):
    mFileName(aFileName),
    mInterval(aInterval)
{

}

Model::ProtoBufferSerializer::~ProtoBufferSerializer()
{

}

bool Model::ProtoBufferSerializer::Serialize(const Model::MemoryModelPtr& aMemoryModel)
{
    return true;
}
