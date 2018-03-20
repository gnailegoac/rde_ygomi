/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file   LogicDbLoader.h
 * @brief  Load logical database by GIS library
 *******************************************************************************
 */

#ifndef LOGICDBLOADER_H
#define LOGICDBLOADER_H

#include "DbConverter/IDbRoadMapData.h"
#include "DbConverter/Poly.h"

#include "PointCloudLoader.h"

namespace Service
{

class LogicDbLoader : public PointCloudLoader
{
public:
    LogicDbLoader();

    bool loadLogicalDb(const std::vector<std::string>& filePathsList, double samplingInterval,
                       bool geodeticFlag);

    double getCurveMinX() const;
    double getCurveMaxX() const;
    double getCurveMinY() const;
    double getCurveMaxY() const;
    double getCurveMinZ() const;
    double getCurveMaxZ() const;

    const DbConverter::Point3d& getReferencePoint() const;

    std::vector<uint64_t>& getLineIds();
    const std::vector<std::uint64_t>& getLineIds() const;

    std::vector<std::shared_ptr<DbConverter::Lane>>& getLanes();
    const std::vector<std::shared_ptr<DbConverter::Lane>>& getLanes() const;

    std::vector<std::vector<std::uint64_t>>& getLaneIds();
    const std::vector<std::vector<std::uint64_t>>& getLaneIds() const;

private:
    bool
    extractDbData(const std::shared_ptr<DbConverter::IDbRoadMapData>& dbRoadMapData);

    void setCurveBoundingBox(
            const std::shared_ptr<DbConverter::IDbRoadMapData>& dbRoadMapData);
    void setPointCloudBoundingBox(
            const std::shared_ptr<DbConverter::IDbRoadMapData>& dbRoadMapData);
    void
    setPointClouds(const std::shared_ptr<DbConverter::IDbRoadMapData>& dbRoadMapData);
    void
    setLanes(const std::shared_ptr<DbConverter::IDbRoadMapData>& dbRoadMapData);

private:
    double mCurveMinX;
    double mCurveMaxX;
    double mCurveMinY;
    double mCurveMaxY;
    double mCurveMinZ;
    double mCurveMaxZ;

    DbConverter::Point3d mReferencePoint;

    std::vector<std::uint64_t> mLineIds;
    std::vector<std::shared_ptr<DbConverter::Lane>> mLanes;
    std::vector<std::vector<std::uint64_t>> mLaneIds;
};

} // namespace QiTools

#endif // LOGICDBLOADER_H
