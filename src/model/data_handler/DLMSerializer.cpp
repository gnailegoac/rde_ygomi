/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DLMSerializer.cpp
 * @brief
 *******************************************************************************
 */

#include "DLMSerializer.h"
#include "DLMInterpreter.h"

Model::DLMSerializer::DLMSerializer() :
    mFileName("dlm.xml"),
    mInterval(0.5)
{

}

Model::DLMSerializer::DLMSerializer(const std::string& aFileName, const double& aInterval) :
    mFileName(aFileName),
    mInterval(aInterval)
{

}

Model::DLMSerializer::~DLMSerializer()
{

}

bool Model::DLMSerializer::Serialize(const Model::MemoryModelPtr& aMemoryModel)
{
    DLMInterpreterPtr dlmInterpreter = std::make_shared<Model::DLMInterpreter>(mFileName, mInterval);
    return dlmInterpreter->SaveDetailJournal(aMemoryModel);
}
