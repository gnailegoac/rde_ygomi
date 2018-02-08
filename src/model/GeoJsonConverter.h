/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    GeoJsonConverter.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <osg/Matrixd>

#include "model/MemoryModel.h"

namespace Model
{

class GeoJsonConverter
{
public:
    GeoJsonConverter();

    QJsonArray Convert(const osg::Matrixd &aMatrix);
    QJsonArray Convert(int aLevel, TileConstPtr& aTile);
    QJsonArray GetTileExtent(const std::shared_ptr<MemoryModel>& aMemoryModel);
    // Use aLevel = -1 for edit, which will use line's geodetic PaintList.
    // Other positive aLevel is used for displaying, which will use ViewPaintList.
    QJsonObject Convert(int aLevel, const Model::RoadPtr& aRoad);

private:
    QJsonObject convert(int aLevel, const LinePtr& aLine);
    QJsonArray convert(const PaintListPtr& aPaintList, bool aIsRelative);
    QJsonObject getTileBound(const TilePtr& aTile);
    void addLine(QJsonObject& aLane, QVector<quint64>& aLineIdVec, QJsonArray& aLines,
                 const QString& aPos, const LinePtr& aLine, int aLevel);
};
}
