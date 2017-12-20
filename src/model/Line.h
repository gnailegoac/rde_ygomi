/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Line.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Element.h"
#include "Point3D.h"
#include "Curve.h"

namespace Model
{

class Lane;

typedef std::shared_ptr<Lane> LanePtr;
typedef std::shared_ptr<const Lane> LaneConstPtr;

class Line : public Element
{
public:
    Line();
    ~Line();

    const std::uint64_t& GetLineId() const;
    void SetLineId(const std::uint64_t& aLineId);

    const CurveListPtr& GetCurveList() const;
    CurveListPtr GetMutableCurveList();
    size_t GetCurveListSize() const;
    CurvePtr GetCurve(const size_t& aIndex);

    float GetConfidence() const;
    void SetConfidence(float aConfidence);

    float GetLength() const;
    void SetLength(float aLength);

    const LanePtr& GetLane() const;
    void SetLane(const LanePtr& aLane);

    const Point3DListPtr& GetGeodeticPoints() const;
    Point3DListPtr GetMutableGeodeticPoints();
    void SetGeodeticPoints(const Point3DListPtr& aGeodeticPoints);

    const ViewPointMapPtr& GetPointListMap() const;
    ViewPointMapPtr GetMutablePointListMap();
    Point3DListPtr GetPointListByLevel(std::uint8_t aLevel);
    void GenerateViewPointMap();

private:
    std::uint64_t mLineId;
    CurveListPtr mCurveList;
    float mConfidence;
    float mLength;
    LanePtr mLane;
    Point3DListPtr mGeodeticPoints;
    ViewPointMapPtr mPointListMap;
};

typedef std::shared_ptr<Line> LinePtr;
typedef std::shared_ptr<const Line> LineConstPtr;
typedef std::vector<LinePtr> LineList;
typedef std::shared_ptr<LineList> LineListPtr;
typedef std::shared_ptr<const LineList> LineListConstPtr;

typedef std::unordered_map<std::uint64_t, LinePtr> LineMap;
typedef std::shared_ptr<LineMap> LineMapPtr;
typedef std::shared_ptr<const LineMap> LineMapConstPtr;

}
