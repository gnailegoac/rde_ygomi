/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DbConvertException.cpp
 * @brief
 *******************************************************************************
 */

#include "DbParseException.h"


Model::DbParseException::DbParseException(Model::DbParseException::Type aType,
                                                    const std::string& aError):
    mType(aType),
    mError(aError)
{

}

Model::DbParseException::~DbParseException()
{

}

Model::DbParseException::Type Model::DbParseException::GetType() const
{
    return mType;
}

void Model::DbParseException::SetType(Model::DbParseException::Type aType)
{
    mType = aType;
}

const char* Model::DbParseException::what() const throw()
{
    return mError.c_str();
}
