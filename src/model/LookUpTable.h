/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    LookUpTable.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include <unordered_map>
#include <string>

namespace Model
{

class LookUpTable
{
public:
    static std::string mDefaultTrafficSignImagePath;
    static std::unordered_map<std::uint64_t, std::string> mTrafficSignImageMap;
};

}
