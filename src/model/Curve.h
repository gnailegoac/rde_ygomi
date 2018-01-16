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

#include "Point3D.h"

namespace Model
{

typedef std::shared_ptr<std::vector<std::pair<double, double>>> PaintRangePtr;
typedef std::shared_ptr<const std::vector<std::pair<double, double>>> PaintRangeConstPtr;

enum class CurveType : std::uint32_t
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

    virtual Point3DListPtr CalculatePointCloud(const double&);
    virtual PaintListPtr CalculatePaintPointCloud(const double&);

    virtual bool Convert(const std::string&);
    virtual std::string Parse() const;
    virtual bool FitPointsToCurve(std::string&);

    const std::uint64_t& GetCurveId() const;
    void SetCurveId(const std::uint64_t& aCurveId);

    float GetLength() const;
    void SetLength(float aLength);

    CurveType GetCurveType() const;
    std::string GetCurveTypeDescriptor() const;
    void SetCurveType(CurveType aCurveType);

    std::uint32_t GetIndexInLine() const;
    void SetIndexInLine(std::uint32_t aIndexInLine);

    std::uint8_t GetWidth() const;
    void SetWidth(std::uint8_t aWidth);

    const PaintRangePtr& GetPaintRange() const;
    PaintRangePtr GetMutablePaintRange();
    void SetPaintRange(const PaintRangePtr& aPaintRange);
    size_t GetPaintRangeSize() const;
    std::pair<double, double> GetPaintRange(const size_t& aIndex) const;

protected:
    std::uint64_t mCurveId;
    float mLength;
    CurveType mCurveType;
    std::uint32_t mIndexInLine;
    std::uint8_t mWidth;
    PaintRangePtr mPaintRange;
};

typedef std::shared_ptr<Curve> CurvePtr;
typedef std::shared_ptr<const Curve> CurveConstPtr;
typedef std::vector<CurvePtr> CurveList;
typedef std::shared_ptr<CurveList> CurveListPtr;
typedef std::shared_ptr<const CurveList> CurveListConstPtr;

}
