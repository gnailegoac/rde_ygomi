/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    NurbsCurve.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include <QJsonArray>

#include "Common.h"
#include "Point3D.h"
#include "Curve.h"
#include "Poly.h"
#include "external/geo_alg.h"

namespace YGEO
{

class NURBS;

}

namespace Model
{

typedef std::shared_ptr<std::vector<double>> KnotListPtr;
typedef std::shared_ptr<const std::vector<double>> KnotListConstPtr;

class NurbsCurve : public Curve
{
public:
    NurbsCurve();
    ~NurbsCurve();

    Point3DListPtr CalculatePointCloud(const double& aSamplingInterval) override;
    PaintListPtr CalculatePaintPointCloud(const double& aSamplingInterval) override;

    bool Convert(const std::string& aStrExpr) override;
    std::string Parse() const override;
    bool FitPointsToCurve(std::string& aErrorMessage) override;

    const Point3DListPtr& GetControlPoints() const;
    Point3DListPtr GetMutableControlPoints();
    void SetControlPoints(const Point3DListPtr& aControlPoints);
    size_t GetControlPointsSize() const;
    Point3DPtr GetControlPoint(const size_t& aIndex) const;

    const KnotListPtr& GetKnots() const;
    KnotListPtr GetMutableKnots();
    void SetKnots(const KnotListPtr& aKnots);
    size_t GetKnotsSize() const;
    double GetKnot(const size_t& aIndex) const;

    const double& GetPaintTotalLength() const;
    void SetPaintTotalLength(const double& aPaintTotalLength);

    const double& GetLineLength() const;
    void SetLineLength(const double& aLineLength);

    void SetNurbs(const Point3DListPtr& aControlPoints, const KnotListPtr& aKnots);
    void CalculateLength();

private:
    void readNurbsFromJSON(const QJsonArray& aObjectCtrlPoints, const QJsonArray& aObjectKnots);
    void readPaintRangeFromJSON(const QJsonArray& aObjectPaintEndPoints);

    void writeNurbsToJSON(QJsonArray& aObjectCtrlPoints, QJsonArray& aObjectKnots) const;
    void writePaintRangeToJSON(QJsonArray& aObjectPaintEndPoints) const;

    void SetPoly3(const poly3_coef_t& aPol3Coef, Poly2Ptr aPoly2);
    void SetPoly3T(const poly3_curve_t& aPoly3Curve, Poly2XYZTPtr aPoly2);
    Poly2XYZTListPtr GetPaintEquationList(const size_t& index);
    Poly2XYZTListPtr GetEquationList();

private:
    Point3DListPtr mControlPoints;
    KnotListPtr mKnots;
    double mPaintTotalLength;
    double mLineLength;
    std::shared_ptr<YGEO::NURBS> mNurbs;
};

typedef std::shared_ptr<NurbsCurve> NurbsCurvePtr;
typedef std::shared_ptr<const NurbsCurve> NurbsCurveConstPtr;
typedef std::vector<NurbsCurvePtr> NurbsCurveList;
typedef std::shared_ptr<NurbsCurveList> NurbsCurveListPtr;
typedef std::shared_ptr<const NurbsCurveList> NurbsCurveListConstPtr;

}
