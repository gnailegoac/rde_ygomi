/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    IdGenerator.cpp
 * @brief
 *******************************************************************************
 */

#include "IdGenerator.h"

Model::IdGenerator::IdGenerator():
    mId(1)
{

}

Model::IdGenerator::~IdGenerator()
{

}

std::uint64_t Model::IdGenerator::GetId() const
{
    return mId;
}

std::uint64_t Model::IdGenerator::GetNewId()
{
    return ++mId;
}
