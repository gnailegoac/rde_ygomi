/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    TrafficSign.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Common.h"
#include "Element.h"
#include "Point3D.h"


class TrafficSign : public Element
{
public:
    TrafficSign();
    ~TrafficSign();

    std::uint64_t GetTrafficSignType() const;
    void SetTrafficSignType(std::uint64_t aTrafficSignType);

    std::int8_t GetOrientation() const;
    void SetOrientation(std::int8_t aOrientation);

    double GetShapeWidth() const;
    void SetShapeWidth(double aShapeWidth);

    double GetShapeHeight() const;
    void SetShapeHeight(double aShapeHeight);

    float GetConfidence() const;
    void SetConfidence(double aConfidence);

    Point3D GetPosition() const;
    void SetPosition(Point3D aPosition);

private:
    std::uint64_t mTrafficSignType;
    std::int8_t mOrientation;
    double mShapeWidth;
    double mShapeHeight;
    float mConfidence;
    Point3D mPosition;
};
