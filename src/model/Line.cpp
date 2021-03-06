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
#include "DouglasPeucker.h"

Model::Line::Line():
    Element(),
    mLineId(0),
    mCurveList(std::make_shared<CurveList>()),
    mConfidence(0.0),
    mLength(0.0),
    mLane(nullptr),
    mGeodeticPointsList(std::make_shared<PaintList>()),
    mPaintListMap(std::make_shared<ViewPaintMap>()),
    mSimplingLevelMap(std::make_shared<SimplingLevelMap>())
{
    mSimplingLevelMap->insert(std::make_pair<std::uint8_t, double>(1, 2.0));
    mSimplingLevelMap->insert(std::make_pair<std::uint8_t, double>(2, 1.0));
    mSimplingLevelMap->insert(std::make_pair<std::uint8_t, double>(3, 0.5));
    mSimplingLevelMap->insert(std::make_pair<std::uint8_t, double>(4, 0.2));
    mSimplingLevelMap->insert(std::make_pair<std::uint8_t, double>(5, 0.1));
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
              [](const CurvePtr & aLhs,
                 const CurvePtr & aRhs)
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

const Model::PaintListPtr& Model::Line::GetGeodeticPointsList() const
{
    return mGeodeticPointsList;
}

Model::PaintListPtr Model::Line::GetMutableGeodeticPointsList()
{
    return mGeodeticPointsList;
}

void Model::Line::SetGeodeticPointsList(const Model::PaintListPtr& aGeodeticPointsList)
{
    mGeodeticPointsList = aGeodeticPointsList;
}

void Model::Line::CreateGeodeticPointsList(const Point3DPtr& aReferencePoint, const double& aSamplingInterval)
{
    std::shared_ptr<CRS::Factory> factory = std::make_shared<CRS::Factory>();
    std::unique_ptr<CRS::ICoordinateTransform> relativeToWgs84 =
                    factory->CreateRelativeTransform(CRS::CoordinateType::Relative,
                                                     CRS::CoordinateType::Wgs84,
                                                     aReferencePoint->GetX(),
                                                     aReferencePoint->GetY(),
                                                     aReferencePoint->GetZ());
    mGeodeticPointsList->clear();

    for (CurvePtr& curve : *mCurveList)
    {
        PaintListPtr paintList = curve->CalculatePaintPointCloud(aSamplingInterval);

        for (Point3DListPtr& point3DList : *paintList)
        {
            if (2 > point3DList->size())
            {
                continue;
            }

            Point3DListPtr geodeticPoints = std::make_shared<Point3DList>();
            geodeticPoints->reserve(point3DList->size());

            for (const Point3DPtr& point : *point3DList)
            {
                double x = point->GetX();
                double y = point->GetY();
                double z = point->GetZ();
                relativeToWgs84->Transform(x, y, z);
                geodeticPoints->push_back(std::make_shared<Point3D>(x, y, z));
            }

            mGeodeticPointsList->push_back(geodeticPoints);
        }
    }
}

const Model::ViewPaintMapPtr& Model::Line::GetPaintListMap() const
{
    return mPaintListMap;
}

Model::ViewPaintMapPtr Model::Line::GetMutablePaintListMap()
{
    return mPaintListMap;
}

Model::PaintListPtr Model::Line::GetPaintListByLevel(std::uint8_t aLevel)
{
    if (0 != mPaintListMap->count(aLevel))
    {
        return mPaintListMap->at(aLevel);
    }
    if(0 != mSimplingLevelMap->count(aLevel))
    {
        GenerateViewPaintMap(aLevel);
        return mPaintListMap->at(aLevel);
    }
    return nullptr;
}

Model::PaintListPtr Model::Line::GetMutablePaintListByLevel(std::uint8_t aLevel)
{
    if (0 == mPaintListMap->count(aLevel))
    {
        (*mPaintListMap)[aLevel] = std::make_shared<PaintList>();
    }

    return mPaintListMap->at(aLevel);
}

void Model::Line::GenerateViewPaintMap(std::uint8_t aLevel)
{
    mPaintListMap->insert(std::make_pair(aLevel, std::make_shared<PaintList>()));
    auto ecef = CRS::Factory().CreateEcefProjection(
                                CRS::CoordinateType::Wgs84,
                                CRS::CoordinateType::Ecef);
    for (Point3DListPtr& geodeticPoints : *mGeodeticPointsList)
    {
        Point3DListPtr points = std::make_shared<Point3DList>();
        points->reserve(geodeticPoints->size());

        for (auto& p : *geodeticPoints)
        {
            double lon = p->GetX();
            double lat = p->GetY();
            double ele = p->GetZ();
            ecef->Transform(lon, lat, ele);
            points->push_back(std::make_shared<Point3D>(lon, lat, ele));
        }
        // Down-sample points with Douglas-Peucker algorithm
        mPaintListMap->at(aLevel)->push_back(Model::DouglasPeucker::SimplifyS(points, mSimplingLevelMap->at(aLevel)));
    }
}

Model::CurveType Model::Line::GetLineType() const
{
    // For a line has curves of different type, will consider it as dashed if there is one dashed curve.
    // If the curves in the line are not continuous, the line type should also be dashed.
    // Actually, the road should be sliced to avoid a line composed of different type curves
    for (const auto& curve : *mCurveList)
    {
        if (curve->GetCurveType() == CurveType::Dashed)
        {
            return CurveType::Dashed;
        }

        if (curve->GetCurveType() == CurveType::UnKnown)
        {
            return CurveType::UnKnown;
        }

        if (curve->GetCurveType() == CurveType::UnDefined)
        {
            return CurveType::UnDefined;
        }
    }

    return CurveType::Solid;
}
