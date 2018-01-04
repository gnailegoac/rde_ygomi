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
                                                  0)); // 0.02 * t
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
        ASSERT_NEAR(u->at(0), 0, ZERO);
        ASSERT_NEAR(u->at(u->size() - 1), 1, ZERO);
    }
}

void writeData(const std::string& aOutFile, const Model::Point3DListPtr& aData)
{
    std::stringstream ss;
    for (const Model::Point3DPtr p : *aData)
    {
        ss << p->GetX() << "," << p->GetY() << "," << p->GetZ() << std::endl;
    }
    std::ofstream fout(aOutFile);
    fout << ss.str();
    fout.close();
}

Model::Point3DListPtr getKnotPoints(const Model::PaintListPtr& aInputPoints)
{
    std::size_t count = 0;
    for (auto& pointSet : *aInputPoints)
    {
        count += pointSet->size();
    }
    Model::Point3DListPtr totalPoints = std::make_shared<Model::Point3DList>();
    totalPoints->reserve(count);
    std::vector<std::pair<uint32_t, uint32_t>> paintIndex;
    paintIndex.reserve(aInputPoints->size());
    for (const Model::Point3DListPtr& pointSet : *aInputPoints)
    {
        paintIndex.push_back(std::make_pair(totalPoints->size(),
                                            totalPoints->size() + pointSet->size() - 1));
        totalPoints->insert(totalPoints->end(), pointSet->begin(), pointSet->end());
    }
    std::shared_ptr<std::vector<uint32_t> > knotIndex = std::make_shared<std::vector<uint32_t>>();
    for (auto& index : paintIndex)
    {
        auto tmpIndex = Model::DouglasPeucker::GetSimplifyIndex(totalPoints, 0.1,
                                                                index.first, index.second);
        knotIndex->insert(knotIndex->end(), tmpIndex->begin(), tmpIndex->end());
    }
    Model::Point3DListPtr knotPoints = std::make_shared<Model::Point3DList>();
    knotPoints->reserve(knotIndex->size());
    for (auto& i : *knotIndex)
    {
        knotPoints->push_back(totalPoints->at(i));
    }
    return knotPoints;
}



TEST_F(NurbsFittingTest, Fit_Points_Test)
{
    //    std::shared_ptr<Model::NurbsCurve> nc = Model::FitNurbs::FitPointsToCurve(mPoints);
    std::shared_ptr<std::vector<double>> knots = std::make_shared<std::vector<double>>();
    std::shared_ptr<std::vector<double>> u = std::make_shared<std::vector<double>>();
    std::shared_ptr<std::vector<std::pair<double, double>>> paintRange
        = std::make_shared<std::vector<std::pair<double, double>>>();
    Model::Point3DListPtr totalPoints = std::make_shared<Model::Point3DList>();
    Model::FitNurbs::initialise(mPoints, 2, knots, u, paintRange, totalPoints);
    std::shared_ptr<std::vector<Model::Point3DPtr> > controlPoints
        = Model::FitNurbs::controlPoints(totalPoints, 2);


    writeData("/media/psf/Home/Mytest/ControlPoints.txt", controlPoints);

    //    const Model::Point3DListPtr& points = nc->CalculatePointCloud(1);
}
