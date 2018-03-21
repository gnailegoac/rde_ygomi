/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    QIModel.cpp
 *******************************************************************************
 */

#include "QIModel.h"

namespace Model {

QIModel::QIModel():mSelectedErrCode(0)
{}

void QIModel::AddErrPoint(int errID, const ErrorPoint& errPoint)
{
    mErrPointMap[errID].push_back(errPoint);
}

const std::map<int, std::vector<ErrorPoint>>& QIModel::GetErrPointMap()
{
    return mErrPointMap;
}

void QIModel::SetRefPoint(double x, double y, double z)
{
    mReferencePoint.SetX(x);
    mReferencePoint.SetY(y);
    mReferencePoint.SetZ(z);
}

const Point3D& QIModel::GetRefPoint()
{
    return mReferencePoint;
}

void QIModel::SetSelectedErrCode(int errCode)
{
    mSelectedErrCode = errCode;
}

int QIModel::GetSelectedErrCode()
{
    return mSelectedErrCode;
}

}
