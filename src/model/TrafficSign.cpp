/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    TrafficSign.cpp
 * @brief
 *******************************************************************************
 */

#include "TrafficSign.h"

TrafficSign::TrafficSign():
    Element(),
    mTrafficSignType(0),
    mOrientation(0),
    mShapeWidth(0.0),
    mShapeHeight(0.0),
    mConfidence(0.0),
    mPosition()
{

}

TrafficSign::~TrafficSign()
{

}

std::uint64_t TrafficSign::GetTrafficSignType() const
{
    return mTrafficSignType;
}

void TrafficSign::SetTrafficSignType(std::uint64_t aTrafficSignType)
{
    mTrafficSignType = aTrafficSignType;
}

std::int8_t TrafficSign::GetOrientation() const
{
    return mOrientation;
}

void TrafficSign::SetOrientation(std::int8_t aOrientation)
{
    mOrientation = aOrientation;
}

double TrafficSign::GetShapeWidth() const
{
    return mShapeWidth;
}

void TrafficSign::SetShapeWidth(double aShapeWidth)
{
    mShapeWidth = aShapeWidth;
}

double TrafficSign::GetShapeHeight() const
{
    return mShapeHeight;
}

void TrafficSign::SetShapeHeight(double aShapeHeight)
{
    mShapeHeight = aShapeHeight;
}

float TrafficSign::GetConfidence() const
{
    return mConfidence;
}

void TrafficSign::SetConfidence(double aConfidence)
{
    mConfidence = aConfidence;
}

Point3D TrafficSign::GetPosition() const
{
    return mPosition;
}

void TrafficSign::SetPosition(Point3D aPosition)
{
    mPosition = aPosition;
}
