/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    ProtoBufferSerializer.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "ISerializer.h"
#include <string>

namespace Model
{

class ProtoBufferSerializer : public ISerializer
{
public:
    ProtoBufferSerializer();
    ProtoBufferSerializer(const std::string& aFileName, const double& aInterval = 0.5);
    ~ProtoBufferSerializer();
    bool Serialize(const MemoryModelPtr& aMemoryModel) override;

private:
    std::string mFileName;
    double mInterval;
};

}
