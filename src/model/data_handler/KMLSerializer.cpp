/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    KMLSerializer.cpp
 * @brief
 *******************************************************************************
 */

#include "KMLSerializer.h"

#include "../MemoryModel.h"
#include "DbRepository.h"

Model::KMLSerializer::KMLSerializer():
    mInterval(0.1)
{

}

Model::KMLSerializer::KMLSerializer(double aInterval):
    mInterval(aInterval)
{

}

Model::KMLSerializer::~KMLSerializer()
{

}

bool Model::KMLSerializer::Serialize(const Model::MemoryModelPtr& aMemoryModel)
{
    return false;
}
