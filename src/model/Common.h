/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Common.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <limits>
#include <cstring>


namespace Model
{

#ifndef DBL_NAN
#define DBL_NAN (std::numeric_limits<double>::quiet_NaN())
#endif

}
