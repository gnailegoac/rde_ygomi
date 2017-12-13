/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Point3D.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Common.h"

class Point3D
{
public:
    Point3D();
    Point3D(double aX, double aY, double aZ);
    Point3D(const Point3D& aPoint);
    ~Point3D();

    double GetX() const;
    void SetX(double aX);

    double GetY() const;
    void SetY(double aY);

    double GetZ() const;
    void SetZ(double aZ);

private:
    double mX;
    double mY;
    double mZ;
};
