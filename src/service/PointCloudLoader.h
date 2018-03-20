/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file   PointCloudLoader.h
 * @brief  An adapter of coventing files by GIS team
 *******************************************************************************
 */

#ifndef POINTCLOUDLOADER_H
#define POINTCLOUDLOADER_H

#include <QString>
#include <QStringList>
#include <map>
#include <memory>
#include <vector>

#include "DbConverter/Point.h"

namespace Service
{

class PointCloudLoader
{
public:
    PointCloudLoader();
    virtual ~PointCloudLoader();

    void filterPoints(const DbConverter::Point3d& minPoint,
                      const DbConverter::Point3d& maxPoint);

    int getDimension() const;

    /// Get the maximum X coordinate of the point cloud.
    double getMaxX() const;

    //-------------------------------------------------------------------------
    /// Get the mininum X coordinate of the point cloud.
    double getMinX() const;

    //-------------------------------------------------------------------------
    /// Get the maximum Y coordinate of the point cloud.
    double getMaxY() const;

    //-------------------------------------------------------------------------
    /// Get the mininum Y coordinate of the point cloud.
    double getMinY() const;

    //-------------------------------------------------------------------------
    /// Get the maximum Z coordinate of the point cloud.
    double getMaxZ() const;

    //-------------------------------------------------------------------------
    /// Get the mininum Z coordinate of the point cloud.
    double getMinZ() const;

    //-------------------------------------------------------------------------
    /// Get all the ids of the road elements.
    /// Each id is associate with the corresponding index of type and point.
    ///
    /// @return the road element ids.
    std::vector<std::uint64_t>& getRoadElementIds();
    const std::vector<std::uint64_t>& getRoadElementIds() const;

    //-------------------------------------------------------------------------
    /// Get all the ids of the point index.
    /// @return the point index.
    std::vector<std::uint64_t>& getPointIdxs();
    const std::vector<std::uint64_t>& getPointIdxs() const;

    //-------------------------------------------------------------------------
    /// Get all types of the road elements.
    /// Each type is associate with the corresponding index of id and point.
    ///
    /// @return the road element types.
    std::vector<std::int32_t>& getSubTypes();
    const std::vector<std::int32_t>& getSubTypes() const;

    /// Get all class types of the road elements: Curve of Triffic sign
    std::vector<DbConverter::PointClass> getPointClasses() const;
    void setPointClass(const std::vector<DbConverter::PointClass>& pointClass);

    //-------------------------------------------------------------------------
    /// Get all the point cloud coordinate data, sorted by y-axis, ascending.
    ///
    /// @return the sorted coordinates.
    std::vector<double>& getCoords();
    const std::vector<double>& getCoords() const;

private:

    static std::vector<std::string>
    readFilenamesFromFolder(const QString& path,
                            const QStringList& fileType = QStringList());

    void autoCalculatePointRange();

protected:
    std::vector<std::uint64_t> mRoadElementIds;
    std::vector<std::uint64_t> mPointIdxs;
    std::vector<DbConverter::PointClass> mPointClasses;
    std::vector<std::int32_t> mSubTypes;
    std::vector<double> mCoords;

    // range of coords for PC
    double mMaxX;
    double mMinX;

    double mMaxY;
    double mMinY;

    double mMaxZ;
    double mMinZ;
};
} // namespace QiTools
#endif // POINTCLOUDLOADER_H
