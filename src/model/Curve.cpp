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

Model::Curve::Curve():
    mCurveId(0),
    mLength(0.0),
    mCurveType(CurveType::UnDefined),
    mIndexInLine(-1),
    mWidth(0.0),
    mPaintRange(std::make_shared<std::vector<std::pair<double, double>>>())
{

}

Model::Curve::~Curve()
{

}

std::uint64_t Model::Curve::GetCurveId() const
{
    return mCurveId;
}

void Model::Curve::SetCurveId(std::uint64_t aCurveId)
{
    mCurveId = aCurveId;
}

float Model::Curve::GetLength() const
{
    return mLength;
}

void Model::Curve::SetLength(float aLength)
{
    mLength = aLength;
}

Model::CurveType Model::Curve::GetCurveType() const
{
    return mCurveType;
}

void Model::Curve::SetCurveType(CurveType aCurveType)
{
    mCurveType = aCurveType;
}

std::int8_t Model::Curve::GetIndexInLine() const
{
    return mIndexInLine;
}

void Model::Curve::SetIndexInLine(std::int8_t aIndexInLine)
{
    mIndexInLine = aIndexInLine;
}

float Model::Curve::GetWidth() const
{
    return mWidth;
}

void Model::Curve::SetWidth(float aWidth)
{
    mWidth = aWidth;
}

Model::PaintRangeConstPtr Model::Curve::GetPaintRange()
{
    return mPaintRange;
}

Model::PaintRangePtr Model::Curve::GetMutablePaintRange()
{
    return mPaintRange;
}

size_t Model::Curve::GetPaintRangeSize() const
{
    return mPaintRange->size();
}

std::pair<double, double> Model::Curve::GetPaintRange(size_t aIndex) const
{
    if (aIndex < mPaintRange->size())
    {
        return mPaintRange->at(aIndex);
    }

    return std::make_pair(DBL_NAN, DBL_NAN);
}
