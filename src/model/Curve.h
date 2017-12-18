/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Curve.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Common.h"

namespace Model
{

typedef std::shared_ptr<std::vector<std::pair<double, double>>> PaintRangePtr;
typedef std::shared_ptr<const std::vector<std::pair<double, double>>> PaintRangeConstPtr;

enum class CurveType : std::uint8_t
{
    Solid = 0,
    Dashed = 1,
    Imputed = 2,
    SlamTrace = 3,
    UnKnown = 200,
    UnDefined = 201
};

class Curve
{
public:
    Curve();
    ~Curve();

    const std::uint64_t& GetCurveId() const;
    void SetCurveId(std::uint64_t aCurveId);

    float GetLength() const;
    void SetLength(float aLength);

    CurveType GetCurveType() const;
    void SetCurveType(CurveType aCurveType);

    std::int8_t GetIndexInLine() const;
    void SetIndexInLine(std::int8_t aIndexInLine);

    float GetWidth() const;
    void SetWidth(float aWidth);

    PaintRangeConstPtr GetPaintRange();
    PaintRangePtr GetMutablePaintRange();
    size_t GetPaintRangeSize() const;
    std::pair<double, double> GetPaintRange(const size_t& aIndex) const;

protected:
    std::uint64_t mCurveId;
    float mLength;
    CurveType mCurveType;
    std::int8_t mIndexInLine;
    float mWidth;
    PaintRangePtr mPaintRange;
};

typedef std::shared_ptr<Curve> CurvePtr;
typedef std::shared_ptr<const Curve> CurveConstPtr;
typedef std::vector<CurvePtr> CurveList;
typedef std::shared_ptr<CurveList> CurveListPtr;
typedef std::shared_ptr<const CurveList> CurveListConstPtr;

}
