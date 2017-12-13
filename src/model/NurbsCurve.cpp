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

NurbsCurve::NurbsCurve():
    Curve(),
    mControlPoints(),
    mKnots(),
    mPaintTotalLength(0.0),
    mLineLength(0.0)
{

}

NurbsCurve::NurbsCurve(std::vector<Point3D>& aControlPoints,
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

NurbsCurve::~NurbsCurve()
{

}

const std::vector<Point3D> NurbsCurve::GetControlPoints() const
{
    return mControlPoints;
}

std::vector<Point3D>* NurbsCurve::GetMutableControlPoints()
{
    return &mControlPoints;
}

size_t NurbsCurve::GetControlPointsSize() const
{
    return mControlPoints.size();
}

Point3D NurbsCurve::GetControlPoint(size_t aIndex) const
{
    if (aIndex < mControlPoints.size())
    {
        return mControlPoints[aIndex];
    }

    return Point3D(DBL_NAN, DBL_NAN, DBL_NAN);
}

const std::vector<double> NurbsCurve::GetKnots() const
{
    return mKnots;
}

std::vector<double>* NurbsCurve::GetMutableKnots()
{
    return &mKnots;
}

size_t NurbsCurve::GetKnotsSize() const
{
    return mKnots.size();
}

double NurbsCurve::GetKnot(size_t aIndex) const
{
    if (aIndex < mKnots.size())
    {
        return mKnots[aIndex];
    }

    return DBL_NAN;
}

double NurbsCurve::GetPaintTotalLength() const
{
    return mPaintTotalLength;
}

void NurbsCurve::SetPaintTotalLength(double aPaintTotalLength)
{
    mPaintTotalLength = aPaintTotalLength;
}

double NurbsCurve::GetLineLength() const
{
    return mLineLength;
}

void NurbsCurve::SetLineLength(double aLineLength)
{
    mLineLength = aLineLength;
}
