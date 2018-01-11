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
#include <osg/Matrixd>

namespace Model
{

class GeoJsonConverter
{
  public:
    GeoJsonConverter();

    QJsonArray Convert(const osg::Matrixd &aMatrix);
};
}
