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

Model::TrafficSign::TrafficSign():
    Element(),
    mTrafficSignType(0),
    mOrientation(0),
    mShapeWidth(0.0),
    mShapeHeight(0.0),
    mConfidence(0.0),
    mPosition()
{

}

Model::TrafficSign::~TrafficSign()
{

}

std::uint64_t Model::TrafficSign::GetTrafficSignType() const
{
    return mTrafficSignType;
}

void Model::TrafficSign::SetTrafficSignType(std::uint64_t aTrafficSignType)
{
    mTrafficSignType = aTrafficSignType;
}

std::int8_t Model::TrafficSign::GetOrientation() const
{
    return mOrientation;
}

void Model::TrafficSign::SetOrientation(std::int8_t aOrientation)
{
    mOrientation = aOrientation;
}

double Model::TrafficSign::GetShapeWidth() const
{
    return mShapeWidth;
}

void Model::TrafficSign::SetShapeWidth(double aShapeWidth)
{
    mShapeWidth = aShapeWidth;
}

double Model::TrafficSign::GetShapeHeight() const
{
    return mShapeHeight;
}

void Model::TrafficSign::SetShapeHeight(double aShapeHeight)
{
    mShapeHeight = aShapeHeight;
}

float Model::TrafficSign::GetConfidence() const
{
    return mConfidence;
}

void Model::TrafficSign::SetConfidence(double aConfidence)
{
    mConfidence = aConfidence;
}

Model::Point3D Model::TrafficSign::GetPosition() const
{
    return mPosition;
}

void Model::TrafficSign::SetPosition(Point3D aPosition)
{
    mPosition = aPosition;
}
