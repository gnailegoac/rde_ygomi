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

Model::BoundingBox::BoundingBox():
    mPosition(std::make_shared<Point3D>()),
    mHeading(0.),
    mWidth(0.),
    mLength(0.),
    mHeight(0.)
{

}

Model::BoundingBox::BoundingBox(const Point3DPtr& aPosition,
                                const double &aHeading,
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

Model::BoundingBox::~BoundingBox()
{

}

const Model::Point3DPtr& Model::BoundingBox::GetPosition() const
{
    return mPosition;
}

void Model::BoundingBox::SetPosition(const Point3DPtr& aPosition)
{
    mPosition = aPosition;
}

const double& Model::BoundingBox::GetHeading() const
{
    return mHeading;
}

void Model::BoundingBox::SetHeading(const double& aHeading)
{
    mHeading = aHeading;
}

float Model::BoundingBox::GetWidth() const
{
    return mWidth;
}

void Model::BoundingBox::SetWidth(float aWidth)
{
    mWidth = aWidth;
}

float Model::BoundingBox::GetLength() const
{
    return mLength;
}

void Model::BoundingBox::SetLength(float aLength)
{
    mLength = aLength;
}

float Model::BoundingBox::GetHeight() const
{
    return mHeight;
}

void Model::BoundingBox::SetHeight(float aHeight)
{
   mHeight = aHeight;
}
