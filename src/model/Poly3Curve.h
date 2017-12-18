/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Poly3Curve.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Common.h"
#include "Curve.h"

namespace Model
{

class Poly3Curve : public Curve
{
public:
    Poly3Curve();
    ~Poly3Curve();

    void SetX(double aX, size_t aIndex);
    double GetX(size_t aIndex) const;

    void SetY(double aY, size_t aIndex);
    double GetY(size_t aIndex) const;

    void SetZ(double aZ, size_t aIndex);
    double GetZ(size_t aIndex) const;

private:
    double mX[4];
    double mY[4];
    double mZ[4];
};

typedef std::shared_ptr<Poly3Curve> Poly3CurvePtr;
typedef std::shared_ptr<const Poly3Curve> Poly3CurveConstPtr;
typedef std::vector<Poly3CurvePtr> Poly3CurveList;
typedef std::shared_ptr<Poly3CurveList> Poly3CurveListPtr;
typedef std::shared_ptr<const Poly3CurveList> Poly3CurveListConstPtr;

}
