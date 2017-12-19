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

namespace Model
{

class TrafficSign : public Element
{
public:
    TrafficSign();
    ~TrafficSign();

    const std::uint64_t& GetTrafficSignId() const;
    void SetTrafficSignId(const std::uint64_t& aTrafficSignId);

    const std::uint64_t& GetTrafficSignType() const;
    void SetTrafficSignType(const std::uint64_t& aTrafficSignType);

    std::int32_t GetOrientation() const;
    void SetOrientation(std::int32_t aOrientation);

    const double& GetShapeWidth() const;
    void SetShapeWidth(const double& aShapeWidth);

    const double& GetShapeHeight() const;
    void SetShapeHeight(const double& aShapeHeight);

    float GetConfidence() const;
    void SetConfidence(float aConfidence);

    const Point3DPtr& GetPosition() const;
    void SetPosition(const Point3DPtr& aPosition);

private:
    std::uint64_t mTrafficSignId;
    std::uint64_t mTrafficSignType;
    std::int32_t mOrientation;
    double mShapeWidth;
    double mShapeHeight;
    float mConfidence;
    Point3DPtr mPosition;
};

typedef std::shared_ptr<TrafficSign> TrafficSignPtr;
typedef std::shared_ptr<const TrafficSign> TrafficSignConstPtr;
typedef std::vector<TrafficSignPtr> TrafficSignList;
typedef std::shared_ptr<TrafficSignList> TrafficSignListPtr;
typedef std::shared_ptr<const TrafficSignList> TrafficSignListConstPtr;

typedef std::unordered_map<std::uint64_t, TrafficSignPtr> TrafficSignMap;
typedef std::shared_ptr<TrafficSignMap> TrafficSignMapPtr;
typedef std::shared_ptr<const TrafficSignMap> TrafficSignMapConstPtr;

}
