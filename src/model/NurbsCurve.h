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

namespace Model
{

typedef std::shared_ptr<std::vector<double>> KnotListPtr;
typedef std::shared_ptr<const std::vector<double>> KnotListConstPtr;

class NurbsCurve : public Curve
{
public:
    NurbsCurve();
    ~NurbsCurve();

    Point3DListConstPtr GetControlPoints() const;
    Point3DListPtr GetMutableControlPoints();
    size_t GetControlPointsSize() const;
    Point3DPtr GetControlPoint(size_t aIndex) const;

    KnotListConstPtr GetKnots() const;
    KnotListPtr GetMutableKnots();
    size_t GetKnotsSize() const;
    double GetKnot(size_t aIndex) const;

    double GetPaintTotalLength() const;
    void SetPaintTotalLength(double aPaintTotalLength);

    double GetLineLength() const;
    void SetLineLength(double aLineLength);

private:
    Point3DListPtr mControlPoints;
    KnotListPtr mKnots;
    double mPaintTotalLength;
    double mLineLength;
};

typedef std::shared_ptr<NurbsCurve> NurbsCurvePtr;
typedef std::shared_ptr<const NurbsCurve> NurbsCurveConstPtr;
typedef std::vector<NurbsCurvePtr> NurbsCurveList;
typedef std::shared_ptr<NurbsCurveList> NurbsCurveListPtr;
typedef std::shared_ptr<const NurbsCurveList> NurbsCurveListConstPtr;

}
