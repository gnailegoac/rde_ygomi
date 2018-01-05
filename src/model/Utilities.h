/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Utilities
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Common.h"

#include <type_traits>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace strings
{

template <class T>
std::vector<std::basic_string<T>> Split(const std::basic_string<T>& aString,
                                        const std::basic_string<T>& aSeparator)
{
    std::vector<std::basic_string<T>> theStrings;
    typename std::basic_string<T>::size_type theLastPos = aString.find_first_not_of(aSeparator, 0);
    typename std::basic_string<T>::size_type theDelPos = aString.find_first_of(aSeparator, theLastPos);

    while(std::basic_string<T>::npos != theDelPos || std::basic_string<T>::npos != theLastPos)
    {
        theStrings.push_back(aString.substr(theLastPos, theDelPos - theLastPos));
        theLastPos = aString.find_first_not_of(aSeparator, theDelPos);
        theDelPos = aString.find_first_of(aSeparator, theLastPos);
    }

    return theStrings;
}

template <class T>
std::vector<std::basic_string<T>> Split(const std::basic_string<T>& aString,
                                        const T* aSeparator)
{
    return Split(aString, std::basic_string<T>(aSeparator));
}

template <class T>
bool StartWith(const std::basic_string<T>& aString,
               const std::basic_string<T>& aStarting)
{
    if (aStarting.size() > aString.size())
    {
        return false;
    }

    return std::equal(aStarting.begin(), aStarting.end(), aString.begin());
}

template <class T>
bool StartWith(const std::basic_string<T>& aString,
               const T* aStarting)
{
    return StartWith(aString, std::basic_string<T>(aStarting));
}

template <class T>
bool EndWith(const std::basic_string<T>& aString,
             const std::basic_string<T>& aEnding)
{
    if (aEnding.size() > aString.size())
    {
        return false;
    }

    return std::equal(aEnding.rbegin(), aEnding.rend(), aString.rbegin());
}

template <class T>
bool EndWith(const std::basic_string<T>& aString,
             const T* aEnding)
{
    return EndWith(aString, std::basic_string<T>(aEnding));
}

template<typename T,
         typename std::enable_if<std::is_floating_point<T>::value, T>::type* = nullptr>
std::string FormatFloat(const T& aValue, int aPrecision)
{
    std::stringstream ss;

    ss << std::setprecision(aPrecision)
       << std::setiosflags(std::ios_base::fixed)
       << aValue;

    return ss.str();
}

}
