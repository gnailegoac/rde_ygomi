/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    IParser.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "../Common.h"

namespace Model
{

class MemoryModel;
typedef std::shared_ptr<MemoryModel> MemoryModelPtr;

class IParser
{
public:
    virtual ~IParser() = default;

    virtual MemoryModelPtr Parse() = 0;
};

typedef std::unique_ptr<IParser> IParserPtr;

}
