/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file   LogicDbLoader.cpp
 *******************************************************************************
 */

#include "DbConverter/DbConvertException.h"
#include "DbConverter/Factory.h"

#include "LogicDbLoader.h"

namespace Service
{

LogicDbLoader::LogicDbLoader()
    : mCurveMinX(0.0)
    , mCurveMaxX(0.0)
    , mCurveMinY(0.0)
    , mCurveMaxY(0.0)
    , mCurveMinZ(0.0)
    , mCurveMaxZ(0.0)
{
}

bool LogicDbLoader::loadLogicalDb(const std::vector<std::string>& filePathsList,
                                  double samplingInterval, bool geodeticFlag)
{
    std::shared_ptr<DbConverter::IDbRoadMapData> roadMapData;
    try
    {
        roadMapData = DbConverter::Factory()
                              .CreateDbParser(filePathsList, samplingInterval,
                                              true, geodeticFlag)
                              ->Convert();
        return extractDbData(roadMapData);
    }
    catch (DbConverter::DbConvertException& ex)
    {
        //QI_GIS_LOG_ERROR(QString(ex.what()));
        return false;
    }
}

double LogicDbLoader::getCurveMinX() const
{
    return mCurveMinX;
}

double LogicDbLoader::getCurveMaxX() const
{
    return mCurveMaxX;
}

double LogicDbLoader::getCurveMinY() const
{
    return mCurveMinY;
}

double LogicDbLoader::getCurveMaxY() const
{
    return mCurveMaxY;
}

double LogicDbLoader::getCurveMinZ() const
{
    return mCurveMinZ;
}

double LogicDbLoader::getCurveMaxZ() const
{
    return mCurveMaxZ;
}

const DbConverter::Point3d& LogicDbLoader::getReferencePoint() const
{
    return mReferencePoint;
}

bool LogicDbLoader::extractDbData(
        const std::shared_ptr<DbConverter::IDbRoadMapData>& dbRoadMapData)
{
    if (dbRoadMapData == Q_NULLPTR)
    {
        return false;
    }

    setPointCloudBoundingBox(dbRoadMapData);
    setCurveBoundingBox(dbRoadMapData);
    setPointClouds(dbRoadMapData);
    setLanes(dbRoadMapData);

    auto refPoint = dbRoadMapData->GetReferencePoint();
    if (refPoint != Q_NULLPTR)
    {
        mReferencePoint = *refPoint;
    }

    return true;
}

void LogicDbLoader::setPointCloudBoundingBox(
        const std::shared_ptr<DbConverter::IDbRoadMapData>& dbRoadMapData)
{
    mMaxX = dbRoadMapData->GetPointCloudBoundingBox()->GetMaxX();
    mMinX = dbRoadMapData->GetPointCloudBoundingBox()->GetMinX();

    mMaxY = dbRoadMapData->GetPointCloudBoundingBox()->GetMaxY();
    mMinY = dbRoadMapData->GetPointCloudBoundingBox()->GetMinY();

    mMaxZ = dbRoadMapData->GetPointCloudBoundingBox()->GetMaxZ();
    mMinZ = dbRoadMapData->GetPointCloudBoundingBox()->GetMinZ();
}

void LogicDbLoader::setCurveBoundingBox(
        const std::shared_ptr<DbConverter::IDbRoadMapData>& dbRoadMapData)
{
    mCurveMaxX = dbRoadMapData->GetCurveEquationBoundingBox()->GetMaxX();
    mCurveMinX = dbRoadMapData->GetCurveEquationBoundingBox()->GetMinX();

    mCurveMaxY = dbRoadMapData->GetCurveEquationBoundingBox()->GetMaxY();
    mCurveMinY = dbRoadMapData->GetCurveEquationBoundingBox()->GetMinY();

    mCurveMaxZ = dbRoadMapData->GetCurveEquationBoundingBox()->GetMaxZ();
    mCurveMinZ = dbRoadMapData->GetCurveEquationBoundingBox()->GetMinZ();
}

void LogicDbLoader::setPointClouds(
        const std::shared_ptr<DbConverter::IDbRoadMapData>& dbRoadMapData)
{
    mPointIdxs.clear();
    mRoadElementIds.clear();
    mLaneIds.clear();
    mLineIds.clear();
    mSubTypes.clear();
    mPointClasses.clear();
    mCoords.clear();

    std::uint64_t pointIdx = 0;
    for (const auto& iterTmpPoints : dbRoadMapData->GetTypedPointCloud())
    {

        mPointIdxs.push_back(pointIdx++);
        mRoadElementIds.push_back(iterTmpPoints->GetId());
        mSubTypes.push_back(iterTmpPoints->GetType());
        mPointClasses.push_back(iterTmpPoints->GetPointClass());
        mCoords.push_back(iterTmpPoints->GetPosition()->GetX());
        mCoords.push_back(iterTmpPoints->GetPosition()->GetY());
        mCoords.push_back(iterTmpPoints->GetPosition()->GetZ());
        mLineIds.push_back(iterTmpPoints->GetLineId());

        std::vector<std::uint64_t> subLanesIds;
        for (auto& laneid : iterTmpPoints->GetLaneIds())
        {
            subLanesIds.push_back(laneid);
        }
        mLaneIds.push_back(subLanesIds);
    }
}

void LogicDbLoader::setLanes(const std::shared_ptr<DbConverter::IDbRoadMapData>& dbRoadMapData)
{
    mLanes = dbRoadMapData->GetLanes();
}

std::vector<uint64_t>& LogicDbLoader::getLineIds()
{
    return mLineIds;
}

const std::vector<uint64_t>& LogicDbLoader::getLineIds() const
{
    return mLineIds;
}

std::vector<std::shared_ptr<DbConverter::Lane>>& LogicDbLoader::getLanes()
{
    return mLanes;
}

const std::vector<std::shared_ptr<DbConverter::Lane>>& LogicDbLoader::getLanes() const
{
    return mLanes;
}

std::vector<std::vector<uint64_t>>& LogicDbLoader::getLaneIds()
{
    return mLaneIds;
}

const std::vector<std::vector<uint64_t>>& LogicDbLoader::getLaneIds() const
{
    return mLaneIds;
}

} // namespace QiTools
