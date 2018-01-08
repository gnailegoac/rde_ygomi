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
    DouglasPeucker();
    ~DouglasPeucker();

    static Point3DListPtr Simplify(const Point3DListPtr& aPoints,
                                   const double& aThreshold);

    static std::shared_ptr<std::vector<uint32_t> > GetSimplifyIndex(
                    const Point3DListPtr& aPoints,
                    const double& aThreshold,
                    const std::uint32_t& aStartIndex,
                    const std::uint32_t& aEndIndex);

private:
    static double pointToPointDistance(const Point3DPtr& aLhs,
                                       const Point3DPtr& aRhs);
    static double pointToLineDistance(const Point3DPtr& aPoint,
                                      const Point3DPtr& aL1,
                                      const Point3DPtr& aL2);
    static double innerProduct(const Point3DPtr& aLhs,
                               const Point3DPtr& aRhs);
    static double mod(const Point3DPtr& aVector);
    static std::pair<std::uint32_t, double> getFarthestPoint(
                    const Point3DListPtr& aPoints,
                    std::uint32_t aBegin,
                    std::uint32_t aEnd);
};

}
