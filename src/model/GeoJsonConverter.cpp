/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    GeoJsonConverter.cpp
 * @brief
 *******************************************************************************
 */

#include "GeoJsonConverter.h"

Model::GeoJsonConverter::GeoJsonConverter()
{
}

QJsonArray Model::GeoJsonConverter::Convert(const osg::Matrixd &aMatrix)
{
    QJsonArray matrixArray = {aMatrix(0, 0), aMatrix(0, 1), aMatrix(0, 2), aMatrix(0, 3),
                              aMatrix(1, 0), aMatrix(1, 1), aMatrix(1, 2), aMatrix(1, 3),
                              aMatrix(2, 0), aMatrix(2, 1), aMatrix(2, 2), aMatrix(2, 3),
                              aMatrix(3, 0), aMatrix(3, 1), aMatrix(3, 2), aMatrix(3, 3)};
    return matrixArray;
}
