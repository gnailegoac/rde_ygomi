/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    ConnectionRelation.cpp
 * @brief
 *******************************************************************************
 */

#include "Curve.h"

Curve::Curve():
    mCurveId(0),
    mLength(0.0),
    mCurveType(CurveType::UnDefine),
    mIndexInLine(-1),
    mWidth(0.0),
    mPaintRange()
{

}

Curve::~Curve()
{

}

std::uint64_t Curve::GetCurveId() const
{
    return mCurveId;
}

void Curve::SetCurveId(std::uint64_t aCurveId)
{
    mCurveId = aCurveId;
}

float Curve::GetLength() const
{
    return mLength;
}

void Curve::SetLength(float aLength)
{
    mLength = aLength;
}

CurveType Curve::GetCurveType() const
{
    return mCurveType;
}

void Curve::SetCurveType(CurveType aCurveType)
{
    mCurveType = aCurveType;
}

std::int8_t Curve::GetIndexInLine() const
{
    return mIndexInLine;
}

void Curve::SetIndexInLine(std::int8_t aIndexInLine)
{
    mIndexInLine = aIndexInLine;
}

float Curve::GetWidth() const
{
    return mWidth;
}

void Curve::SetWidth(float aWidth)
{
    mWidth = aWidth;
}

const std::vector<std::pair<double, double>>& Curve::GetPaintRange() const
{
    return mPaintRange;
}

std::vector<std::pair<double, double>>* Curve::GetMutablePaintRange()
{
    return &mPaintRange;
}

size_t Curve::GetPaintRangeSize() const
{
    return mPaintRange.size();
}

std::pair<double, double> Curve::GetPaintRange(size_t aIndex) const
{
    if (aIndex < mPaintRange.size())
    {
        return mPaintRange[aIndex];
    }

    return std::make_pair(DBL_NAN, DBL_NAN);
}
