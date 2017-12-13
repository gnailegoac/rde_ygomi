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

class Line : public Element
{
public:
    Line();
    ~Line();

    std::uint64_t GetLineId() const;
    void SetLineId(std::uint64_t aLineId);

    const std::vector<std::shared_ptr<Curve>> GetCurveList() const;
    std::vector<std::shared_ptr<Curve>>* GetMutableCurveList();
    size_t GetCurveListSize() const;
    std::shared_ptr<Curve> GetCurve(size_t aIndex);

    float GetConfidence() const;
    void SetConfidence(float aConfidence);

    float GetLength() const;
    void SetLength(float aLength);

    std::shared_ptr<Lane> GetLane() const;
    void SetLane(std::shared_ptr<Lane> aLane);

    const std::unordered_map<std::uint8_t, std::vector<Point3D>> GetPointListMap() const;
    std::unordered_map<std::uint8_t, std::vector<Point3D>>* GetMutablePointListMap();
    std::vector<Point3D>* GetPointListByLevel(std::uint8_t aLevel);

private:
    std::uint64_t mLineId;
    std::vector<std::shared_ptr<Curve>> mCurveList;
    float mConfidence;
    float mLength;
    std::shared_ptr<Lane> mLane;
    std::unordered_map<std::uint8_t, std::vector<Point3D>> mPointListMap;
};

}
