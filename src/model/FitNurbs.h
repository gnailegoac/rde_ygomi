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
    static std::shared_ptr<NurbsCurve> FitPointsToCurve(
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

    //    static std::shared_ptr<std::vector<Point3DPtr> > getControlPoints(
    //                    const std::shared_ptr<std::vector<double>>& aKnots,
    //                    const std::shared_ptr<std::vector<double>>& aU,
    //                    const std::shared_ptr<std::vector<Point3DPtr>>& aInputPoints,
    //                    const std::int32_t& aDegree);

    //    static std::shared_ptr<std::vector<Point3DPtr> > getControlPoints2D(
    //                    const std::shared_ptr<std::vector<double>>& aU,
    //                    const std::shared_ptr<std::vector<Point3DPtr>>& aInputPoints,
    //                    const std::int32_t& aDegree);

    //    static std::shared_ptr<std::vector<Point3DPtr> > controlPoints(
    //                    const std::shared_ptr<std::vector<Point3DPtr>>& aInputPoints,
    //                    const std::int32_t& aDegree);

    // referenced from roadDB Core::Common::NURBS
    static std::int32_t findSpan(const int32_t& aHigh,
                                 const int32_t& aLow,
                                 const double& aU,
                                 const std::shared_ptr<std::vector<double>>& aKnot);

    // referenced from roadDB Core::Common::NURBS
    static std::shared_ptr<std::vector<double> > basisFun(
                    const int32_t aSpan,
                    const double aU,
                    const int32_t& aDegree,
                    const std::vector<double>& aKnots);

    // solve matrix N
    static std::shared_ptr<std::vector<ColOfMatrix> > solveMatrixN(
                    const int32_t& aNumControlPoints,
                    const std::shared_ptr<std::vector<double>>& aKnots,
                    const std::shared_ptr<std::vector<double>>& aU);

    ///-------------------------------------------------------------------------
    /// R = (N^T)P
    static std::shared_ptr<std::vector<Point3DPtr> > solveMatrixR(
                    const std::shared_ptr<std::vector<ColOfMatrix>>& aN,
                    const std::shared_ptr<std::vector<Point3DPtr>>& aP);

};
}
