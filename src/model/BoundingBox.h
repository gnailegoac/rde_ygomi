/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    BoundingBox.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Common.h"
#include "Point3D.h"

namespace Model
{

class BoundingBox
{
public:
    BoundingBox();
    BoundingBox(Point3D aPosition,
                double aHeading,
                float aWidth,
                float aLength,
                float aHeight);
    ~BoundingBox();

    Point3D GetPosition() const;
    void SetPosition(Point3D aPosition);

    double GetHeading() const;
    void SetHeading(double aHeading);

    float GetWidth() const;
    void SetWidth(float aWidth);

    float GetLength() const;
    void SetLength(float aLength);

    float GetHeight() const;
    void SetHeight(float aHeight);

private:
    Point3D mPosition;
    double mHeading;
    float mWidth;
    float mLength;
    float mHeight;
};

}
