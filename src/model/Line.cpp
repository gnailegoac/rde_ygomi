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
    mCurveList(),
    mConfidence(0.0),
    mLength(0.0),
    mLane(nullptr),
    mPointListMap()
{

}

Model::Line::~Line()
{

}

std::uint64_t Model::Line::GetLineId() const
{
    return mLineId;
}

void Model::Line::SetLineId(std::uint64_t aLineId)
{
    mLineId = aLineId;
}

const std::vector<std::shared_ptr<Model::Curve>> Model::Line::GetCurveList() const
{
    return mCurveList;
}

std::vector<std::shared_ptr<Model::Curve>>* Model::Line::GetMutableCurveList()
{
    return &mCurveList;
}

size_t Model::Line::GetCurveListSize() const
{
    return mCurveList.size();
}

std::shared_ptr<Model::Curve> Model::Line::GetCurve(size_t aIndex)
{
    if (aIndex < mCurveList.size())
    {
        return mCurveList[aIndex];
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

std::shared_ptr<Model::Lane> Model::Line::GetLane() const
{
    return mLane;
}

void Model::Line::SetLane(std::shared_ptr<Model::Lane> aLane)
{
    mLane = aLane;
}

const std::unordered_map<std::uint8_t, std::vector<Model::Point3D>> Model::Line::GetPointListMap() const
{
    return mPointListMap;
}

std::unordered_map<std::uint8_t, std::vector<Model::Point3D>>* Model::Line::GetMutablePointListMap()
{
    return &mPointListMap;
}

std::vector<Model::Point3D>* Model::Line::GetPointListByLevel(std::uint8_t aLevel)
{
    if (0 != mPointListMap.count(aLevel))
    {
        return &(mPointListMap[aLevel]);
    }

    return nullptr;
}
