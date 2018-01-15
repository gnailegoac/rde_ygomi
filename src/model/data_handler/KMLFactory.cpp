/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    KMLFactory.cpp
 * @brief
 *******************************************************************************
 */

#include "KMLFactory.h"

#include "KMLSerializer.h"

Model::KMLFactory::KMLFactory()
{
}

Model::KMLFactory::~KMLFactory()
{

}

Model::IParserPtr Model::KMLFactory::CreateParser()
{
    return nullptr;
}

Model::ISerializerPtr Model::KMLFactory::CreateSerializer()
{
    return std::unique_ptr<ISerializer>(new KMLSerializer);
}
