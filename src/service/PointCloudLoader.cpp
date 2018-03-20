/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file   PointCloudLoader.cpp
 *******************************************************************************
 */

#include "PointCloudLoader.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QJsonDocument>
#include <algorithm>

namespace Service
{

static const int gcDimension = 3;

PointCloudLoader::PointCloudLoader()
    : mMaxX(0.0)
    , mMinX(0.0)
    , mMaxY(0.0)
    , mMinY(0.0)
    , mMaxZ(0.0)
    , mMinZ(0.0)
{
}

PointCloudLoader::~PointCloudLoader()
{
}

void PointCloudLoader::filterPoints(const DbConverter::Point3d& minPoint,
                                    const DbConverter::Point3d& maxPoint)
{
    double minX = minPoint.GetX();
    double maxX = maxPoint.GetX();
    if (minX > maxX)
    {
        std::swap(minX, maxX);
    }

    double minY = minPoint.GetY();
    double maxY = maxPoint.GetY();
    if (minY > maxY)
    {
        std::swap(minY, maxY);
    }

    std::vector<std::uint64_t> ids;
    std::vector<DbConverter::PointClass> pointClasses;
    std::vector<std::int32_t> subTypes;
    std::vector<double> coords;
    for (std::size_t i = 0; i < mRoadElementIds.size(); ++i)
    {
        double x = mCoords[i * gcDimension];
        double y = mCoords[i * gcDimension + 1];
        if ((x < minX) || (x > maxX) || (y < minY) || (y > maxY))
        {
            continue;
        }

        ids.push_back(mRoadElementIds[i]);
        pointClasses.push_back(mPointClasses[i]);
        subTypes.push_back(mSubTypes[i]);
        coords.push_back(x);
        coords.push_back(y);
        coords.push_back(mCoords[i * gcDimension + 2]);
    }

    mRoadElementIds = ids;
    mPointClasses = pointClasses;
    mSubTypes = subTypes;
    mCoords = coords;

    autoCalculatePointRange();
}

int PointCloudLoader::getDimension() const
{
    return gcDimension;
}

double PointCloudLoader::getMaxX() const
{
    return mMaxX;
}

double PointCloudLoader::getMinX() const
{
    return mMinX;
}

double PointCloudLoader::getMaxY() const
{
    return mMaxY;
}

double PointCloudLoader::getMinY() const
{
    return mMinY;
}

double PointCloudLoader::getMaxZ() const
{
    return mMaxZ;
}

double PointCloudLoader::getMinZ() const
{
    return mMinZ;
}

std::vector<std::uint64_t>& PointCloudLoader::getRoadElementIds()
{
    return mRoadElementIds;
}

const std::vector<std::uint64_t>& PointCloudLoader::getRoadElementIds() const
{
    return mRoadElementIds;
}

std::vector<uint64_t>& PointCloudLoader::getPointIdxs()
{
    return mPointIdxs;
}

const std::vector<uint64_t>& PointCloudLoader::getPointIdxs() const
{
    return mPointIdxs;
}

std::vector<std::int32_t>& PointCloudLoader::getSubTypes()
{
    return mSubTypes;
}

const std::vector<std::int32_t>& PointCloudLoader::getSubTypes() const
{
    return mSubTypes;
}

std::vector<double>& PointCloudLoader::getCoords()
{
    return mCoords;
}

const std::vector<double>& PointCloudLoader::getCoords() const
{
    return mCoords;
}

void PointCloudLoader::setPointClass(const std::vector<DbConverter::PointClass>& pointClass)
{
    mPointClasses = pointClass;
}

std::vector<DbConverter::PointClass> PointCloudLoader::getPointClasses() const
{
    return mPointClasses;
}

std::vector<std::string>
PointCloudLoader::readFilenamesFromFolder(const QString& path,
                                          const QStringList& fileType)
{
    // Read the file in the directory ONLY
    // Don't care the sub-directory
    std::vector<std::string> result;
    QDir dir(path);
    if (dir.exists())
    {
        dir.setFilter(QDir::Files);

        QFileInfoList fileList = dir.entryInfoList(fileType);
        for (const auto& iterFileList : fileList)
        {
            result.push_back(iterFileList.absoluteFilePath().toStdString());
        }
    }

    return result;
}

void PointCloudLoader::autoCalculatePointRange()
{
    auto pointCount = mRoadElementIds.size();
    if (pointCount <= 0)
    {
        return;
    }

    mMinX = mMaxX = mCoords[0];
    mMinY = mMaxY = mCoords[1];
    mMinZ = mMaxZ = mCoords[2];
    for (std::size_t i = 1; i < pointCount; ++i)
    {
        const auto& x = mCoords[i * gcDimension];
        const auto& y = mCoords[i * gcDimension + 1];
        const auto& z = mCoords[i * gcDimension + 2];
        if (mMinX > x)
        {
            mMinX = x;
        }
        if (mMinY > y)
        {
            mMinY = y;
        }
        if (mMinZ > z)
        {
            mMinZ = z;
        }
        if (mMaxX < x)
        {
            mMaxX = x;
        }
        if (mMaxY < y)
        {
            mMaxY = y;
        }
        if (mMaxZ < z)
        {
            mMaxZ = z;
        }
    }
}
} // namespace QiTools
