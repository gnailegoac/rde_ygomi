/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DLMSerializer.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "ISerializer.h"

#include <string>

namespace Model
{

class DLMSerializer : public ISerializer
{
public:
    DLMSerializer();
    DLMSerializer(const std::string& aFileName, const double& aInterval = 0.5);
    ~DLMSerializer();

    bool Serialize(const MemoryModelPtr& aMemoryModel) override;

private:
    std::string mFileName;
    double mInterval;
};

}
