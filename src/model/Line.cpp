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

Line::Line():
    Element(),
    mLineId(0),
    mCurveList(),
    mConfidence(0.0),
    mLength(0.0),
    mLane(nullptr),
    mPointListMap()
{

}

Line::~Line()
{

}

std::uint64_t Line::GetLineId() const
{
    return mLineId;
}

void Line::SetLineId(std::uint64_t aLineId)
{
    mLineId = aLineId;
}

const std::vector<std::shared_ptr<Curve>> Line::GetCurveList() const
{
    return mCurveList;
}

std::vector<std::shared_ptr<Curve>>* Line::GetMutableCurveList()
{
    return &mCurveList;
}

size_t Line::GetCurveListSize() const
{
    return mCurveList.size();
}

std::shared_ptr<Curve> Line::GetCurve(size_t aIndex)
{
    if (aIndex < mCurveList.size())
    {
        return mCurveList[aIndex];
    }

    return nullptr;
}

float Line::GetConfidence() const
{
    return mConfidence;
}

void Line::SetConfidence(float aConfidence)
{
    mConfidence = aConfidence;
}

float Line::GetLength() const
{
    return mLength;
}

void Line::SetLength(float aLength)
{
    mLength = aLength;
}

std::shared_ptr<Lane> Line::GetLane() const
{
    return mLane;
}

void Line::SetLane(std::shared_ptr<Lane> aLane)
{
    mLane = aLane;
}

const std::unordered_map<std::uint8_t, std::vector<Point3D>> Line::GetPointListMap() const
{
    return mPointListMap;
}

std::unordered_map<std::uint8_t, std::vector<Point3D>>* Line::GetMutablePointListMap()
{
    return &mPointListMap;
}

std::vector<Point3D>* Line::GetPointListByLevel(std::uint8_t aLevel)
{
    if (0 != mPointListMap.count(aLevel))
    {
        return &(mPointListMap[aLevel]);
    }

    return nullptr;
}
