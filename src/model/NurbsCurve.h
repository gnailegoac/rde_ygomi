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

    const Point3DListPtr& GetControlPoints() const;
    Point3DListPtr GetMutableControlPoints();
    size_t GetControlPointsSize() const;
    Point3DPtr GetControlPoint(const size_t& aIndex) const;

    const KnotListPtr& GetKnots() const;
    KnotListPtr GetMutableKnots();
    size_t GetKnotsSize() const;
    double GetKnot(const size_t& aIndex) const;

    const double& GetPaintTotalLength() const;
    void SetPaintTotalLength(const double& aPaintTotalLength);

    const double& GetLineLength() const;
    void SetLineLength(const double& aLineLength);

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
