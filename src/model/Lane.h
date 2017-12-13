/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Lane.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Common.h"
#include "Element.h"
#include "Line.h"

class Road;

class Lane : public Element
{
public:
    Lane();
    ~Lane();

    std::uint64_t GetLaneId() const;
    void SetLaneId(std::uint64_t aLaneId);

    std::shared_ptr<Line> GetLeftLine() const;
    void SetLeftLine(std::shared_ptr<Line> aLeftLine);

    std::shared_ptr<Line> GetRightLine() const;
    void SetRightLine(std::shared_ptr<Line> aRightLine);

    std::shared_ptr<Line> GetCenterLine() const;
    void SetCenterLine(std::shared_ptr<Line> aCenterLine);

    const std::unordered_map<std::uint64_t, std::uint64_t>& GetConnectionMap();
    std::unordered_map<std::uint64_t, std::uint64_t>* GetMutableConnectionMap();
    std::uint64_t GetConnectionById(uint64_t aId);

    std::shared_ptr<Road> GetRoad() const;
    void SetRoad(std::shared_ptr<Road> aRoad);

private:
    std::uint64_t mLaneId;
    std::shared_ptr<Line> mLeftLine;
    std::shared_ptr<Line> mRightLine;
    std::shared_ptr<Line> mCenterLine;
    std::unordered_map<std::uint64_t, std::uint64_t> mConnectionMap;
    std::shared_ptr<Road> mRoad;
};
