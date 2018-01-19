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

#include "model/Tile.h"
#include "model/Line.h"

namespace Model
{

class GeoJsonConverter
{
public:
    GeoJsonConverter();

    QJsonArray Convert(const osg::Matrixd &aMatrix);
    QJsonArray Convert(int aLevel, TileConstPtr& aTile);

private:
    QJsonObject convert(int aLevel, const LinePtr& aLine);
    QJsonArray convert(const PaintListPtr& aPaintList);
};
}
