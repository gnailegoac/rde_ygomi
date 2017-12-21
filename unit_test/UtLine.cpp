/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    UtLine.cpp
 * @brief
 *******************************************************************************
 */

#include <gtest/gtest.h>

#define private public
#define protected public
#include "../src/model/Line.h"
#undef protected
#undef private

TEST(Line_Test, Generate_View_Points_Test)
{
    Model::Point3DList points;
    points.reserve(10000);
    double increment = 0.1 / 6378137 * 180 / 3.141592653;
    for (std::uint32_t i = 0; i < 10000; ++i)
    {
        double lon = 30 + increment * i;
        double lat = 0.001 * std::sin(1000 * lon) + 0.001 * std::cos(1000 * lon) + 5;
        double ele = 0.1 * increment * i;
        points.push_back(std::make_shared<Model::Point3D>(lon, lat, ele));
    }
    Model::Line line;
    line.SetGeodeticPoints(std::make_shared<Model::Point3DList>(points));
    line.GenerateViewPointMap();
    ASSERT_EQ(line.mPointListMap->size(), 3);
}
