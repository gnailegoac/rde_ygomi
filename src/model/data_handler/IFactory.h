/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    IFactory.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "IParser.h"
#include "ISerializer.h"

namespace Model
{

class IFactory
{
public:
    virtual IParserPtr CreateParser() = 0;

    virtual ISerializerPtr CreateSerializer() = 0;
};

typedef std::unique_ptr<IFactory> IFactoryPtr;

}
