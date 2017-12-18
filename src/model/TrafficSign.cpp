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
    mPosition(std::make_shared<Point3D>())
{

}

Model::TrafficSign::~TrafficSign()
{

}

std::uint64_t Model::TrafficSign::GetTrafficSignId() const
{
    return mTrafficSignId;
}

void Model::TrafficSign::SetTrafficSignId(std::uint64_t aTrafficSignId)
{
    mTrafficSignId = aTrafficSignId;
}

std::uint64_t Model::TrafficSign::GetTrafficSignType() const
{
    return mTrafficSignType;
}

void Model::TrafficSign::SetTrafficSignType(std::uint64_t aTrafficSignType)
{
    mTrafficSignType = aTrafficSignType;
}

std::int32_t Model::TrafficSign::GetOrientation() const
{
    return mOrientation;
}

void Model::TrafficSign::SetOrientation(std::int32_t aOrientation)
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

Model::Point3DConstPtr Model::TrafficSign::GetPosition() const
{
    return mPosition;
}

void Model::TrafficSign::SetPosition(Point3DPtr aPosition)
{
    mPosition = aPosition;
}
