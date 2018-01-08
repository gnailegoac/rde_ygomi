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

class ColOfMatrix
{
public:
    ColOfMatrix();
    ~ColOfMatrix();

    std::int32_t GetStart() const;
    void SetStart(const std::int32_t& aStart);
    std::int32_t GetEnd() const;
    void SetEnd(const std::int32_t& aEnd);
    const std::shared_ptr<std::vector<double> >& GetData() const;
    std::shared_ptr<std::vector<double> >& GetData();
    void SetData(const std::shared_ptr<std::vector<double> >& aData);

private:
    std::int32_t mStart;
    std::int32_t mEnd;
    std::shared_ptr<std::vector<double>> mData;
};

class FitNurbs
{
public:
    FitNurbs();
    ~FitNurbs();

    ///------------------------------------------------------------------------
    /// Static function for fitting points to nurbs curve,
    ///   - input points might be solid or piecewise solid (dashed),
    ///   - return a shared pointer of class Model::NurbsCurve, with all data
    ///     members (length, paint length, paint range, knots...) calculated
    static std::shared_ptr<NurbsCurve> FitPointsToCurve(
                    const PaintListPtr& aInputPoints,
                    const std::uint8_t& aOrder,
                    std::string& aErrorInfo);

private:
    ///------------------------------------------------------------------------
    /// input fit data validation
    static bool checkFitData(
                    const PaintListPtr& aInputPoints);

    ///------------------------------------------------------------------------
    /// initialise parameters including u value, knots vector and paint range
    static bool initialise(
                    const PaintListPtr& aInputPoints,
                    const uint32_t& aDegree,
                    std::shared_ptr<std::vector<double>>& aKnots,
                    std::shared_ptr<std::vector<double>>& aU,
                    std::shared_ptr<std::vector<std::pair<double, double>>>& aPaintRange,
                    Point3DListPtr& aTotalPoints);

    ///------------------------------------------------------------------------
    /// generate u value of each input points based on chord length normalization
    static std::shared_ptr<std::vector<double> > generateU(
                    const Point3DListPtr& aPoints);

    ///------------------------------------------------------------------------
    /// referenced from roadDB Core::Common::NURBS
    /// find the corresponding knot span of the given u value
    static std::int32_t findSpan(
                    const int32_t& aHigh,
                    const int32_t& aLow,
                    const double& aU,
                    const std::shared_ptr<std::vector<double>>& aKnot);

    ///------------------------------------------------------------------------
    /// referenced from roadDB Core::Common::NURBS
    /// solve basis function with the given u value under certain degree
    static std::shared_ptr<std::vector<double> > basisFun(
                    const int32_t aSpan,
                    const double aU,
                    const int32_t& aDegree,
                    const std::vector<double>& aKnots);

    ///------------------------------------------------------------------------
    /// referenced from roadDB Core::Common::NURBS
    /// calculate matrix Ni,p(u)
    static std::shared_ptr<std::vector<ColOfMatrix> > solveMatrixN(
                    const int32_t& aNumControlPoints,
                    const std::shared_ptr<std::vector<double>>& aKnots,
                    const std::shared_ptr<std::vector<double>>& aU,
                    const int32_t& aDegree);

    ///-------------------------------------------------------------------------
    /// referenced from roadDB Core::Common::NURBS
    /// calculate matrix R = (N^T)P
    static std::shared_ptr<std::vector<Point3DPtr> > solveMatrixR(
                    const std::shared_ptr<std::vector<ColOfMatrix>>& aN,
                    const std::shared_ptr<std::vector<Point3DPtr>>& aP);

    ///-------------------------------------------------------------------------
    /// referenced from roadDB Core::Common::NURBS
    /// calculate control points based on N, R and input points
    static std::shared_ptr<std::vector<Point3DPtr> > solveControlPoints(
                    const std::shared_ptr<std::vector<Point3DPtr>>& aInputPoints,
                    const std::shared_ptr<std::vector<ColOfMatrix>>& aN,
                    const std::shared_ptr<std::vector<Point3DPtr>>& aR);

};
}
