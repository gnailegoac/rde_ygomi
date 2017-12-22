/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DbParseException.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include <exception>
#include <string>

namespace Model
{

class DbParseException : public std::exception
{
public:
    enum class Type
    {
        DbParseError,
        RoadPointillizeError,
        TrafficSignPointillizeError,
        LaneConnectionError,
        CoordinateTransformError
    };

public:
    DbParseException(Type aType, const std::string& aError);
    ~DbParseException();

    Type GetType() const;
    void SetType(Type aType);

    const char* what() const throw() override;

private:
    Type mType;
    std::string mError;
};

}
