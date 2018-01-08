/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    NurbsCurve.cpp
 * @brief
 *******************************************************************************
 */

#include "NurbsCurve.h"

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>
#include <QJsonObject>

#include "Utilities.h"
#include "external/geo_alg.h"

Model::NurbsCurve::NurbsCurve():
    Curve(),
    mControlPoints(std::make_shared<Point3DList>()),
    mKnots(std::make_shared<std::vector<double>>()),
    mPaintTotalLength(0.0),
    mLineLength(0.0),
    mNurbs(nullptr)
{

}

Model::NurbsCurve::~NurbsCurve()
{

}
Model::Point3DListPtr Model::NurbsCurve::CalculatePointCloud(
                const double& aSamplingInterval)
{
    Point3DListPtr result = std::make_shared<Point3DList>();
    Model::Poly2XYZTListPtr polys = GetEquationList();

    for (const Poly2XYZTPtr& poly : *polys)
    {
        double startKnot = poly->GetStart();
        double endKnot = poly->GetEnd();
        double length = mNurbs->GetLength(startKnot, endKnot);
        size_t sampleNumber = length / aSamplingInterval;
        sampleNumber = (sampleNumber < 2) ? 2 : sampleNumber;
        double knotInterval = (endKnot - startKnot) / sampleNumber;

        if (std::fabs(startKnot - endKnot) < 1e-10)
        {
            continue;
        }

        for (double t = startKnot; t <= endKnot + 1e-10; t += knotInterval)
        {
            t = std::fabs(t - endKnot) < knotInterval / 2.0 ? endKnot : t;
            Point3DPtr point = poly->GetPosition(t);
            result->push_back(point);
        }
    }

    return result;
}

Model::PaintListPtr Model::NurbsCurve::CalculatePaintPointCloud(
                const double& aSamplingInterval)
{
    PaintListPtr result = std::make_shared<PaintList>();
    const size_t paintRangeSize = mPaintRange->size();

    for (size_t i = 0; i < paintRangeSize; i++)
    {
        Model::Poly2XYZTListPtr polys = GetPaintEquationList(i);
        Point3DListPtr paint = std::make_shared<Point3DList>();

        for (const Poly2XYZTPtr& poly : *polys)
        {
            double startKnot = poly->GetStart();
            double endKnot = poly->GetEnd();
            double length = mNurbs->GetLength(startKnot, endKnot);
            size_t sampleNumber = length / aSamplingInterval;
            sampleNumber = (sampleNumber < 2) ? 2 : sampleNumber;
            double knotInterval = (endKnot - startKnot) / sampleNumber;

            if (std::fabs(startKnot - endKnot) < 1e-10)
            {
                continue;
            }

            for (double t = startKnot; t <= endKnot + 1e-10; t += knotInterval)
            {
                t = std::fabs(t - endKnot) < knotInterval / 2.0 ? endKnot : t;
                Point3DPtr point = poly->GetPosition(t);
                paint->push_back(point);
            }

        }

        if (1 < paint->size())
        {
            result->push_back(paint);
        }
    }

    return result;
}

bool Model::NurbsCurve::Convert(const std::string& aStrExpr)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(aStrExpr).toUtf8(), &err);

    if (doc.isNull() || err.error != QJsonParseError::NoError)
    {
        return false;
    }

    QJsonObject object = doc.object();

    if (!object.contains(QString("ControlPoints")) ||
            !object.contains(QString("PaintEndPoints")) ||
            !object.contains(QString("Knots")))
    {
        return false;
    }

    QJsonArray objectCtrlPoints = object.value(QString("ControlPoints")).toArray();
    QJsonArray objectPaintEndPoints = object.value(QString("PaintEndPoints")).toArray();
    QJsonArray objectKnots = object.value(QString("Knots")).toArray();

    if (0 == objectCtrlPoints.size() || 0 == objectPaintEndPoints.size())
    {
        return false;
    }

    readNurbsFromJSON(objectCtrlPoints, objectKnots);
    readPaintRangeFromJSON(objectPaintEndPoints);

    QJsonArray objectPaintTotalLength = object.value(QString("PaintTotalLength")).toArray();
    QJsonArray objectLineLength = object.value(QString("LineLength")).toArray();

    if (1 != objectPaintTotalLength.size() || 1 != objectPaintTotalLength.size())
    {
        return false;
    }

    mPaintTotalLength = objectPaintTotalLength.at(0).toString().toDouble();
    mLineLength = objectLineLength.at(0).toString().toDouble();

    return true;
}

std::string Model::NurbsCurve::Parse() const
{
    QJsonArray objectCtrlPoints;
    QJsonArray objectPaintEndPoints;
    QJsonArray objectKnots;

    writeNurbsToJSON(objectCtrlPoints, objectKnots);
    writePaintRangeToJSON(objectPaintEndPoints);

    QJsonArray objectPaintTotalLength;
    QJsonArray objectLineLength;

    std::string paintTotalLength = strings::FormatFloat<double>(mPaintTotalLength, 10);
    objectPaintTotalLength.append(QJsonValue(QString::fromStdString(paintTotalLength)));

    std::string lineLength = strings::FormatFloat<double>(mLineLength, 10);
    objectLineLength.append(QJsonValue(QString::fromStdString(lineLength)));

    QJsonObject object;
    object.insert("ControlPoints", objectCtrlPoints);
    object.insert("PaintEndPoints", objectPaintEndPoints);
    object.insert("Knots", objectKnots);
    object.insert("PaintTotalLength", objectPaintTotalLength);
    object.insert("LineLength", objectLineLength);

    QJsonDocument document;
    document.setObject(object);
    QString strJson(document.toJson(QJsonDocument::Indented));

    return strJson.toStdString();
}

const Model::Point3DListPtr& Model::NurbsCurve::GetControlPoints() const
{
    return mControlPoints;
}

Model::Point3DListPtr Model::NurbsCurve::GetMutableControlPoints()
{
    return mControlPoints;
}

void Model::NurbsCurve::SetControlPoints(const Model::Point3DListPtr& aControlPoints)
{
    mControlPoints = aControlPoints;
}

size_t Model::NurbsCurve::GetControlPointsSize() const
{
    return mControlPoints->size();
}

Model::Point3DPtr Model::NurbsCurve::GetControlPoint(const size_t& aIndex) const
{
    if (aIndex < mControlPoints->size())
    {
        return mControlPoints->at(aIndex);
    }

    return std::make_shared<Point3D>(DBL_NAN, DBL_NAN, DBL_NAN);
}

const Model::KnotListPtr& Model::NurbsCurve::GetKnots() const
{
    return mKnots;
}

Model::KnotListPtr Model::NurbsCurve::GetMutableKnots()
{
    return mKnots;
}

void Model::NurbsCurve::SetKnots(const Model::KnotListPtr& aKnots)
{
    mKnots = aKnots;
}

size_t Model::NurbsCurve::GetKnotsSize() const
{
    return mKnots->size();
}

double Model::NurbsCurve::GetKnot(const size_t& aIndex) const
{
    if (aIndex < mKnots->size())
    {
        return mKnots->at(aIndex);
    }

    return DBL_NAN;
}

const double& Model::NurbsCurve::GetPaintTotalLength() const
{
    return mPaintTotalLength;
}

void Model::NurbsCurve::SetPaintTotalLength(const double& aPaintTotalLength)
{
    mPaintTotalLength = aPaintTotalLength;
}

const double& Model::NurbsCurve::GetLineLength() const
{
    return mLineLength;
}

void Model::NurbsCurve::SetLineLength(const double& aLineLength)
{
    mLineLength = aLineLength;
}

void Model::NurbsCurve::SetNurbs(const Model::Point3DListPtr& aControlPoints,
                                 const Model::KnotListPtr& aKnots)
{
    std::vector<_vec3d> ctrls;
    ctrls.reserve(aControlPoints->size());
    for (std::size_t i = 0; i < aControlPoints->size(); ++i)
    {
        _vec3d p =
        {
            aControlPoints->at(i)->GetX(),
            aControlPoints->at(i)->GetY(),
            aControlPoints->at(i)->GetZ()
        };
        ctrls.push_back(p);
    }
    mNurbs = std::make_shared<YGEO::NURBS>(ctrls, *aKnots, aKnots->front(), aKnots->back());
}

void Model::NurbsCurve::CalculateLength(const double& aSamplingInterval,
                                        double& aTotalLength,
                                        double& aPaintLength)
{
    PaintListPtr paintPoints = CalculatePaintPointCloud(aSamplingInterval);
    aTotalLength = 0;
    aPaintLength = 0;
    for (std::size_t i = 0; i < paintPoints->size(); ++i)
    {
        std::size_t j;
        for (j = 0; j < paintPoints->at(i)->size() - 1; ++j)
        {
            aPaintLength += Point3D::Distance(paintPoints->at(i)->at(j),
                                              paintPoints->at(i)->at(j + 1));
        }
        double interval = 0;
        if (i < paintPoints->size() - 1)
        {
            interval = Point3D::Distance(paintPoints->at(i)->at(j),
                                         paintPoints->at(i + 1)->at(0));
        }
        aTotalLength = aTotalLength + aPaintLength + interval;
    }
}

void Model::NurbsCurve::generateNurbsFromJSON(const QJsonArray& aObjectCtrlPoints, const QJsonArray& aObjectKnots)
{
    std::vector<_vec3d> ctrls;
    size_t controlPointSize = static_cast<size_t>(aObjectCtrlPoints.size());

    ctrls.reserve(controlPointSize);
    mControlPoints->clear();
    mControlPoints->reserve(controlPointSize);

    for (size_t i = 0; i < controlPointSize; i++)
    {
        std::string ctrlPointStr = aObjectCtrlPoints.at(i).toString().toStdString();
        std::vector<std::string> coefficients = strings::Split(ctrlPointStr, ",");
        _vec3d point =
        {
            std::stod(coefficients[0]),
            std::stod(coefficients[1]),
            std::stod(coefficients[2])
        };
        ctrls.push_back(point);
        mControlPoints->push_back(std::make_shared<Point3D>(point.x, point.y, point.z));
    }

    size_t knotSize = static_cast<size_t>(aObjectKnots.size());
    mKnots->reserve(knotSize);

    for (size_t i = 0; i < knotSize; i++)
    {
        std::string knotStr = aObjectKnots.at(i).toString().toStdString();
        mKnots->push_back(std::stod(knotStr));
    }

    mNurbs = std::make_shared<YGEO::NURBS>(ctrls, *mKnots, mKnots->front(), mKnots->back());
}

void Model::NurbsCurve::readPaintRangeFromJSON(const QJsonArray& aObjectPaintEndPoints)
{
    size_t numPaint = static_cast<size_t>(aObjectPaintEndPoints.size());
    mPaintRange->clear();
    mPaintRange->reserve(numPaint);

    for (size_t i = 0; i < numPaint; i++)
    {
        std::vector<std::string> results;
        results = strings::Split(aObjectPaintEndPoints.at(i).toString().toStdString(), ",");

        mPaintRange->push_back(
                        std::make_pair<double, double>(std::stod(results[0]), std::stod(results[1]))
        );
    }
}

void Model::NurbsCurve::writeNurbsToJSON(QJsonArray& aObjectCtrlPoints, QJsonArray& aObjectKnots) const
{
    for (const Point3DPtr& point : *mControlPoints)
    {
        std::string pointStr = point->FormatPoint(13, 13, 13);
        aObjectCtrlPoints.append(QJsonValue(QString::fromStdString(pointStr)));
    }

    for (const double& knot : *mKnots)
    {
        std::string knotStr = strings::FormatFloat<double>(knot, 17);
        aObjectKnots.append(QJsonValue(QString::fromStdString(knotStr)));
    }
}

void Model::NurbsCurve::writePaintRangeToJSON(QJsonArray& aObjectPaintEndPoints) const
{
    for (const auto& paint : *mPaintRange)
    {
        std::string paintStr;

        paintStr += strings::FormatFloat<double>(paint.first, 17);
        paintStr += ",";
        paintStr += strings::FormatFloat<double>(paint.second, 17);

        aObjectPaintEndPoints.append(QJsonValue(QString::fromStdString(paintStr)));
    }
}

void Model::NurbsCurve::SetPoly3(const poly3_coef_t& aPol3Coef, Poly2Ptr aPoly2)
{
    aPoly2->SetC(aPol3Coef.a);
    aPoly2->SetB(aPol3Coef.b);
    aPoly2->SetA(aPol3Coef.c);
}

void Model::NurbsCurve::SetPoly3T(const poly3_curve_t& aPoly3Curve, Poly2XYZTPtr aPoly2)
{
    aPoly2->SetStart(aPoly3Curve.t_min);
    aPoly2->SetEnd(aPoly3Curve.t_max);
    SetPoly3(aPoly3Curve.fx, aPoly2->GetMutablePoly2X());
    SetPoly3(aPoly3Curve.fy, aPoly2->GetMutablePoly2Y());
    SetPoly3(aPoly3Curve.fz, aPoly2->GetMutablePoly2Z());
}

Model::Poly2XYZTListPtr Model::NurbsCurve::GetPaintEquationList(const size_t& index)
{
    if (index >= mPaintRange->size())
    {
        return nullptr;
    }

    Poly2XYZTListPtr equationParameters = std::make_shared<Poly2XYZTList>();
    const std::vector<poly3_curve_t>& curves =  mNurbs->GetCurve();
    const std::pair<double, double>& paintRange = mPaintRange->at(index);

    const double& startKnot = paintRange.first;
    const double& endKnot = paintRange.second;

    for (size_t i = 0; i < curves.size(); i++)
    {
        const poly3_curve_t& curve = curves[i];
        double knot1 = curve.t_min;
        double knot2 = curve.t_max;

        if (knot2 < startKnot || knot1 > endKnot)
        {
            continue;
        }

        knot1 = (knot1 < startKnot) ? startKnot : knot1;
        knot2 = (knot2 > endKnot) ? endKnot : knot2;

        Poly2XYZTPtr curve2 = std::make_shared<Poly2XYZT>();
        SetPoly3T(curve, curve2);
        curve2->SetStart(knot1);
        curve2->SetEnd(knot2);
        equationParameters->push_back(curve2);
    }

    return equationParameters;
}

Model::Poly2XYZTListPtr Model::NurbsCurve::GetEquationList()
{
    Poly2XYZTListPtr equationParameters = std::make_shared<Poly2XYZTList>();
    const std::vector<poly3_curve_t>& curves =  mNurbs->GetCurve();

    for (const std::pair<double, double>& paintRange : *mPaintRange)
    {
        const double& startKnot = paintRange.first;
        const double& endKnot = paintRange.second;

        for (size_t i = 0; i < curves.size(); i++)
        {
            const poly3_curve_t& curve = curves[i];
            double knot1 = curve.t_min;
            double knot2 = curve.t_max;

            if (knot2 < startKnot || knot1 > endKnot)
            {
                continue;
            }

            knot1 = (knot1 < startKnot) ? startKnot : knot1;
            knot2 = (knot2 > endKnot) ? endKnot : knot2;

            Poly2XYZTPtr curve2 = std::make_shared<Poly2XYZT>();
            SetPoly3T(curve, curve2);
            curve2->SetStart(knot1);
            curve2->SetEnd(knot2);
            equationParameters->push_back(curve2);
        }
    }

    return equationParameters;
}
