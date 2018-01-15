/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    LookUpTable.cpp
 *******************************************************************************
 */

#include "LookUpTable.h"

std::string Model::LookUpTable::mDefaultTrafficSignImagePath
("resource/trafficsign/US_signs_pics_defines/default.jpg");
std::unordered_map<std::uint64_t, std::string> Model::LookUpTable::mTrafficSignImageMap
(
{
    {
        {274540, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_040_3.png"},
        {274560, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_060_1.png"},
        {276000, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_CAR_NO_PASSING_1.png"},
        {133000, "resource/trafficsign/US_signs_pics_defines/SRIF_SIGN_CLASS_DANGER_PEDESTRIAN_1.png"},
        {274555, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_055_4.png"},
        {274545, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_045_2.png"},
        {274540, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_040_5.png"},
        {274555, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_055_5.png"},
        {274555, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_055_1.png"},
        {205000, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_GIVE_WAY_1.png"},
        {274520, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_020_2.png"},
        {274550, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_050_4.png"},
        {206000, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_STOP_1.png"},
        {274530, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_030_6.png"},
        {274525, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_025_4.png"},
        {274555, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_055_6.png"},
        {274570, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_070_2.png"},
        {274505, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_005_1.png"},
        {274530, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_030_4.png"},
        {274515, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_015_2.png"},
        {274575, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_075_1.png"},
        {274545, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_045_4.png"},
        {274545, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_045_1.png"},
        {274565, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_065_2.png"},
        {211200, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_DIR_ARROW_SIDE_RIGHT_1.png"},
        {274555, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_055_7.png"},
        {209200, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_DIR_ARROW_TURN_RIGHT_1.png"},
        {274545, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_045_5.png"},
        {222100, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_DIR_ARROW_PASS_LEFT_1.png"},
        {274575, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_075_2.png"},
        {274535, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_035_4.png"},
        {274540, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_040_4.png"},
        {274565, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_065_3.png"},
        {274535, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_035_5.png"},
        {274540, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_040_6.png"},
        {274580, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_080_2.png"},
        {274525, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_025_5.png"},
        {274555, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_055_3.png"},
        {274525, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_025_1.png"},
        {274525, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_025_6.png"},
        {274585, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_085_1.png"},
        {274530, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_030_5.png"},
        {274555, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_055_8.png"},
        {274535, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_035_3.png"},
        {274535, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_035_1.png"},
        {274515, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_015_1.png"},
        {274570, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_070_1.png"},
        {274535, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_035_2.png"},
        {209300, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_DIR_ARROW_STRAIGHT_1.png"},
        {274530, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_030_3.png"},
        {222200, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_DIR_ARROW_PASS_RIGHT_1.png"},
        {274565, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_065_5.png"},
        {274515, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_015_4.png"},
        {274565, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_065_4.png"},
        {274525, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_025_3.png"},
        {274550, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_050_1.png"},
        {274525, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_025_2.png"},
        {274550, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_050_3.png"},
        {211100, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_DIR_ARROW_SIDE_LEFT_1.png"},
        {274540, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_040_1.png"},
        {274545, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_045_6.png"},
        {274555, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_055_2.png"},
        {274560, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_060_3.png"},
        {274540, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_040_2.png"},
        {274550, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_050_2.png"},
        {274530, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_030_2.png"},
        {274510, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_010_1.png"},
        {274530, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_030_1.png"},
        {274515, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_015_3.png"},
        {274565, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_065_1.png"},
        {274520, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_020_1.png"},
        {274580, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_080_1.png"},
        {274560, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_060_2.png"},
        {276000, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_CAR_NO_PASSING_2.png"},
        {274545, "resource/trafficsign/US_signs_pics_defines/SR_SIGN_CLASS_MAIN_SL_045_3.png"},
    }
}
);
