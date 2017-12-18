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

namespace Model
{

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

typedef std::shared_ptr<Point3D> Point3DPtr;
typedef std::shared_ptr<const Point3D> Point3DConstPtr;
typedef std::vector<Point3DPtr> Point3DList;
typedef std::shared_ptr<Point3DList> Point3DListPtr;
typedef std::shared_ptr<const Point3DList> Point3DListConstPtr;

typedef std::unordered_map<uint8_t, Point3DListPtr> ViewPointMap;
typedef std::shared_ptr<ViewPointMap> ViewPointMapPtr;
typedef std::shared_ptr<const ViewPointMap> ViewPointMapConstPtr;

}
