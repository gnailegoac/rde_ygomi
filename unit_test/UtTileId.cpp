/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    UtTileId.cpp
 * @brief
 *******************************************************************************
 */

#define private public
#define protected public
#include "../src/model/Tile.h"
#undef protected
#undef private

#include <gtest/gtest.h>

TEST(Tile_Test, Transform_Between_TileId_And_GeodeticPoint)
{
    Model::TilePtr tile(new Model::Tile);

    {
        const std::int32_t tileId = 34030404;
        const double l = 10.12939453125;
        const double b = 47.98828125;
        Model::Point3DPtr point = tile->tileIdToReferencePoint(tileId);
        EXPECT_EQ(tile->referencePointToTileId(l, b), tileId);
        EXPECT_NEAR(l, point->GetX(), 1e-8);
        EXPECT_NEAR(b, point->GetY(), 1e-8);
    }

    {
        const std::int32_t tileId = 34030416;
        const double l = 10.1513671875;
        const double b = 47.98828125;
        Model::Point3DPtr point = tile->tileIdToReferencePoint(tileId);
        EXPECT_EQ(tile->referencePointToTileId(l, b), tileId);
        EXPECT_NEAR(l, point->GetX(), 1e-8);
        EXPECT_NEAR(b, point->GetY(), 1e-8);
    }

    {
        const std::int32_t tileId = 34030407;
        const double l = 10.140380859375;
        const double b = 47.999267578125;
        Model::Point3DPtr point = tile->tileIdToReferencePoint(tileId);
        EXPECT_EQ(tile->referencePointToTileId(l, b), tileId);
        EXPECT_NEAR(l, point->GetX(), 1e-8);
        EXPECT_NEAR(b, point->GetY(), 1e-8);
    }
}
