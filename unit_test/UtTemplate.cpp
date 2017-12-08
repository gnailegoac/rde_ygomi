/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    UtTemplate.cpp
 * @brief
 *******************************************************************************
 */

#include <gtest/gtest.h>

TEST(Test_Example, Data_Plus)
{
    ASSERT_EQ(3, 3);
    ASSERT_NE(19, 2);
    ASSERT_GE(8, 8);
    ASSERT_GE(10, 8);
    ASSERT_LT(8, 10);
}
