/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    UtFitNurbs.cpp
 * @brief
 *******************************************************************************
 */

#include <gtest/gtest.h>
#include "../src/model/NurbsCurve.h"
#include "../src/model/DouglasPeucker.h"

#define private public
#define protected public
#include "../src/model/FitNurbs.h"
#undef protected
#undef private

#include <sstream>
#include <fstream>

class NurbsFittingTest : public testing::Test
{
protected:
    static void SetUpTestCase()
    {
        mPoints = std::make_shared<Model::PaintList>();
        Model::Point3DListPtr points = std::make_shared<Model::Point3DList>();
        for (std::size_t t = 0; t < 1000; ++t)
        {
            if ((t > 150 && t < 210) || (t > 420 && t < 475) ||
                    (t > 638 && t < 700) || (t > 862 && t < 900))
            {
                if (!points->empty())
                {
                    Model::Point3DList tmp = *points;
                    mPoints->push_back(std::make_shared<Model::Point3DList>(tmp));
                    points->clear();
                }
            }
            else
            {
                points->push_back(std::make_shared<Model::Point3D>(
                                                  t,
                                                  200 * std::sin(0.01 * t),
                                                  0.02 * t));
                if (t == 999)
                {
                    Model::Point3DList tmp = *points;
                    mPoints->push_back(std::make_shared<Model::Point3DList>(tmp));
                    points->clear();
                }
            }
        }
    }

protected:
    static Model::PaintListPtr mPoints;
};

Model::PaintListPtr NurbsFittingTest::mPoints;

TEST_F(NurbsFittingTest, U_Value_Generation_Test)
{
    double ZERO = 1e-6;
    for (std::size_t i = 0; i < mPoints->size(); ++i)
    {
        std::shared_ptr<std::vector<double>> u = Model::FitNurbs::generateU(mPoints->at(i));
        EXPECT_NEAR(u->at(0), 0, ZERO);
        EXPECT_NEAR(u->at(u->size() - 1), 1, ZERO);
    }
}


TEST_F(NurbsFittingTest, Fit_Points_Test)
{
    const double ZERO = 1e-6;
    std::string error = "";
    std::shared_ptr<Model::NurbsCurve> nc = Model::FitNurbs::FitPointsToCurve(mPoints, 3, error);
    EXPECT_TRUE(error.empty());

    /// knots test
    EXPECT_EQ(nc->GetKnotsSize(), 117);
    EXPECT_EQ(nc->GetKnot(0), 0);
    EXPECT_EQ(nc->GetKnot(nc->GetKnotsSize() - 1), 1);

    /// control points test
    auto controlPoints = nc->GetControlPoints();
    EXPECT_EQ(controlPoints->size(), 114);
    // the first control point should be the same as the first input point
    EXPECT_NEAR(controlPoints->at(0)->GetX(), mPoints->at(0)->at(0)->GetX(), ZERO);
    EXPECT_NEAR(controlPoints->at(0)->GetY(), mPoints->at(0)->at(0)->GetY(), ZERO);
    EXPECT_NEAR(controlPoints->at(0)->GetZ(), mPoints->at(0)->at(0)->GetZ(), ZERO);
    // the last control point should be the same as the last input point
    std::size_t sourceIndex1 = mPoints->size() - 1;
    std::size_t sourceIndex2 = mPoints->at(sourceIndex1)->size() - 1;
    std::size_t targetIndex = controlPoints->size() - 1;
    EXPECT_NEAR(controlPoints->at(targetIndex)->GetX(),
                mPoints->at(sourceIndex1)->at(sourceIndex2)->GetX(), ZERO);
    EXPECT_NEAR(controlPoints->at(targetIndex)->GetY(),
                mPoints->at(sourceIndex1)->at(sourceIndex2)->GetY(), ZERO);
    EXPECT_NEAR(controlPoints->at(targetIndex)->GetZ(),
                mPoints->at(sourceIndex1)->at(sourceIndex2)->GetZ(), ZERO);

    /// fitted points test
    auto fittedPoints = nc->CalculatePointCloud(2);
    EXPECT_EQ(fittedPoints->size(), 741);
    // first point should be the same
    EXPECT_NEAR(fittedPoints->at(0)->GetX(), mPoints->at(0)->at(0)->GetX(), ZERO);
    EXPECT_NEAR(fittedPoints->at(0)->GetY(), mPoints->at(0)->at(0)->GetY(), ZERO);
    EXPECT_NEAR(fittedPoints->at(0)->GetZ(), mPoints->at(0)->at(0)->GetZ(), ZERO);
    // last point should be the same
    targetIndex = fittedPoints->size() - 1;
    EXPECT_NEAR(fittedPoints->at(targetIndex)->GetX(),
                mPoints->at(sourceIndex1)->at(sourceIndex2)->GetX(), ZERO);
    EXPECT_NEAR(fittedPoints->at(targetIndex)->GetY(),
                mPoints->at(sourceIndex1)->at(sourceIndex2)->GetY(), ZERO);
    EXPECT_NEAR(fittedPoints->at(targetIndex)->GetZ(),
                mPoints->at(sourceIndex1)->at(sourceIndex2)->GetZ(), ZERO);
}
