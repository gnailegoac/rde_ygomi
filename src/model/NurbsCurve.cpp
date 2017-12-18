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
    mControlPoints(std::make_shared<Point3DList>()),
    mKnots(std::make_shared<std::vector<double>>()),
    mPaintTotalLength(0.0),
    mLineLength(0.0)
{

}

Model::NurbsCurve::~NurbsCurve()
{

}

Model::Point3DListConstPtr Model::NurbsCurve::GetControlPoints() const
{
    return mControlPoints;
}

Model::Point3DListPtr Model::NurbsCurve::GetMutableControlPoints()
{
    return mControlPoints;
}

size_t Model::NurbsCurve::GetControlPointsSize() const
{
    return mControlPoints->size();
}

Model::Point3DPtr Model::NurbsCurve::GetControlPoint(const size_t& aIndex) const
{
    if (aIndex < mControlPoints->size())
    {
        return mControlPoints->at(aIndex);
    }

    return std::make_shared<Point3D>(DBL_NAN, DBL_NAN, DBL_NAN);
}

Model::KnotListConstPtr Model::NurbsCurve::GetKnots() const
{
    return mKnots;
}

Model::KnotListPtr Model::NurbsCurve::GetMutableKnots()
{
    return mKnots;
}

size_t Model::NurbsCurve::GetKnotsSize() const
{
    return mKnots->size();
}

double Model::NurbsCurve::GetKnot(const size_t& aIndex) const
{
    if (aIndex < mKnots->size())
    {
        return mKnots->at(aIndex);
    }

    return DBL_NAN;
}

const double& Model::NurbsCurve::GetPaintTotalLength() const
{
    return mPaintTotalLength;
}

void Model::NurbsCurve::SetPaintTotalLength(const double& aPaintTotalLength)
{
    mPaintTotalLength = aPaintTotalLength;
}

const double& Model::NurbsCurve::GetLineLength() const
{
    return mLineLength;
}

void Model::NurbsCurve::SetLineLength(const double& aLineLength)
{
    mLineLength = aLineLength;
}
