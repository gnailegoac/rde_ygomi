/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DouglasPeucker.cpp
 * @brief
 *******************************************************************************
 */
#include "DouglasPeucker.h"

#include <algorithm>

namespace
{
const double ZERO = 1e-6;
}

Model::DouglasPeucker::DouglasPeucker(const Point3DListPtr& aPointList):
    mPoints(aPointList)
{

}

Model::DouglasPeucker::~DouglasPeucker()
{

}

Model::Point3DListPtr Model::DouglasPeucker::Simplify(
                const double& aThreshold) const
{
    std::vector<Point3DPtr> simplified;
    std::vector<std::uint32_t> resultIndex;
    std::vector<std::pair<std::uint32_t, std::uint32_t> > periods;
    periods.push_back(std::make_pair(0, mPoints->size() - 1));
    while (!periods.empty())
    {
        auto line = periods.back();
        periods.pop_back();
        std::pair<std::uint32_t, double> point = getFarthestPoint(line.first,
                                                                  line.second);
        if (point.second >= aThreshold)
        {
            periods.push_back(std::make_pair(line.first, point.first));
            periods.push_back(std::make_pair(point.first, line.second));
        }
        else
        {
            resultIndex.push_back(line.first);
        }
    }
    resultIndex.push_back(mPoints->size() - 1);

    // sort index and get points
    std::sort(resultIndex.begin(), resultIndex.end(),
              [](const std::uint32_t& a, const std::uint32_t& b) -> bool
    {
        return a < b;
    });
    for (auto& idx : resultIndex)
    {
        simplified.push_back(mPoints->at(idx));
    }
    return std::make_shared<Point3DList>(simplified);
}

double Model::DouglasPeucker::pointToPointDistance(
                const Model::Point3DPtr& aLhs,
                const Model::Point3DPtr& aRhs) const
{
    return mod(std::make_shared<Point3D>(aLhs->GetX() - aRhs->GetX(),
                                         aLhs->GetY() - aRhs->GetY(),
                                         aLhs->GetZ() - aRhs->GetZ()));
}

double Model::DouglasPeucker::pointToLineDistance(
                const Model::Point3DPtr& aPoint,
                const Model::Point3DPtr& aL1,
                const Model::Point3DPtr& aL2) const
{
    Point3DPtr vecL1p = std::make_shared<Point3D>(aPoint->GetX() - aL1->GetX(),
                                                  aPoint->GetY() - aL1->GetY(),
                                                  aPoint->GetZ() - aL1->GetZ());
    Point3DPtr vecL2p = std::make_shared<Point3D>(aPoint->GetX() - aL2->GetX(),
                                                  aPoint->GetY() - aL2->GetY(),
                                                  aPoint->GetZ() - aL2->GetZ());
    Point3DPtr vecL12 = std::make_shared<Point3D>(aL2->GetX() - aL1->GetX(),
                                                  aL2->GetY() - aL1->GetY(),
                                                  aL2->GetZ() - aL1->GetZ());
    Point3DPtr vecL21 = std::make_shared<Point3D>(aL1->GetX() - aL2->GetX(),
                                                  aL1->GetY() - aL2->GetY(),
                                                  aL1->GetZ() - aL2->GetZ());
    if (innerProduct(vecL1p, vecL12) <= 0)
    {
        return mod(vecL1p);
    }
    else if (innerProduct(vecL2p, vecL21) <= 0)
    {
        return mod(vecL2p);
    }
    else
    {
        // Heron's formula
        double a = mod(vecL12);
        if (a < ZERO)
        {
            return mod(vecL1p);
        }
        double b = mod(vecL2p);
        double c = mod(vecL1p);
        double p = 0.5 * (a + b + c);
        return 2 * std::sqrt((p) * (p - a) * (p - b) * (p - c)) / a;
    }
}

double Model::DouglasPeucker::innerProduct(const Model::Point3DPtr& aLhs,
                                           const Model::Point3DPtr& aRhs) const
{
    return aLhs->GetX() * aRhs->GetX()
           + aLhs->GetY() * aRhs->GetY()
           + aLhs->GetZ() * aRhs->GetZ();
}

double Model::DouglasPeucker::mod(const Model::Point3DPtr& aVector) const
{
    return std::sqrt(aVector->GetX() * aVector->GetX()
                     + aVector->GetY() * aVector->GetY()
                     + aVector->GetZ() * aVector->GetZ());
}

std::pair<std::uint32_t, double> Model::DouglasPeucker::getFarthestPoint(
                std::uint32_t aBegin,
                std::uint32_t aEnd) const
{
    std::uint32_t idx = 0;
    double maxDistance = 0;
    for (std::uint32_t i = aBegin + 1; i < aEnd; ++i)
    {
        double distance = pointToLineDistance(mPoints->at(i),
                                              mPoints->at(aBegin),
                                              mPoints->at(aEnd));
        if (distance > maxDistance)
        {
            idx = i;
            maxDistance = distance;
        }
    }
    return std::make_pair(idx, maxDistance);
}
