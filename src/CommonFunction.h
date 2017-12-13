/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    CommonFunction.h
 * @brief   To handle pick event
 *******************************************************************************
 */

#pragma once

class CommonFunction
{
public:
    template<typename T>
    static T* ConvertToNonConstType(const void* aValue)
    {
        T* result = static_cast<T*>(const_cast<void*>(aValue));
        return result;
    }
};
