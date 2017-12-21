/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Line.cpp
 * @brief
 *******************************************************************************
 */

#include "Line.h"

#include "CoordinateTransform/Factory.h"
#include "CoordinateTransform/ICoordinateTransform.h"

Model::Line::Line():
    Element(),
    mLineId(0),
    mCurveList(std::make_shared<CurveList>()),
    mConfidence(0.0),
    mLength(0.0),
    mLane(nullptr),
    mGeodeticPoints(std::make_shared<Point3DList>()),
    mPointListMap(std::make_shared<ViewPointMap>())
{

}

Model::Line::~Line()
{

}

const uint64_t& Model::Line::GetLineId() const
{
    return mLineId;
}

void Model::Line::SetLineId(const uint64_t& aLineId)
{
    mLineId = aLineId;
}

const Model::CurveListPtr& Model::Line::GetCurveList() const
{
    return mCurveList;
}

void Model::Line::SortCurve()
{
    std::sort(mCurveList->begin(), mCurveList->end(),
              [](const CurvePtr& aLhs,
                 const CurvePtr& aRhs)
    {
        return aLhs->GetIndexInLine() <= aRhs->GetIndexInLine();
    });
}

Model::CurveListPtr Model::Line::GetMutableCurveList()
{
    return mCurveList;
}

size_t Model::Line::GetCurveListSize() const
{
    return mCurveList->size();
}

Model::CurvePtr Model::Line::GetCurve(const size_t& aIndex)
{
    if (aIndex < mCurveList->size())
    {
        return mCurveList->at(aIndex);
    }

    return nullptr;
}

float Model::Line::GetConfidence() const
{
    return mConfidence;
}

void Model::Line::SetConfidence(float aConfidence)
{
    mConfidence = aConfidence;
}

float Model::Line::GetLength() const
{
    return mLength;
}

void Model::Line::SetLength(float aLength)
{
    mLength = aLength;
}

const Model::LanePtr& Model::Line::GetLane() const
{
    return mLane;
}

void Model::Line::SetLane(const Model::LanePtr& aLane)
{
    mLane = aLane;
}

const Model::Point3DListPtr& Model::Line::GetGeodeticPoints() const
{
    return mGeodeticPoints;
}

Model::Point3DListPtr Model::Line::GetMutableGeodeticPoints()
{
    return mGeodeticPoints;
}

void Model::Line::SetGeodeticPoints(const Model::Point3DListPtr& aGeodeticPoints)
{
    mGeodeticPoints = aGeodeticPoints;
}

void Model::Line::CreateGeodeticPoints(const Point3DPtr& aReferencePoint, const double& aSamplingInterval)
{
    std::shared_ptr<CRS::Factory> factory = std::make_shared<CRS::Factory>();
    std::unique_ptr<CRS::ICoordinateTransform> relativeToWgs84 =
                    factory->CreateRelativeTransform(CRS::CoordinateType::Relative,
                                                     CRS::CoordinateType::Wgs84,
                                                     aReferencePoint->GetX(),
                                                     aReferencePoint->GetY(),
                                                     aReferencePoint->GetZ());
    std::unique_ptr<CRS::ICoordinateTransform> wgs84ToUtm =
                    factory->CreateProjectionTransform(CRS::CoordinateType::Wgs84,
                                                       CRS::CoordinateType::Utm,
                                                       "+proj=utm +datum=WGS84 +unit=m +no_defs");
    mGeodeticPoints->clear();

    for (CurvePtr curve : *mCurveList)
    {
        Point3DListPtr point3DList = curve->CalculatePointCloud(aSamplingInterval);
        mGeodeticPoints->reserve(mGeodeticPoints->size() + point3DList->size());

        for (Point3DPtr point : *point3DList)
        {
            double x = point->GetX();
            double y = point->GetY();
            double z = point->GetZ();
            relativeToWgs84->Transform(x, y, z);
            wgs84ToUtm->Transform(x, y, z);
            mGeodeticPoints->push_back(std::make_shared<Point3D>(x, y, z));
        }
    }
}

const Model::ViewPointMapPtr& Model::Line::GetPointListMap() const
{
    return mPointListMap;
}

Model::ViewPointMapPtr Model::Line::GetMutablePointListMap()
{
    return mPointListMap;
}

Model::Point3DListPtr Model::Line::GetPointListByLevel(std::uint8_t aLevel)
{
    if (0 != mPointListMap->count(aLevel))
    {
        return mPointListMap->at(aLevel);
    }

    return nullptr;
}

Model::Point3DListPtr Model::Line::GetMutablePointListByLevel(std::uint8_t aLevel)
{
    if (0 == mPointListMap->count(aLevel))
    {
        (*mPointListMap)[aLevel] = std::make_shared<Point3DList>();
    }

    return mPointListMap->at(aLevel);
}
