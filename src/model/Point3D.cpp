/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Point3D.cpp
 * @brief
 *******************************************************************************
 */

#include "Point3D.h"

Model::Point3D::Point3D():
    mX(0.0),
    mY(0.0),
    mZ(0.0)
{

}

Model::Point3D::Point3D(const double& aX, const double& aY, const double& aZ):
    mX(aX),
    mY(aY),
    mZ(aZ)
{

}

Model::Point3D::Point3D(const Point3D& aPoint)
{
    mX = aPoint.GetX();
    mY = aPoint.GetY();
    mZ = aPoint.GetZ();
}

Model::Point3D::~Point3D()
{

}

const double& Model::Point3D::GetX() const
{
    return mX;
}

void Model::Point3D::SetX(const double& aX)
{
    mX = aX;
}

const double& Model::Point3D::GetY() const
{
    return mY;
}

void Model::Point3D::SetY(const double& aY)
{
    mY = aY;
}

const double& Model::Point3D::GetZ() const
{
    return mZ;
}

void Model::Point3D::SetZ(const double& aZ)
{
    mZ = aZ;
}