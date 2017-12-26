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
    Point3D(const double& aX, const double& aY, const double& aZ);
    Point3D(const Point3D& aPoint);
    ~Point3D();

    const double& GetX() const;
    void SetX(const double& aX);

    const double& GetY() const;
    void SetY(const double& aY);

    const double& GetZ() const;
    void SetZ(const double& aZ);

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

typedef std::vector<Point3DListPtr> PaintList;
typedef std::shared_ptr<PaintList> PaintListPtr;

typedef std::unordered_map<uint8_t, Point3DListPtr> ViewPointMap;
typedef std::shared_ptr<ViewPointMap> ViewPointMapPtr;
typedef std::shared_ptr<const ViewPointMap> ViewPointMapConstPtr;

typedef std::unordered_map<uint8_t, PaintListPtr> ViewPaintMap;
typedef std::shared_ptr<ViewPaintMap> ViewPaintMapPtr;
typedef std::shared_ptr<const ViewPaintMap> ViewPaintMapConstPtr;

}
