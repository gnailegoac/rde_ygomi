/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    ProtoBufferInterpreter.cpp
 * @brief
 *******************************************************************************
 */

#include "ProtoBufferInterpreter.h"

#include <QDebug>
#include <fstream>

#include "model/RoadModelUtilities.h"

#include "CoordinateTransform/Factory.h"

static const std::map<Model::CurveType, LaneBoundaryType> scBoundaryTypeMap =
{
    {Model::CurveType::UnKnown, LANEBOUNDARYTYPE_UNKNOWN},
    {Model::CurveType::UnDefined, LANEBOUNDARYTYPE_MARKING_NONE},
    {Model::CurveType::Dashed, LANEBOUNDARYTYPE_MARKING_DASHED},
    {Model::CurveType::Solid, LANEBOUNDARYTYPE_MARKING_SOLID},
};

Model::ProtoBufferInterpreter::ProtoBufferInterpreter(const std::string& aFileName,
                                                      const double& aInterval):
    mFileName(aFileName),
    mInterval(aInterval),
    mIdGenerator(std::make_shared<IdGenerator>())
{

}

Model::ProtoBufferInterpreter::~ProtoBufferInterpreter()
{

}

bool Model::ProtoBufferInterpreter::SaveRoadSections(const Model::MemoryModelPtr& aMemoryModel)
{
    std::ofstream outStream;

    try
    {
        outStream.open(mFileName, std::ofstream::out | std::ofstream::binary);
        RoadSections roadSections;
        const TileMapPtr& tileMap = aMemoryModel->GetTileMap();

        for (const auto& tile : *tileMap)
        {
            const RoadMapPtr& roadMap = tile.second->GetRoadMap();
            for (const auto& road : *roadMap)
            {
                saveRoad(roadSections.add_roadsections(), road.second, tileMap);
            }
        }
        roadSections.SerializeToOstream(&outStream);
    }
    catch(...)
    {
        outStream.close();
        return false;
    }

    return true;
}

void Model::ProtoBufferInterpreter::saveRoad(RoadSection* aRoadSection,
                                             const Model::RoadPtr& aRoad,
                                             const Model::TileMapPtr& aTileMap)
{
    aRoadSection->set_sectionid(convertRoadId(aRoad->GetRoadId()));

    LaneModel *laneModel = aRoadSection->mutable_lanemodel();
    LanePtr lane = GetLeftMostLane(aRoad);

    if (lane)
    {
        // Currently, we use left line of left-most lane as the centerline of the road.
        // Maybe we could use the center line of the left-most line and right-most line.
        saveGeometry(aRoadSection->mutable_roadsectioncenterline(), lane->GetLeftLine());

        // TODO: LandMark. Now we have TrafficSign, but it belongs to a tile not a road.
        // so can not add LandMark, will add if TrafficSign belongs to a road.

        // number of lane_boundaries should be number of lanes plus 1.
        saveLane(laneModel->add_lanes(), lane, aTileMap);
        saveLaneBoundary(laneModel->add_boundaries(), lane->GetLeftLine());
        saveLaneBoundary(laneModel->add_boundaries(), lane->GetRightLine());

        while ((lane = GetRightLane(lane)) != nullptr)
        {
            saveLane(laneModel->add_lanes(), lane, aTileMap);
            saveLaneBoundary(laneModel->add_boundaries(), lane->GetRightLine());
        }
    }
}

void Model::ProtoBufferInterpreter::saveLane(::Lane* aLanePb,
                                             const Model::LanePtr& aLane,
                                             const Model::TileMapPtr& aTileMap)
{
    // TODO: Set Lane Type when we have

    aLanePb->set_laneid(getLaneIndex(aLane));

    // Set predecessor lane id, actually it is lane index in predecessor road
    // as predecessor lane may not in the same tile as current lane, search through tileMap.
    LanePtr predecessorLane = getPredecessorLane(aLane, aTileMap);

    if (predecessorLane != nullptr)
    {
        aLanePb->add_predecessorlaneids(getLaneIndex(predecessorLane));
        // Set predecessor road section id, first get predecessor lane, then use its road id.
        aLanePb->set_predecessorsectionid(convertRoadId(predecessorLane->GetRoad()->GetRoadId()));
    }

}

void Model::ProtoBufferInterpreter::saveLaneBoundary(LaneBoundary* aBoundary, const Model::LinePtr& aLine)
{
    Model::CurveType lineType = aLine->GetLineType();
    Geometry* geometry = aBoundary->mutable_geometry();

    aBoundary->set_type(convertLineType(lineType));
    saveGeometry(geometry, aLine);
}

void Model::ProtoBufferInterpreter::saveGeometry(Geometry* aGeometry, const Model::LinePtr& aLine)
{
    const Point3DPtr& referencePoint = aLine->GetLane()->GetRoad()->GetTile()->GetReferencePoint();
    std::shared_ptr<CRS::Factory> factory = std::make_shared<CRS::Factory>();
    std::unique_ptr<CRS::ICoordinateTransform> relativeToWgs84 =
                    factory->CreateRelativeTransform(CRS::CoordinateType::Relative,
                                                     CRS::CoordinateType::Wgs84,
                                                     referencePoint->GetX(),
                                                     referencePoint->GetY(),
                                                     referencePoint->GetZ());

    for (const auto& curve : *(aLine->GetCurveList()))
    {
        Point3DListPtr points = curve->CalculatePointCloud(mInterval);
        for (const auto& point : *points)
        {
            double x = point->GetX();
            double y = point->GetY();
            double z = point->GetZ();
            relativeToWgs84->Transform(x, y, z);
            GeometryPoint* geometryPoint = aGeometry->add_points();
            geometryPoint->set_lon(x);
            geometryPoint->set_lat(y);
            geometryPoint->set_height(z);
        }
    }
}

uint32_t Model::ProtoBufferInterpreter::getLaneIndex(const Model::LanePtr& aLane)
{
    std::uint32_t index = 0;
    RoadPtr road = aLane->GetRoad();
    LanePtr lane = GetLeftMostLane(road);

    while (nullptr != lane)
    {
        if (lane == aLane)
        {
            break;
        }
        ++index;
        lane = GetRightLane(lane);
    }

    return index;
}

LaneBoundaryType Model::ProtoBufferInterpreter::convertLineType(const Model::CurveType& aCurveType)
{
    if (scBoundaryTypeMap.count(aCurveType) > 0)
    {
        return scBoundaryTypeMap.at(aCurveType);
    }

    return LANEBOUNDARYTYPE_UNKNOWN;
}

uint32_t Model::ProtoBufferInterpreter::convertRoadId(const uint64_t& aRoadId)
{
    std::lock_guard<std::mutex> mutexGuard(mMutex);

    if (mRoadIdMap.count(aRoadId) == 0)
    {
        mRoadIdMap[aRoadId] = static_cast<uint32_t>(mIdGenerator->GetNewId());
    }

    return mRoadIdMap[aRoadId];
}

Model::LanePtr Model::ProtoBufferInterpreter::getPredecessorLane(const Model::LanePtr& aLane,
                                                                 const Model::TileMapPtr& aTileMap)
{
    std::uint64_t predecessorLaneId = aLane->GetPredecessorLaneId();

    for (const auto& tile : *aTileMap)
    {
        LanePtr lane = tile.second->GetLane(predecessorLaneId);
        if (lane != nullptr)
        {
            return lane;
        }
    }

    return nullptr;
}

