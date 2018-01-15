/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    KMLFactory.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "../Common.h"
#include "IFactory.h"

namespace Model
{

class KMLFactory : public IFactory
{
public:
    KMLFactory();
    ~KMLFactory();

    IParserPtr CreateParser() override;
    ISerializerPtr CreateSerializer() override;
};

}
