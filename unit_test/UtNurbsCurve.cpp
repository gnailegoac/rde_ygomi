/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    UtNurbsCurve.cpp
 * @brief
 *******************************************************************************
 */

#include <gtest/gtest.h>

#define private public
#define protected public
#include "../src/model/NurbsCurve.h"
#undef protected
#undef private

std::string gSplineJsonValue =  "{"
                                "\"ControlPoints\": ["
                                "\"515.4965209960938,1262.366943359375,64.12987518310547\","
                                "\"474.4831848144531,1179.629150390625,63.78123474121094\","
                                "\"434.5125732421875,1096.431884765625,63.10688781738281\","
                                "\"393.7693176269531,1013.565124511719,61.36127471923828\""
                                "],"
                                "\"Knots\": ["
                                "\"0.4285394549369812\","
                                "\"0.4523540735244751\","
                                "\"0.4761701226234436\","
                                "\"0.4999855160713196\","
                                "\"0.523802638053894\","
                                "\"0.5476223230361938\","
                                "\"0.5714346766471863\""
                                "],"
                                "\"PaintEndPoints\": ["
                                "\"0.4999412894248962,0.5045203566551208\""
                                "],"
                                "\"PaintTotalLength\": ["
                                "\"17.69827270507812\""
                                "],"
                                "\"LineLength\": ["
                                "\"8.874074935913086\""
                                "]"
                                "}";

TEST(NurbsCurve_Test, Serialize_Nurbs)
{
    Model::NurbsCurve nurbs;
    EXPECT_TRUE(nurbs.Convert(gSplineJsonValue));
    double length = nurbs.mNurbs->getLength(0.4999412894248962,0.5045203566551208);

    Model::NurbsCurve nurbsNew;
    EXPECT_TRUE(nurbsNew.Convert(nurbs.Parse()));
    double lengthNew = nurbsNew.mNurbs->getLength(0.4999412894248962,0.5045203566551208);

    EXPECT_NEAR(length, lengthNew, 1e-5);
}
