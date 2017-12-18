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
    BoundingBox(Point3DPtr aPosition,
                const double& aHeading,
                float aWidth,
                float aLength,
                float aHeight);
    ~BoundingBox();

    const Point3DPtr& GetPosition() const;
    void SetPosition(const Point3DPtr& aPosition);

    const double& GetHeading() const;
    void SetHeading(double aHeading);

    float GetWidth() const;
    void SetWidth(float aWidth);

    float GetLength() const;
    void SetLength(float aLength);

    float GetHeight() const;
    void SetHeight(float aHeight);

private:
    Point3DPtr mPosition;
    double mHeading;
    float mWidth;
    float mLength;
    float mHeight;
};

typedef std::shared_ptr<BoundingBox> BoundingBoxPtr;
typedef std::shared_ptr<const BoundingBox> BoundingBoxConstPtr;
typedef std::vector<BoundingBoxPtr> BoundingBoxList;
typedef std::shared_ptr<BoundingBoxList> BoundingBoxListPtr;
typedef std::shared_ptr<const BoundingBoxList> BoundingBoxListConstPtr;

}
