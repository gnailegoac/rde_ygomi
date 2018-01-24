/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    ProtoBufferInterpreter.cpp
 * @brief
 *******************************************************************************
 */

#include "ProtoBufferInterpreter.h"

Model::ProtoBufferInterpreter::ProtoBufferInterpreter(const std::string& aFileName,
                                                      const double& aInterval):
    mFileName(aFileName),
    mInterval(aInterval)
{

}

Model::ProtoBufferInterpreter::~ProtoBufferInterpreter()
{

}

bool Model::ProtoBufferInterpreter::SaveRoadSections(const MemoryModelPtr& aMemoryModel)
{
    return true;
}
