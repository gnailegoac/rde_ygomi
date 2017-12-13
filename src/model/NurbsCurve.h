/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    NurbsCurve.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Common.h"
#include "Point3D.h"
#include "Curve.h"

class NurbsCurve : public Curve
{
public:
    NurbsCurve();
    NurbsCurve(std::vector<Point3D>& aControlPoints,
               std::vector<double>& aKnots,
               double aPaintTotalLength,
               double aLineLength);
    ~NurbsCurve();

    const std::vector<Point3D> GetControlPoints() const;
    std::vector<Point3D>* GetMutableControlPoints();
    size_t GetControlPointsSize() const;
    Point3D GetControlPoint(size_t aIndex) const;

    const std::vector<double> GetKnots() const;
    std::vector<double>* GetMutableKnots();
    size_t GetKnotsSize() const;
    double GetKnot(size_t aIndex) const;

    double GetPaintTotalLength() const;
    void SetPaintTotalLength(double aPaintTotalLength);

    double GetLineLength() const;
    void SetLineLength(double aLineLength);

private:
    std::vector<Point3D> mControlPoints;
    std::vector<double> mKnots;
    double mPaintTotalLength;
    double mLineLength;
};
