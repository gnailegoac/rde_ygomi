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

#include "opencv2/opencv.hpp"
#include "DouglasPeucker.h"
#include "NurbsCurve.h"

Model::ColOfMatrix::ColOfMatrix():
    mStart(-1),
    mEnd(0),
    mData(std::make_shared<std::vector<double>>())
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

Model::FitNurbs::FitNurbs()
{

}

Model::FitNurbs::~FitNurbs()
{

}

std::shared_ptr<Model::NurbsCurve> Model::FitNurbs::FitPointsToCurve(
                const PaintListPtr& aInputPoints,
                const uint8_t& aOrder,
                std::string& aErrorInfo)
{
    if (!checkFitData(aInputPoints))
    {
        aErrorInfo = "Invalid input fitting points";
        return nullptr;
    }
    std::shared_ptr<std::vector<double> > knots
        = std::make_shared<std::vector<double>>();
    std::shared_ptr<std::vector<double> > u
        = std::make_shared<std::vector<double>>();
    std::shared_ptr<std::vector<std::pair<double, double>>> paintRange
        = std::make_shared<std::vector<std::pair<double, double>>>();
    Point3DListPtr allPoints = std::make_shared<Point3DList>();
    if (initialise(aInputPoints, aOrder - 1, knots, u, paintRange, allPoints))
    {
        std::uint32_t numControlPoints = knots->size() - aOrder;
        auto matrixN = solveMatrixN(numControlPoints, knots, u, aOrder - 1);
        if (matrixN == nullptr)
        {
            aErrorInfo = "Error while trying to solve matrix N";
            return nullptr;
        }
        auto matrixR = solveMatrixR(matrixN, allPoints);
        if (matrixR == nullptr)
        {
            aErrorInfo = "Error while trying to solve matrix R";
            return nullptr;
        }
        Point3DListPtr controlPoints = solveControlPoints(allPoints, matrixN, matrixR);
        if (controlPoints == nullptr)
        {
            aErrorInfo = "Error while trying to calculate control points";
            return nullptr;
        }
        std::shared_ptr<NurbsCurve> result = std::make_shared<NurbsCurve>();
        result->SetControlPoints(controlPoints);
        result->SetKnots(knots);
        result->SetPaintRange(paintRange);
        result->SetNurbs(controlPoints, knots);
        double paintLength;
        double totalLength;
        result->CalculateLength(0.1, totalLength, paintLength);
        result->SetLineLength(totalLength);
        result->SetPaintTotalLength(paintLength);
        return result;
    }
    else
    {
        aErrorInfo = "Error in parameters initializing";
        return nullptr;
    }
}

bool Model::FitNurbs::checkFitData(const PaintListPtr& aInputPoints)
{
    if (aInputPoints == nullptr)
    {
        return false;
    }
    std::int32_t pointCount = 0;
    for (auto& pointSet : *aInputPoints)
    {
        if (pointSet->size() < 2)
        {
            return false;
        }
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
    u->reserve(aPoints->size());
    double sumDistance = 0;
    u->push_back(sumDistance);
    for (std::size_t i = 0; i < aPoints->size() - 1; ++i)
    {
        sumDistance += Point3D::Distance(aPoints->at(i), aPoints->at(i + 1));
        u->push_back(sumDistance);
    }
    sumDistance = 1. / sumDistance;
    for (auto& tmp : *u)
    {
        tmp *= sumDistance;
    }
    return u;
}

int32_t Model::FitNurbs::findSpan(
                const int32_t& aHigh,
                const int32_t& aLow,
                const double& aU,
                const std::shared_ptr<std::vector<double> >& aKnot)
{
    if (aHigh < static_cast<int32_t>(aKnot->size()) && aLow <= aHigh)
    {
        if (aU < aKnot->at(aLow))
        {
            return aLow;
        }
        else if (aU > aKnot->at(aHigh))
        {
            return aHigh;
        }
        else
        {
            int32_t low = aLow;
            int32_t high = aHigh + 1;
            int32_t mid = low + (high - low) / 2; // mid = floor((high+low)/2)

            while (low + 1 < high) // +1 for low = high -1 case
            {
                if (aU - aKnot->at(mid) < -FLT_MIN)
                {
                    high = mid;
                }
                else if (aU - aKnot->at(mid + 1) > -FLT_MIN)
                {
                    low = mid;
                }
                else
                {
                    break;
                }
                mid = low + (high - low) / 2; // mid = floor((high+low)/2)
            }
            return mid;
        }
    }
    else
    {
        return -1;
    }
}

std::shared_ptr<std::vector<double> > Model::FitNurbs::basisFun(
                const int32_t aSpan,
                const double aU,
                const int32_t& aDegree,
                const std::vector<double>& aKnots)
{
    if (aKnots.empty())
    {
        return nullptr;
    }
    std::shared_ptr<std::vector<double> > vecN
        = std::make_shared<std::vector<double>>();
    vecN->reserve(aDegree + 1);
    vecN->push_back(1.0);
    std::vector<double> left(aDegree + 1, 0.0);
    std::vector<double> right(aDegree + 1, 0.0);
    double saved, tmp;
    for (int32_t j = 1; j <= aDegree; ++j)
    {
        left[j] = aU - aKnots[aSpan + 1 - j];
        right[j] = aKnots[aSpan + j] - aU;
        saved = 0.0;
        for (int32_t r = 0; r < j; ++r)
        {
            tmp = vecN->at(r) / (right[r + 1] + left[j - r]);
            vecN->at(r) = saved + right[r + 1] * tmp;
            saved = left[j - r] * tmp;
        }
        vecN->push_back(saved);
    }
    return vecN;
}

std::shared_ptr<std::vector<Model::ColOfMatrix> > Model::FitNurbs::solveMatrixN(
                const int32_t& aNumControlPoints,
                const std::shared_ptr<std::vector<double> >& aKnots,
                const std::shared_ptr<std::vector<double> >& aU,
                const int32_t& aDegree)
{
    std::int32_t numPoints = static_cast<int32_t>(aU->size());
    if(aNumControlPoints > numPoints || aKnots->empty() || aU->empty())
    {
        return nullptr;
    }
    std::shared_ptr<std::vector<ColOfMatrix> > matrixN
        = std::make_shared<std::vector<ColOfMatrix> >();
    matrixN->reserve(aNumControlPoints);
    for (std::int32_t i = 0; i < aNumControlPoints; ++i)
    {
        matrixN->push_back(ColOfMatrix());
    }

    std::int32_t span = aDegree;
    std::shared_ptr<std::vector<double> > vecN
        = std::make_shared<std::vector<double>>();      //It is a temporary variable
    for (int32_t row = 0; row < numPoints; ++row)
    {
        span = findSpan(aNumControlPoints - 1, span, aU->at(row), aKnots);
        if (span == -1)
        {
            return nullptr;
        }
        vecN = basisFun(span, aU->at(row), aDegree, *aKnots);
        if (vecN->empty())
        {
            return nullptr;
        }
        std::int32_t index = span - aDegree;
        for (std::int32_t p = 0; p <= aDegree; ++p)
        {
            if (matrixN->at(index + p).GetStart() == -1)
            {
                matrixN->at(index + p).SetStart(row);
            }
            matrixN->at(index + p).GetData()->push_back(vecN->at(p));
        }
    }
    for (auto& elem : *matrixN)
    {
        elem.SetEnd(elem.GetStart() + elem.GetData()->size());
    }

    return matrixN;
}

std::shared_ptr<std::vector<Model::Point3DPtr> > Model::FitNurbs::solveMatrixR(
                const std::shared_ptr<std::vector<Model::ColOfMatrix> >& aN,
                const std::shared_ptr<std::vector<Model::Point3DPtr> >& aP)
{
    if (aN->empty() || aP->empty())
    {
        return nullptr;
    }
    // It equal to the number of control Points. Every element of matrixN is a column of a matrix
    int32_t cols = static_cast<int32_t>(aN->size());
    if (cols < 2)
    {
        return nullptr;
    }
    int32_t rows = static_cast<int32_t>(aP->size());

    Point3DList matrixR;
    matrixR.reserve(rows);
    matrixR.push_back(std::make_shared<Point3D>());  // matrixR[0]

    ColOfMatrix firstCol = aN->front();
    ColOfMatrix lastCol = aN->back();
    double element1, element2;
    for (int32_t k = 1; k < rows - 1; ++k)
    {
        if (k < firstCol.GetStart() || k >= firstCol.GetEnd())
        {
            element1 = 0;
        }
        else
        {
            element1 = firstCol.GetData()->at(k - firstCol.GetStart());
        }

        if (k < lastCol.GetStart() || k >= lastCol.GetEnd())
        {
            element2 = 0;
        }
        else
        {
            element2 = lastCol.GetData()->at(k - lastCol.GetStart());
        }

        double x = aP->at(k)->GetX() - (element1 * aP->front()->GetX() + element2 * aP->back()->GetX());
        double y = aP->at(k)->GetY() - (element1 * aP->front()->GetY() + element2 * aP->back()->GetY());
        double z = aP->at(k)->GetZ() - (element1 * aP->front()->GetZ() + element2 * aP->back()->GetZ());
        matrixR.push_back(std::make_shared<Point3D>(x, y, z));
    }
    matrixR.push_back(std::make_shared<Point3D>());  // matrixR[rows - 1]

    std::vector<Point3DPtr> result;
    result.reserve(cols - 2);

    auto itN = aN->begin() + 1;
    int32_t startIdxN, endIdxN, startIdxR;
    for (int32_t i = 1; i < cols - 1; ++i)
    {
        if (itN->GetStart() < 1)
        {
            startIdxR = 1;
            startIdxN = 1 - itN->GetStart();
        }
        else
        {
            startIdxR = itN->GetStart();
            startIdxN = 0;
        }

        if (itN->GetEnd() > rows - 1)
        {
            endIdxN = rows - 1 - itN->GetStart();
        }
        else
        {
            endIdxN = itN->GetEnd() - itN->GetStart();
        }

        auto it = matrixR.begin() + startIdxR;

        Model::Point3DPtr tmpPoint = std::make_shared<Point3D>();
        for (int32_t j = startIdxN; j < endIdxN; ++j)
        {
            tmpPoint->SetX(tmpPoint->GetX() + itN->GetData()->at(j) * (*it)->GetX());
            tmpPoint->SetY(tmpPoint->GetY() + itN->GetData()->at(j) * (*it)->GetY());
            tmpPoint->SetZ(tmpPoint->GetZ() + itN->GetData()->at(j) * (*it)->GetZ());
            ++it;
        }
        result.emplace_back(tmpPoint);
        ++itN;
    }

    return std::make_shared<Point3DList>(result);
}

std::shared_ptr<std::vector<Model::Point3DPtr> > Model::FitNurbs::solveControlPoints(
                const std::shared_ptr<std::vector<Model::Point3DPtr> >& aInputPoints,
                const std::shared_ptr<std::vector<Model::ColOfMatrix> >& aN,
                const std::shared_ptr<std::vector<Model::Point3DPtr> >& aR)
{
    //matrixN represent a matrix
    if (aInputPoints->empty() || aN->empty() || aR->empty())
    {
        return nullptr;
    }

    int32_t cols = static_cast<int32_t>(aN->size());  //Every element of matrixN is a column of a matrix
    if (cols - 2 != static_cast<int32_t>(aR->size()))
    {
        return nullptr;
    }

    //calculate (matrixN^T * matrixN)
    cv::Mat squareMatrix = cv::Mat::zeros(cols, cols, CV_64F); // squareMatrix = matrixN' * matrixN
    auto iter = aN->begin();
    for (int32_t r = 0; r < cols; ++r)  // square matrix
    {
        auto p = squareMatrix.ptr<double>(r);
        double tmp(0);
        for (auto itCol = iter->GetData()->begin(); itCol < iter->GetData()->end(); ++itCol)
        {
            tmp += (*itCol) * (*itCol);
        }
        p[r] = tmp * 0.5f; // because it calculate a half of the matrix only, this function will sum this matrix and it's transposition
        auto itc = iter + 1;

        for (int32_t c = r + 1; c < cols; ++c) // square matrix
        {
            auto length = iter->GetEnd() - itc->GetStart();
            if (length <= 0)
            {
                break;
            }
            else if (length > static_cast<int32_t>(itc->GetData()->size()))
            {
                length = itc->GetData()->size();
            }
            else
            {
            }

            tmp = 0;
            auto index = itc->GetStart() - iter->GetStart();
            for (int32_t i = 0; i < length; ++i)
            {
                tmp += iter->GetData()->at(index) * itc->GetData()->at(i);
                ++index;
            }

            p[c] = tmp;
            ++itc;
        }
        ++iter;
    }

    squareMatrix = squareMatrix + squareMatrix.t();

    cv::Rect center(1, 1, cols - 2, cols - 2);
    cv::Mat sMatrix = squareMatrix(center);

    //calculate (matrixN^T * matrixN)
    sMatrix = sMatrix.inv(cv::DECOMP_LU);

    //calculate control points. control points = squareMatrix*R
    Point3DList controlPoints;
    controlPoints.reserve(cols);
    cols = sMatrix.cols;

    //the first control point is the first point of the curve
    controlPoints.emplace_back(aInputPoints->front());
    for (int32_t i = 0; i < cols; ++i)
    {
        Point3D tmpPoint(0, 0, 0);
        double* q = sMatrix.ptr<double>(i);
        auto itR = aR->begin();
        for (int32_t j = 0; j < cols; ++j)
        {
            const auto& element = q[j];
            tmpPoint.SetX(tmpPoint.GetX() + (*itR)->GetX() * element);
            tmpPoint.SetY(tmpPoint.GetY() + (*itR)->GetY() * element);
            tmpPoint.SetZ(tmpPoint.GetZ() + (*itR)->GetZ() * element);
            ++itR;
        }
        controlPoints.emplace_back(std::make_shared<Point3D>(tmpPoint));
    }
    //the last control point is the last point of the curve
    controlPoints.emplace_back(aInputPoints->back());

    return std::make_shared<Point3DList>(controlPoints);
}
