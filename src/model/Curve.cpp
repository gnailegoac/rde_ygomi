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

Model::Point3DListPtr Model::Curve::CalculatePointCloud(const double&)
{
    return nullptr;
}

const uint64_t& Model::Curve::GetCurveId() const
{
    return mCurveId;
}

void Model::Curve::SetCurveId(const uint64_t& aCurveId)
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

std::uint32_t Model::Curve::GetIndexInLine() const
{
    return mIndexInLine;
}

void Model::Curve::SetIndexInLine(std::uint32_t aIndexInLine)
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

const Model::PaintRangePtr& Model::Curve::GetPaintRange() const
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

std::pair<double, double> Model::Curve::GetPaintRange(const size_t& aIndex) const
{
    if (aIndex < mPaintRange->size())
    {
        return mPaintRange->at(aIndex);
    }

    return std::make_pair(DBL_NAN, DBL_NAN);
}
