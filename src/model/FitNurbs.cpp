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

//int32_t Model::FitNurbs::findSpan(
//                const int32_t& aHigh,
//                const int32_t& aLow,
//                const double& aU,
//                const std::shared_ptr<std::vector<double> >& aKnot)
//{
//    if (aHigh < static_cast<int32_t>(aKnot.size()) && aLow <= aHigh)
//    {
//        if (aU < knot[aLow])
//        {
//            return aLow;
//        }
//        else if (aU > knot[aHigh])
//        {
//            return aHigh;
//        }
//        else
//        {
//            int32_t low = aLow;
//            int32_t high = aHigh + 1;
//            int32_t mid = low + (high - low) / 2; // mid = floor((high+low)/2)

//            while (low + 1 < high) // +1 for low = high -1 case
//            {
//                if (aU - knot[mid] < -FLT_MIN)
//                {
//                    high = mid;
//                }
//                else if (aU - knot[mid + 1] > -FLT_MIN)
//                {
//                    low = mid;
//                }
//                else
//                {
//                    break;
//                }
//                mid = low + (high - low) / 2; // mid = floor((high+low)/2)
//            }
//            return mid;
//        }
//    }
//    else
//    {
//        return -1;
//    }
//}

//std::shared_ptr<std::vector<double> > Model::FitNurbs::basisFun(
//                const int32_t aSpan,
//                const double aU,
//                const int32_t& aDegree,
//                const std::vector<double>& aKnots)
//{
//    if (aKnots.empty())
//    {
//        return nullptr;
//    }
//    std::shared_ptr<std::vector<double> > nVec
//        = std::make_shared<std::vector<double>>();
//    nVec->reserve(aDegree + 1);
//    nVec->at(0) = 1.0;
//    std::vector<double> left(aDegree + 1, 0.0);
//    std::vector<double> right(aDegree + 1, 0.0);
//    double saved, tmp;
//    for (int32_t j = 1; j <= aDegree; ++j)
//    {
//        left[j] = aU - aKnots[aSpan + 1 - j];
//        right[j] = aKnots[aSpan + j] - aU;
//        saved = 0.0;
//        for (int32_t r = 0; r < j; ++r)
//        {
//            tmp = nVec->at(r) / (right[r + 1] + left[j - r]);
//            nVec->at(r) = saved + right[r + 1] * tmp;
//            saved = left[j - r] * tmp;
//        }
//        nVec->at(j) = saved;
//    }
//    return nVec;
//}

//std::shared_ptr<std::vector<Model::ColOfMatrix> > Model::FitNurbs::solveMatrixN(
//                const int32_t& aNumControlPoints,
//                const std::shared_ptr<std::vector<double> >& aKnots,
//                const std::shared_ptr<std::vector<double> >& aU)
//{
//    std::int32_t numPoints = static_cast<int32_t>(aU.size());
//    if(aNumControlPoints > numPoints || aKnots.empty() || aU.empty())
//    {
//        // SDOR_LOG_ERROR << "The arguments is error in function solveMatrixN.";
//        return nullptr;
//    }
//    std::int32_t degree = ORDER - 1;
//    std::shared_ptr<std::vector<ColOfMatrix> > matrixN
//        = std::make_shared<std::vector<ColOfMatrix> >();
//    matrixN->reserve(aNumControlPoints);
//    for (auto& elem : *matrixN)
//    {
//        elem = ColOfMatrix();
//    }

//    std::int32_t span = degree;
//    std::shared_ptr<std::vector<double>> vecN;      //It is a temporary variable
//    for (int32_t row = 0; row < numPoints; ++row)
//    {
//        span = findSpan(aNumControlPoints - 1, span, aU->at(row), aKnots);
//        if (span == nullptr)
//        {
//            return nullptr;
//        }
//        vecN = basisFun(span, aU[row], degree, aKnots);
//        if (vecN.empty())
//        {
//            return nullptr;
//        }
//        std::int32_t index = span - degree;
//        for (std::int32_t p = 0; p <= degree; ++p)
//        {
//            if (matrixN->at(index + p).GetStart() == -1)
//            {
//                matrixN->at(index + p).SetStart(row);
//            }
//            matrixN->at(index + p).GetData()->push_back(vecN->at(p));
//        }
//    }
//    for (auto& elem : *matrixN)
//    {
//        elem.SetEnd(elem.GetStart() + elem.GetData()->size());
//    }

//    return matrixN;
//}

//std::shared_ptr<std::vector<Model::Point3DPtr> > Model::FitNurbs::solveMatrixR(
//                const std::shared_ptr<std::vector<Model::ColOfMatrix> >& aN,
//                const std::shared_ptr<std::vector<Model::Point3DPtr> >& aP)
//{
//    if (aN->empty() || aP->empty())
//    {
//        return nullptr;
//    }
//}

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

Model::ColOfMatrix::ColOfMatrix():
    mStart(-1),
    mEnd(0)
{

}

Model::ColOfMatrix::~ColOfMatrix()
{

}

int32_t Model::ColOfMatrix::GetStart() const
{
    return mStart;
}

void Model::ColOfMatrix::SetStart(const int32_t& aStart)
{
    mStart = aStart;
}

int32_t Model::ColOfMatrix::GetEnd() const
{
    return mEnd;
}

void Model::ColOfMatrix::SetEnd(const int32_t& aEnd)
{
    mEnd = aEnd;
}

const std::shared_ptr<std::vector<double> >& Model::ColOfMatrix::GetData() const
{
    return mData;
}

std::shared_ptr<std::vector<double> >& Model::ColOfMatrix::GetData()
{
    return mData;
}

void Model::ColOfMatrix::SetData(const std::shared_ptr<std::vector<double> >& aData)
{
    mData = aData;
}
