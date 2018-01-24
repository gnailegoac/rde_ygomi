/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    UtTile.cpp
 * @brief
 *******************************************************************************
 */

#include <gtest/gtest.h>

#include "../src/model/Common.h"

static inline std::int32_t GetBit(std::int32_t aInteger, std::uint8_t aIndex)
{
    uint32_t integer = static_cast<uint32_t>(aInteger);
    return static_cast<int32_t>((integer >> aIndex) & 1);
}

static inline  void SetBit(std::int32_t& aInteger, std::uint8_t aIndex)
{
    aInteger = aInteger | (uint32_t)1 << aIndex;
}

static inline std::int32_t point_to_tile_id(double l, double b)
{
    int32_t tile_id = 0;

    int32_t x = (int32_t)(l / (90. / (1 << 30)));
    int32_t y = (int32_t)(b / (90. / (1 << 30)));
    tile_id = GetBit(x, 31);

    for (int32_t i = 30; i > 16; --i)
    {
        tile_id <<= 2;
        tile_id |= GetBit(y, i) << 1 | GetBit(x, i);
    }

    return tile_id;
}

void CalculateReferencePoint(const std::int64_t& aTileId, double& l, double& b)
{
    std::int32_t tileId = static_cast<std::int32_t>(aTileId);
    std::int32_t x = 0;
    std::int32_t y = 0;

    for (int i = 13; i >= 0; --i)
    {
        x <<= 1;
        x |= GetBit(tileId, i * 2);
        y <<= 1;
        y |= GetBit(tileId, i * 2 + 1);
    }

    l = (90. / (1 << 13)) * (x);
    b = (90. / (1 << 13)) * (y);
}

TEST(Tile_Test, Serialize_Nurbs)
{
    double l = 10.096435546875;
    double b = 47.933349609375;

    std::int32_t tile_id = point_to_tile_id(l, b);
    EXPECT_EQ(tile_id, 34029983);

    double l_new = 0.0;
    double b_new = 0.0;

    CalculateReferencePoint(tile_id, l_new, b_new);
    EXPECT_NEAR(l, l_new, 1e-8);
    EXPECT_NEAR(b, b_new, 1e-8);
}
