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

namespace  CRS
{

class ICoordinateTransform;

}

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
    void SortCurve();

    float GetConfidence() const;
    void SetConfidence(float aConfidence);

    float GetLength() const;
    void SetLength(float aLength);

    const LanePtr& GetLane() const;
    void SetLane(const LanePtr& aLane);

    const PaintListPtr& GetGeodeticPointsList() const;
    PaintListPtr GetMutableGeodeticPointsList();
    void SetGeodeticPointsList(const PaintListPtr& aGeodeticPointsList);
    void CreateGeodeticPointsList(const Point3DPtr& aReferencePoint,
                                  const double& aSamplingInterval);

    const ViewPaintMapPtr& GetPaintListMap() const;
    ViewPaintMapPtr GetMutablePaintListMap();
    PaintListPtr GetPaintListByLevel(std::uint8_t aLevel);
    PaintListPtr GetMutablePaintListByLevel(std::uint8_t aLevel);
    void GenerateViewPaintMap(std::unique_ptr<CRS::ICoordinateTransform>& aTransformer);

    CurveType GetLineType() const;

private:
    std::uint64_t mLineId;
    CurveListPtr mCurveList;
    float mConfidence;
    float mLength;
    LanePtr mLane;
    PaintListPtr mGeodeticPointsList;
    ViewPaintMapPtr mPaintListMap;
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
