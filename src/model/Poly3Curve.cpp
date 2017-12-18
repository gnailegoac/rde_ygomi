/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Poly3Curve.cpp
 * @brief
 *******************************************************************************
 */

#include "Poly3Curve.h"

Model::Poly3Curve::Poly3Curve():
    Curve()
{
    memset(mX, 0, 4 * sizeof(double));
    memset(mY, 0, 4 * sizeof(double));
    memset(mZ, 0, 4 * sizeof(double));
}

Model::Poly3Curve::~Poly3Curve()
{

}

void Model::Poly3Curve::SetX(const double& aX, const size_t& aIndex)
{
    if (aIndex < 4)
    {
        mX[aIndex] = aX;
    }
}

const double& Model::Poly3Curve::GetX(const size_t& aIndex) const
{
    if (aIndex < 4)
    {
        return mX[aIndex];
    }

    return DBL_NAN;
}

void Model::Poly3Curve::SetY(const double& aY, const size_t& aIndex)
{
    if (aIndex < 4)
    {
        mY[aIndex] = aY;
    }
}

const double& Model::Poly3Curve::GetY(const size_t& aIndex) const
{
    if (aIndex < 4)
    {
        return mY[aIndex];
    }

    return DBL_NAN;
}

void Model::Poly3Curve::SetZ(const double& aZ, const size_t& aIndex)
{
    if (aIndex < 4)
    {
        mZ[aIndex] = aZ;
    }
}

const double& Model::Poly3Curve::GetZ(const size_t& aIndex) const
{
    if (aIndex < 4)
    {
        return mZ[aIndex];
    }

    return DBL_NAN;
}
