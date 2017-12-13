/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    ConnectionRelation.cpp
 * @brief
 *******************************************************************************
 */

#include "BoundingBox.h"

BoundingBox::BoundingBox():
    mPosition(),
    mHeading(0.),
    mWidth(0.),
    mLength(0.),
    mHeight(0.)
{

}

BoundingBox::BoundingBox(Point3D aPosition,
                         double aHeading,
                         float aWidth,
                         float aLength,
                         float aHeight):
    mPosition(aPosition),
    mHeading(aHeading),
    mWidth(aWidth),
    mLength(aLength),
    mHeight(aHeight)
{

}

BoundingBox::~BoundingBox()
{

}

Point3D BoundingBox::GetPosition() const
{
    return mPosition;
}

void BoundingBox::SetPosition(Point3D aPosition)
{
    mPosition = aPosition;
}

double BoundingBox::GetHeading() const
{
    return mHeading;
}

void BoundingBox::SetHeading(double aHeading)
{
    mHeading = aHeading;
}

float BoundingBox::GetWidth() const
{
    return mWidth;
}

void BoundingBox::SetWidth(float aWidth)
{
    mWidth = aWidth;
}

float BoundingBox::GetLength() const
{
    return mLength;
}

void BoundingBox::SetLength(float aLength)
{
    mLength = aLength;
}

float BoundingBox::GetHeight() const
{
    return mHeight;
}

void BoundingBox::SetHeight(float aHeight)
{
   mHeight = aHeight;
}
