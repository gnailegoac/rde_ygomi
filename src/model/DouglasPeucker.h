/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DouglasPeucker.h
 * @brief
 *******************************************************************************
 */
#pragma once

#include "Point3D.h"

namespace Model
{

class DouglasPeucker
{
public:
    DouglasPeucker(const Point3DListPtr& aPointList);
    ~DouglasPeucker();

    Point3DListPtr Simplify(const double& aThreshold) const;

private:
    double pointToPointDistance(const Point3DPtr& aLhs,
                                const Point3DPtr& aRhs) const;
    double pointToLineDistance(const Point3DPtr& aPoint,
                               const Point3DPtr& aL1,
                               const Point3DPtr& aL2) const;
    double innerProduct(const Point3DPtr& aLhs,
                        const Point3DPtr& aRhs) const;
    double mod(const Point3DPtr& aVector) const;
    std::pair<std::uint32_t, double> getFarthestPoint(
                    std::uint32_t aBegin,
                    std::uint32_t aEnd) const;

private:
    Point3DListPtr mPoints;
};

}
