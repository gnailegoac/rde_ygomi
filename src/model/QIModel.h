/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    QIModel.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Common.h"
#include "Point3D.h"
#include <map>

namespace Model
{

struct ErrorPoint
{
    double               dX;
    double               dY;
    double               dZ;
};

class QIModel
{
public:
    QIModel();
    void AddErrPoint(int errID, const ErrorPoint& errPoint);
    const std::map<int, std::vector<ErrorPoint>>& GetErrPointMap();

    void SetRefPoint(double x, double y, double z);
    const Point3D& GetRefPoint();

    void SetSelectedErrCode(int errCode);
    int GetSelectedErrCode();

private:

private:
    std::map<int, std::vector<ErrorPoint>> mErrPointMap;
    Point3D mReferencePoint;
    int mSelectedErrCode;
};

}
