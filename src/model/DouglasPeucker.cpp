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

Model::DouglasPeucker::DouglasPeucker()
{

}

Model::DouglasPeucker::~DouglasPeucker()
{

}

Model::Point3DListPtr Model::DouglasPeucker::Simplify(
                const Point3DListPtr& aPoints,
                const double& aThreshold)
{
    std::vector<Point3DPtr> simplified;
    auto index = GetSimplifyIndex(aPoints, aThreshold, 0, aPoints->size() - 1);
    for (auto& idx : *index)
    {
        simplified.push_back(aPoints->at(idx));
    }
    return std::make_shared<Point3DList>(simplified);
}

std::shared_ptr<std::vector<uint32_t>> Model::DouglasPeucker::GetSimplifyIndex(
                                                    const Model::Point3DListPtr& aPoints,
                                                    const double& aThreshold,
                                                    const uint32_t& aStartIndex,
                                                    const uint32_t& aEndIndex)
{
    std::vector<std::uint32_t> resultIndex;
    if (aStartIndex >= aEndIndex || aEndIndex >= aPoints->size())
    {
        return std::make_shared<std::vector<uint32_t>>(resultIndex);
    }
    std::vector<std::pair<std::uint32_t, std::uint32_t> > periods;
    periods.push_back(std::make_pair(aStartIndex, aEndIndex));
    while (!periods.empty())
    {
        auto line = periods.back();
        periods.pop_back();
        std::pair<std::uint32_t, double> point = getFarthestPoint(aPoints,
                                                                  line.first,
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
    resultIndex.push_back(aEndIndex);

    // sort index and get points
    std::sort(resultIndex.begin(), resultIndex.end(),
              [](const std::uint32_t& a, const std::uint32_t& b) -> bool
    {
        return a < b;
    });
    return std::make_shared<std::vector<uint32_t>>(resultIndex);
}

double Model::DouglasPeucker::pointToPointDistance(
                const Model::Point3DPtr& aLhs,
                const Model::Point3DPtr& aRhs)
{
    return mod(std::make_shared<Point3D>(aLhs->GetX() - aRhs->GetX(),
                                         aLhs->GetY() - aRhs->GetY(),
                                         aLhs->GetZ() - aRhs->GetZ()));
}

///----------------------------------------------------------------------------
/// point to line distance is calculated based on triangle area calculation,
/// where relevant parameters are listed in the following figure
///                                    p
///                                   @
///                                  *   *
///                                 *       *
///                                *           *
///                               *               *
///                           L1 @*******************@ L2
double Model::DouglasPeucker::pointToLineDistance(
                const Model::Point3DPtr& aPoint,
                const Model::Point3DPtr& aL1,
                const Model::Point3DPtr& aL2)
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
        double lengthL12 = mod(vecL12);
        if (lengthL12 < ZERO)
        {
            return mod(vecL1p);
        }
        double lengthL2p = mod(vecL2p);
        double lengthL1p = mod(vecL1p);
        double semiPerimeter = 0.5 * (lengthL12 + lengthL2p + lengthL1p);
        return 2 * std::sqrt((semiPerimeter)
                             * (semiPerimeter - lengthL12)
                             * (semiPerimeter - lengthL2p)
                             * (semiPerimeter - lengthL1p)) / lengthL12;
    }
}

double Model::DouglasPeucker::innerProduct(const Model::Point3DPtr& aLhs,
                                           const Model::Point3DPtr& aRhs)
{
    return aLhs->GetX() * aRhs->GetX()
           + aLhs->GetY() * aRhs->GetY()
           + aLhs->GetZ() * aRhs->GetZ();
}

double Model::DouglasPeucker::mod(const Model::Point3DPtr& aVector)
{
    return std::sqrt(aVector->GetX() * aVector->GetX()
                     + aVector->GetY() * aVector->GetY()
                     + aVector->GetZ() * aVector->GetZ());
}

std::pair<std::uint32_t, double> Model::DouglasPeucker::getFarthestPoint(
                const Point3DListPtr& aPoints,
                std::uint32_t aBegin,
                std::uint32_t aEnd)
{
    std::uint32_t idx = 0;
    double maxDistance = 0;
    for (std::uint32_t i = aBegin + 1; i < aEnd; ++i)
    {
        double distance = pointToLineDistance(aPoints->at(i),
                                              aPoints->at(aBegin),
                                              aPoints->at(aEnd));
        if (distance > maxDistance)
        {
            idx = i;
            maxDistance = distance;
        }
    }
    return std::make_pair(idx, maxDistance);
}

Model::Point3DPtr Model::DouglasPeucker::crossProduct(
                const Model::Point3DPtr& aLhs,
                const Model::Point3DPtr& aRhs)
{
    return  std::make_shared<Point3D>(aLhs->GetY() * aRhs->GetZ() - aLhs->GetZ() * aRhs->GetY(),
                                      aLhs->GetZ() * aRhs->GetX() - aLhs->GetX() * aRhs->GetZ(),
                                      aLhs->GetX() * aRhs->GetY() - aLhs->GetY() * aRhs->GetX());
}

double Model::DouglasPeucker::pointToPointDistanceSquare(
                const Model::Point3DPtr& aLhs,
                const Model::Point3DPtr& aRhs)
{
    return modSquare(std::make_shared<Point3D>(aLhs->GetX() - aRhs->GetX(),
                                               aLhs->GetY() - aRhs->GetY(),
                                               aLhs->GetZ() - aRhs->GetZ()));
}

double Model::DouglasPeucker::pointToLineDistanceSquare(
                const Model::Point3DPtr& aPoint,
                const Model::Point3DPtr& aL1,
                const Model::Point3DPtr& aL2)
{
    Point3DPtr vecL1p = std::make_shared<Point3D>(aPoint->GetX() - aL1->GetX(),
                                                  aPoint->GetY() - aL1->GetY(),
                                                  aPoint->GetZ() - aL1->GetZ());
    Point3DPtr vecL12 = std::make_shared<Point3D>(aL2->GetX() - aL1->GetX(),
                                                  aL2->GetY() - aL1->GetY(),
                                                  aL2->GetZ() - aL1->GetZ());
    double hSquare = modSquare(vecL12);
    if(hSquare < ZERO)
    {
        return 0.0;
    }
    return modSquare(crossProduct(vecL1p, vecL12)) / hSquare;
}

double Model::DouglasPeucker::modSquare(const Model::Point3DPtr& aVector)
{
    return aVector->GetX() * aVector->GetX()
           + aVector->GetY() * aVector->GetY()
           + aVector->GetZ() * aVector->GetZ();
}

std::pair<uint32_t, double> Model::DouglasPeucker::getFarthestPointS(
                const Model::Point3DListPtr& aPoints,
                uint32_t aBegin,
                uint32_t aEnd)
{
    std::uint32_t idx = 0;
    double maxDistance = 0;
    for (std::uint32_t i = aBegin + 1; i < aEnd; ++i)
    {
        double distance = pointToLineDistanceSquare(aPoints->at(i),
                                                    aPoints->at(aBegin),
                                                    aPoints->at(aEnd));
        if (distance > maxDistance)
        {
            idx = i;
            maxDistance = distance;
        }
    }
    return std::make_pair(idx, maxDistance);
}

Model::Point3DListPtr Model::DouglasPeucker::SimplifyS(
                const Model::Point3DListPtr& aPoints,
                const double& aThreshold)
{
    std::vector<Point3DPtr> simplified;
    double threshold = aThreshold * aThreshold;
    auto index = GetSimplifyIndexS(aPoints, threshold, 0, aPoints->size() - 1);
    for (auto& idx : *index)
    {
        simplified.push_back(aPoints->at(idx));
    }
    return std::make_shared<Point3DList>(simplified);
}

std::shared_ptr<std::vector<uint32_t>> Model::DouglasPeucker::GetSimplifyIndexS(
                                                    const Model::Point3DListPtr& aPoints,
                                                    const double& aThreshold,
                                                    const uint32_t& aStartIndex,
                                                    const uint32_t& aEndIndex)
{
    std::vector<std::uint32_t> resultIndex;
    if (aStartIndex >= aEndIndex || aEndIndex >= aPoints->size())
    {
        return std::make_shared<std::vector<uint32_t>>(resultIndex);
    }
    std::vector<std::pair<std::uint32_t, std::uint32_t> > periods;
    periods.push_back(std::make_pair(aStartIndex, aEndIndex));
    while (!periods.empty())
    {
        auto line = periods.back();
        periods.pop_back();
        std::pair<std::uint32_t, double> point = getFarthestPointS(aPoints,
                                                                   line.first,
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
    resultIndex.push_back(aEndIndex);

    // sort index and get points
    std::sort(resultIndex.begin(), resultIndex.end(),
              [](const std::uint32_t& a, const std::uint32_t& b) -> bool
    {
        return a < b;
    });
    return std::make_shared<std::vector<uint32_t>>(resultIndex);
}
