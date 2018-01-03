/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    FitNurbs.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Point3D.h"

namespace Model
{

class NurbsCurve;

class FitNurbs
{
public:
    FitNurbs();
    ~FitNurbs();
    static std::shared_ptr<NurbsCurve> FitPoints(
                    const PaintListPtr& aInputPoints);

private:
    static bool checkFitData(
                    const PaintListPtr& aInputPoints);

    // genearte u, knots and paint range
    static bool initialise(
                    const PaintListPtr& aInputPoints,
                    const uint32_t& aDegree,
                    std::shared_ptr<std::vector<double>>& aKnots,
                    std::shared_ptr<std::vector<double>>& aU,
                    std::shared_ptr<std::vector<std::pair<double, double>>>& aPaintRange,
                    Point3DListPtr& aTotalPoints);

    static std::shared_ptr<std::vector<double> > generateU(
                    const Point3DListPtr& aPoints);

    static double pointDistance(const Point3DPtr& aLhs,
                                const Point3DPtr& aRhs);

    static std::shared_ptr<std::vector<Point3DPtr> > getControlPoints(
                    const std::shared_ptr<std::vector<double>>& aKnots,
                    const std::shared_ptr<std::vector<double>>& aU,
                    const std::shared_ptr<std::vector<Point3DPtr>>& aInputPoints,
                    const std::int32_t& aDegree);



};
}
