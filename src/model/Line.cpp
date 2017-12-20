/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Line.cpp
 * @brief
 *******************************************************************************
 */

#include "Line.h"

Model::Line::Line():
    Element(),
    mLineId(0),
    mCurveList(std::make_shared<CurveList>()),
    mConfidence(0.0),
    mLength(0.0),
    mLane(nullptr),
    mGeodeticPoints(std::make_shared<Point3DList>()),
    mPointListMap(std::make_shared<ViewPointMap>())
{

}

Model::Line::~Line()
{

}

const uint64_t& Model::Line::GetLineId() const
{
    return mLineId;
}

void Model::Line::SetLineId(const uint64_t& aLineId)
{
    mLineId = aLineId;
}

const Model::CurveListPtr& Model::Line::GetCurveList() const
{
    return mCurveList;
}

Model::CurveListPtr Model::Line::GetMutableCurveList()
{
    return mCurveList;
}

size_t Model::Line::GetCurveListSize() const
{
    return mCurveList->size();
}

Model::CurvePtr Model::Line::GetCurve(const size_t& aIndex)
{
    if (aIndex < mCurveList->size())
    {
        return mCurveList->at(aIndex);
    }

    return nullptr;
}

float Model::Line::GetConfidence() const
{
    return mConfidence;
}

void Model::Line::SetConfidence(float aConfidence)
{
    mConfidence = aConfidence;
}

float Model::Line::GetLength() const
{
    return mLength;
}

void Model::Line::SetLength(float aLength)
{
    mLength = aLength;
}

const Model::LanePtr& Model::Line::GetLane() const
{
    return mLane;
}

void Model::Line::SetLane(const Model::LanePtr& aLane)
{
    mLane = aLane;
}

const Model::Point3DListPtr& Model::Line::GetGeodeticPoints() const
{
    return mGeodeticPoints;
}

Model::Point3DListPtr Model::Line::GetMutableGeodeticPoints()
{
    return mGeodeticPoints;
}

void Model::Line::SetGeodeticPoints(const Model::Point3DListPtr& aGeodeticPoints)
{
    mGeodeticPoints = aGeodeticPoints;
}

const Model::ViewPointMapPtr& Model::Line::GetPointListMap() const
{
    return mPointListMap;
}

Model::ViewPointMapPtr Model::Line::GetMutablePointListMap()
{
    return mPointListMap;
}

Model::Point3DListPtr Model::Line::GetPointListByLevel(std::uint8_t aLevel)
{
    if (0 != mPointListMap->count(aLevel))
    {
        return mPointListMap->at(aLevel);
    }

    return nullptr;
}

void Model::Line::GenerateViewPointMap()
{
    // Convert geodetic coordinates into UTM coordinates


    // Down-sample points with Douglas-Peucker algorithm

}
