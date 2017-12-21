/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    UtDouglasPeucker.cpp
 * @brief
 *******************************************************************************
 */

#include <gtest/gtest.h>
#include <cmath>

#define private public
#define protected public
#include "../src/model/DouglasPeucker.h"
#undef protected
#undef private

class TestData : public testing::Test
{
protected:
    static void SetUpTestCase()
    {
        std::vector<Model::Point3DPtr> testPoints;
        testPoints.reserve(1000);
        for (double x = 0; x < 100; x += 0.1)
        {
            double y = 5 * std::sin(0.01 * x) + 10 * std::cos(0.1 * x) + 5;
            double z = 0.1 * x;
            testPoints.push_back(std::make_shared<Model::Point3D>(x, y, z));
        }
        mDouglas = std::make_shared<Model::DouglasPeucker>(
                                   std::make_shared<Model::Point3DList>(testPoints));
        mPoint1 = std::make_shared<Model::Point3D>(0, 0, 0);
        mPoint2 = std::make_shared<Model::Point3D>(10, 0, 0);
        mPoint3 = std::make_shared<Model::Point3D>(3.6, 4.8, 0);
    }

protected:
    static std::shared_ptr<Model::DouglasPeucker> mDouglas;
    static Model::Point3DPtr mPoint1;
    static Model::Point3DPtr mPoint2;
    static Model::Point3DPtr mPoint3;
};

std::shared_ptr<Model::DouglasPeucker> TestData::mDouglas;
Model::Point3DPtr TestData::mPoint1;
Model::Point3DPtr TestData::mPoint2;
Model::Point3DPtr TestData::mPoint3;

TEST_F(TestData, Point_To_Point_Distance_Test)
{
    double ZERO = 1e-6;
    double distance = mDouglas->pointToPointDistance(mPoint1, mPoint2);
    EXPECT_NEAR(distance, 10, ZERO);
    distance = mDouglas->pointToPointDistance(mPoint1, mPoint3);
    EXPECT_NEAR(distance, 6, ZERO);
    distance = mDouglas->pointToPointDistance(mPoint2, mPoint3);
    EXPECT_NEAR(distance, 8, ZERO);
}

TEST_F(TestData, Point_To_Line_Distance_Test)
{
    double ZERO = 1e-6;
    double distance = mDouglas->pointToLineDistance(mPoint1, mPoint2, mPoint3);
    EXPECT_NEAR(distance, 6, ZERO);
    distance = mDouglas->pointToLineDistance(mPoint2, mPoint1, mPoint3);
    EXPECT_NEAR(distance, 8, ZERO);
    distance = mDouglas->pointToLineDistance(mPoint3, mPoint1, mPoint2);
    EXPECT_NEAR(distance, 4.8, ZERO);
}

TEST_F(TestData, Farthest_Distance_Test)
{
    double ZERO = 1e-6;
    std::pair<std::uint32_t, double> fp = mDouglas->getFarthestPoint(0, 1000);
    EXPECT_EQ(fp.first, 295);
    EXPECT_NEAR(fp.second, 14.040098356989594, ZERO);
    fp = mDouglas->getFarthestPoint(0, 295);
    EXPECT_EQ(fp.first, 74);
    EXPECT_NEAR(fp.second, 2.006942452711151, ZERO);
    fp = mDouglas->getFarthestPoint(295, 1000);
    EXPECT_EQ(fp.first, 626);
    EXPECT_NEAR(fp.second, 19.294119951184758, ZERO);
    fp = mDouglas->getFarthestPoint(295, 626);
    EXPECT_EQ(fp.first, 378);
    EXPECT_NEAR(fp.second, 2.6619359453332816, ZERO);
    fp = mDouglas->getFarthestPoint(626, 1000);
    EXPECT_EQ(fp.first, 891);
    EXPECT_NEAR(fp.second, 5.113883850910421, ZERO);
}

TEST_F(TestData, Simplify_Test)
{
    double ZERO = 1e-6;
    Model::Point3DListPtr points = mDouglas->Simplify(5);
    EXPECT_EQ(points->size(), 5);
    EXPECT_NEAR(points->at(0)->GetX(), 0, ZERO);
    EXPECT_NEAR(points->at(1)->GetX(), 29.5, ZERO);
    EXPECT_NEAR(points->at(2)->GetX(), 62.6, ZERO);
    EXPECT_NEAR(points->at(3)->GetX(), 89.1, ZERO);
    EXPECT_NEAR(points->at(4)->GetX(), 100, ZERO);
    points = mDouglas->Simplify(2);
    EXPECT_EQ(points->size(), 9);
    EXPECT_NEAR(points->at(0)->GetX(), 0, ZERO);
    EXPECT_NEAR(points->at(1)->GetX(), 7.4, ZERO);
    EXPECT_NEAR(points->at(2)->GetX(), 29.5, ZERO);
    EXPECT_NEAR(points->at(3)->GetX(), 37.8, ZERO);
    EXPECT_NEAR(points->at(4)->GetX(), 54.7, ZERO);
    EXPECT_NEAR(points->at(5)->GetX(), 62.6, ZERO);
    EXPECT_NEAR(points->at(6)->GetX(), 70.7, ZERO);
    EXPECT_NEAR(points->at(7)->GetX(), 89.1, ZERO);
    EXPECT_NEAR(points->at(8)->GetX(), 100, ZERO);
}
