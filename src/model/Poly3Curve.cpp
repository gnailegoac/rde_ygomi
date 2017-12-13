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

Poly3Curve::Poly3Curve():
    Curve()
{
    memset(mX, 0, 4 * sizeof(double));
    memset(mY, 0, 4 * sizeof(double));
    memset(mZ, 0, 4 * sizeof(double));
}

Poly3Curve::~Poly3Curve()
{

}

void Poly3Curve::SetX(double aX, size_t aIndex)
{
    if (aIndex < 4)
    {
        mX[aIndex] = aX;
    }
}

double Poly3Curve::GetX(size_t aIndex) const
{
    if (aIndex < 4)
    {
        return mX[aIndex];
    }

    return DBL_NAN;
}

void Poly3Curve::SetY(double aY, size_t aIndex)
{
    if (aIndex < 4)
    {
        mY[aIndex] = aY;
    }
}

double Poly3Curve::GetY(size_t aIndex) const
{
    if (aIndex < 4)
    {
        return mY[aIndex];
    }

    return DBL_NAN;
}

void Poly3Curve::SetZ(double aZ, size_t aIndex)
{
    if (aIndex < 4)
    {
        mZ[aIndex] = aZ;
    }
}

double Poly3Curve::GetZ(size_t aIndex) const
{
    if (aIndex < 4)
    {
        return mZ[aIndex];
    }

    return DBL_NAN;
}
