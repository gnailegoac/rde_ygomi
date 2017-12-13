/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    NurbsCurve.cpp
 * @brief
 *******************************************************************************
 */

#include "NurbsCurve.h"

Model::NurbsCurve::NurbsCurve():
    Curve(),
    mControlPoints(),
    mKnots(),
    mPaintTotalLength(0.0),
    mLineLength(0.0)
{

}

Model::NurbsCurve::NurbsCurve(std::vector<Model::Point3D>& aControlPoints,
                              std::vector<double>& aKnots,
                              double aPaintTotalLength,
                              double aLineLength):
    Curve(),
    mPaintTotalLength(aPaintTotalLength),
    mLineLength(aLineLength)
{
    mControlPoints.insert(mControlPoints.begin(), aControlPoints.begin(), aControlPoints.end());
    mKnots.insert(mKnots.begin(), aKnots.begin(), aKnots.end());
}

Model::NurbsCurve::~NurbsCurve()
{

}

const std::vector<Model::Point3D> Model::NurbsCurve::GetControlPoints() const
{
    return mControlPoints;
}

std::vector<Model::Point3D>* Model::NurbsCurve::GetMutableControlPoints()
{
    return &mControlPoints;
}

size_t Model::NurbsCurve::GetControlPointsSize() const
{
    return mControlPoints.size();
}

Model::Point3D Model::NurbsCurve::GetControlPoint(size_t aIndex) const
{
    if (aIndex < mControlPoints.size())
    {
        return mControlPoints[aIndex];
    }

    return Point3D(DBL_NAN, DBL_NAN, DBL_NAN);
}

const std::vector<double> Model::NurbsCurve::GetKnots() const
{
    return mKnots;
}

std::vector<double>* Model::NurbsCurve::GetMutableKnots()
{
    return &mKnots;
}

size_t Model::NurbsCurve::GetKnotsSize() const
{
    return mKnots.size();
}

double Model::NurbsCurve::GetKnot(size_t aIndex) const
{
    if (aIndex < mKnots.size())
    {
        return mKnots[aIndex];
    }

    return DBL_NAN;
}

double Model::NurbsCurve::GetPaintTotalLength() const
{
    return mPaintTotalLength;
}

void Model::NurbsCurve::SetPaintTotalLength(double aPaintTotalLength)
{
    mPaintTotalLength = aPaintTotalLength;
}

double Model::NurbsCurve::GetLineLength() const
{
    return mLineLength;
}

void Model::NurbsCurve::SetLineLength(double aLineLength)
{
    mLineLength = aLineLength;
}
