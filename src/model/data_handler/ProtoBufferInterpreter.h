/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    ProtoBufferInterpreter.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include "RoadSection.pb.h"

#include "../Curve.h"
#include "../MemoryModel.h"

namespace Model
{

typedef std::shared_ptr<MemoryModel> MemoryModelPtr;

class ProtoBufferInterpreter
{
public:
    ProtoBufferInterpreter(const std::string& aFileName, const double& aInterval = 0.5);
    ~ProtoBufferInterpreter();

    bool SaveRoadSections(const MemoryModelPtr& aMemoryModel);

private:
    LanePtr getLeftMostLane(const RoadPtr& aRoad);
    LanePtr getRightLane(const LanePtr& aLane);
    std::uint32_t getLaneIndex(const LanePtr& aLane);
    CurveType getLineType(const LinePtr& aLine);
    LaneBoundaryType convertLineType(const CurveType& aCurveType);
    LanePtr getPredecessorLane(const LanePtr& aLane, const TileMapPtr& aTileMap);
    void saveRoad(RoadSection* aRoadSection, const RoadPtr& aRoad, const TileMapPtr& aTileMap);
    void saveLane(::Lane* aLanePb, const LanePtr& aLane, const TileMapPtr& aTileMap);
    void saveLaneBoundary(LaneBoundary* aBoundary, const LinePtr& aLine);
    void saveGeometry(Geometry* aGeometry, const LinePtr& aLine);

private:
    std::string mFileName;
    double mInterval;
};

typedef std::shared_ptr<ProtoBufferInterpreter> ProtoBufferInterpreterPtr;

}
