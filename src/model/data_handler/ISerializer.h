/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    ISerializer.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "../Common.h"

namespace Model
{

class MemoryModel;
typedef std::shared_ptr<MemoryModel> MemoryModelPtr;

class ISerializer
{
public:
    virtual ~ISerializer() = default;

    virtual bool Serialize(const MemoryModelPtr& aMemoryModel) = 0;
};

typedef std::unique_ptr<ISerializer> ISerializerPtr;

}
