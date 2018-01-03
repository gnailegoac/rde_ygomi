/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    FitNurbs.cpp
 * @brief
 *******************************************************************************
 */
#include "FitNurbs.h"

#include "external/nurbs.h"

#include "DouglasPeucker.h"
#include "NurbsCurve.h"


namespace
{
const std::int32_t ORDER = 3;
const std::int32_t MAX_NUM_POINTS = INT32_MAX;
const std::int32_t MIN_NUM_CONTROL_POINTS = 7;
const double MIN_CONTROL_POINTS_RATIO = 1. / 300;
const double MAX_CONTROL_POINTS_RATIO = 1. / 10;
const double TRANSLATION = 1.;
const double ERROR_THRESHOLD = 0.5;
}


Model::FitNurbs::FitNurbs()
{

}

Model::FitNurbs::~FitNurbs()
{

}

std::shared_ptr<Model::NurbsCurve> Model::FitNurbs::FitPoints(
                const PaintListPtr& aInputPoints)
{
    std::string errorInfo = "";
    if (!checkFitData(aInputPoints))
    {
        errorInfo = "Invalid input points";
        return nullptr;
    }
    std::shared_ptr<std::vector<double>> knots;
    std::shared_ptr<std::vector<double>> u;
    std::shared_ptr<std::vector<std::pair<double, double>>> paintRange;
    Point3DListPtr allPoints = std::make_shared<Point3DList>();
    if (initialise(aInputPoints, ORDER - 1, knots, u, paintRange, allPoints))
    {
        std::shared_ptr<std::vector<Point3DPtr> > controlPoints
            = getControlPoints(knots, u, allPoints, ORDER - 1);
        std::shared_ptr<NurbsCurve> result = std::make_shared<NurbsCurve>();
        result->GetMutableControlPoints() = controlPoints;
        result->GetMutableKnots() = knots;
        result->GetMutablePaintRange() = paintRange;
        result->SetNurbs(controlPoints, knots);
        double paintLength;
        double totalLength;
        result->CalculateLength(0.1, totalLength, paintLength);
        result->SetLineLength(totalLength);
        result->SetPaintTotalLength(paintLength);
        return result;
    }
    return nullptr;
}

bool Model::FitNurbs::checkFitData(const PaintListPtr& aInputPoints)
{
    std::int32_t pointCount = 0;
    for (auto& pointSet : *aInputPoints)
    {
        pointCount += pointSet->size();
    }
    if (pointCount < 2)
    {
        return false;
    }
    return true;
}

bool Model::FitNurbs::initialise(
                const Model::PaintListPtr& aInputPoints,
                const uint32_t& aDegree,
                std::shared_ptr<std::vector<double> >& aKnots,
                std::shared_ptr<std::vector<double> >& aU,
                std::shared_ptr<std::vector<std::pair<double, double> > >& aPaintRange,
                Point3DListPtr& aTotalPoints)
{
    std::size_t count = 0;
    for (auto& pointSet : *aInputPoints)
    {
        count += pointSet->size();
    }
    aTotalPoints->clear();
    aTotalPoints->reserve(count);
    std::vector<std::pair<uint32_t, uint32_t>> paintIndex;
    paintIndex.reserve(aInputPoints->size());
    for (const Point3DListPtr& pointSet : *aInputPoints)
    {
        paintIndex.push_back(std::make_pair(aTotalPoints->size(),
                                            aTotalPoints->size() + pointSet->size() - 1));
        aTotalPoints->insert(aTotalPoints->end(), pointSet->begin(), pointSet->end());
    }
    aU = generateU(aTotalPoints);
    // generate knots and paint range
    aKnots->clear();
    for (uint32_t i = 0; i < aDegree; ++i)
    {
        aKnots->push_back(0);
    }
    aPaintRange->clear();
    for (auto& index : paintIndex)
    {
        aPaintRange->push_back(std::make_pair(aU->at(index.first),
                                              aU->at(index.second)));
        std::shared_ptr<std::vector<uint32_t> > knotIndex
            = DouglasPeucker::GetSimplifyIndex(aTotalPoints, 0.1,
                                               index.first, index.second);
        for (const auto& idx : *knotIndex)
        {
            aKnots->push_back(aU->at(idx));
        }
    }
    for (uint32_t i = 0; i < aDegree; ++i)
    {
        aKnots->push_back(1);
    }
    return true;
}

std::shared_ptr<std::vector<double> > Model::FitNurbs::generateU(
                const Model::Point3DListPtr& aPoints)
{
    auto u = std::make_shared<std::vector<double>>();
    u->reserve(aPoints->size() - 1);
    double sumDistance = 0;
    /////////////////////////////////    u[0]??????
    for (std::size_t i = 0; i < aPoints->size() - 1; ++i)
    {
        sumDistance += pointDistance(aPoints->at(i), aPoints->at(i + 1));
        u->at(i) = sumDistance;
    }
    sumDistance = 1. / sumDistance;
    for (auto& tmp : *u)
    {
        tmp *= sumDistance;
    }
    return u;
}

double Model::FitNurbs::pointDistance(const Model::Point3DPtr& aLhs,
                                      const Model::Point3DPtr& aRhs)
{
    return std::sqrt((aLhs->GetX() - aRhs->GetX()) * (aLhs->GetX() - aRhs->GetX())
                     + (aLhs->GetY() - aRhs->GetY()) * (aLhs->GetY() - aRhs->GetY())
                     + (aLhs->GetZ() - aRhs->GetZ()) * (aLhs->GetZ() - aRhs->GetZ()));
}

std::shared_ptr<std::vector<Model::Point3DPtr> > Model::FitNurbs::getControlPoints(
                const std::shared_ptr<std::vector<double> >& aKnots,
                const std::shared_ptr<std::vector<double> >& aU,
                const std::shared_ptr<std::vector<Model::Point3DPtr> >& aInputPoints,
                const int32_t& aDegree)
{
    std::size_t numControlPoints = aKnots->size() - aDegree - 1;
    cv::Point3d cvControlPoints[numControlPoints];
    cv::Point3d cvInputPoints[aInputPoints->size()];
    double knots[aKnots->size()];
    double u[aU->size()];
    for (std::size_t i = 0; i < aInputPoints->size(); ++i)
    {
        cvInputPoints[i].x = aInputPoints->at(i)->GetX();
        cvInputPoints[i].y = aInputPoints->at(i)->GetY();
        cvInputPoints[i].z = aInputPoints->at(i)->GetZ();
    }
    for (std::size_t i = 0; i < aKnots->size(); ++i)
    {
        knots[i] = aKnots->at(i);
    }
    for (std::size_t i = 0; i < aU->size(); ++i)
    {
        u[i] = aU->at(i);
    }
    nurbs::NurbsCurveFit(numControlPoints - 1, aDegree, aInputPoints->size() - 1,
                         knots, u, cvInputPoints, cvControlPoints);
    auto result = std::make_shared<std::vector<Point3DPtr>>();
    result->reserve(numControlPoints);
    for (std::size_t i = 0; i < numControlPoints; ++i)
    {
        result->push_back(std::make_shared<Point3D>(cvControlPoints[i].x,
                                                    cvControlPoints[i].y,
                                                    cvControlPoints[i].z));
    }
    return result;
}
