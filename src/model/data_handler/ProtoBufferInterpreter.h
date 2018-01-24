/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    ProtoBufferInterpreter.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include <string>
#include <memory>

namespace Model
{

class MemoryModel;
typedef std::shared_ptr<MemoryModel> MemoryModelPtr;

class ProtoBufferInterpreter
{
public:
    ProtoBufferInterpreter(const std::string& aFileName, const double& aInterval = 0.5);
    ~ProtoBufferInterpreter();

    bool SaveRoadSections(const MemoryModelPtr& aMemoryModel);

private:
    std::string mFileName;
    double mInterval;
};

}
