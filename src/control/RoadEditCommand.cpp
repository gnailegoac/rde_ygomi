/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    RoadEditCommand.cpp
 * @brief
 *******************************************************************************
 */

#include "RoadEditCommand.h"

#include <CoordinateTransform/Factory.h>
#include <PureMVC/PureMVC.hpp>

#include "CommonFunction.h"
#include "facade/ApplicationFacade.h"
#include "model/FitNurbs.h"
#include "model/Lane.h"
#include "model/MemoryModel.h"
#include "model/SceneModel.h"
#include "proxy/MainProxy.h"

#include "model/Utilities.h"

void Controller::RoadEditCommand::execute(const PureMVC::Interfaces::INotification& aNotification)
{
    if (ApplicationFacade::MERGE_ROAD == aNotification.getName())
    {
        ApplicationFacade::SendNotification(ApplicationFacade::DEHIGHLIGHT_ALL_NODE);
        std::pair<uint64_t, uint64_t> selectPair =
                *CommonFunction::ConvertToNonConstType<std::pair<uint64_t, uint64_t>>(aNotification.getBody());
        mergeRoad(selectPair.first, selectPair.second);
        ApplicationFacade::SendNotification(ApplicationFacade::UPDATE_TREE_VIEW);
    }
}

std::string Controller::RoadEditCommand::GetCommandName()
{
    return "RoadEditCommand";
}

void Controller::RoadEditCommand::mergeRoad(const uint64_t& aFromRoadId, const uint64_t& aToRoadId)
{
    // No need to check if the two roads are in the same tile.
    // Will transform the points' geodetic coordinate in the same relative coordinate frame.
    const std::shared_ptr<Model::MemoryModel>& memoryModel = getMainProxy()->GetMemoryModel();
    const std::shared_ptr<Model::SceneModel>& sceneModel = getMainProxy()->GetSceneModel();
    std::shared_ptr<Model::Road> fromRoad = memoryModel->GetRoadById(aFromRoadId);
    std::shared_ptr<Model::Road> toRoad = memoryModel->GetRoadById(aToRoadId);
    if (canRoadsBeMerged(fromRoad, toRoad))
    {
        // Remove road from scene model
        sceneModel->RemoveRoadFromScene(aFromRoadId);
        sceneModel->RemoveRoadFromScene(aToRoadId);
        // Merge roads, update road1, remove road2
        QVector<uint64_t> lineIdVec;
        for (const auto& lane : *(fromRoad->GetLaneList()))
        {
            uint64_t successorLaneId = lane->GetSuccessorLaneId();
            Model::LanePtr successorLane = toRoad->GetLaneById(successorLaneId);
            // Merge line
            mergeLine(lineIdVec, lane->GetLeftLine(), successorLane->GetLeftLine());
            mergeLine(lineIdVec, lane->GetRightLine(), successorLane->GetRightLine());
            mergeLine(lineIdVec, lane->GetCenterLine(), successorLane->GetCenterLine());
            mergeLine(lineIdVec, lane->GetAvgSlamTrace(), successorLane->GetAvgSlamTrace());
        }
        // Update successor of lanes in fromRoad and predecessor of lanes in toRoad's successor
        updateRoadConnection(fromRoad, toRoad);
        // Remove toRoad from memory model
        memoryModel->RemoveRoad(toRoad);
        // Update scene model: Add merged fromRoad to scene model again
        sceneModel->AddRoadToScene(fromRoad);
    }
}

void Controller::RoadEditCommand::mergeLine(QVector<uint64_t>& aMergedLineIds,
                                            const Model::LinePtr& aFromLine, const Model::LinePtr& aToLine)
{
    if (aFromLine == nullptr || aToLine == nullptr)
    {
        return;
    }
    uint64_t fromLineId = aFromLine->GetLineId();
    if (aMergedLineIds.count(fromLineId) != 0)
    {
        return;
    }
    aMergedLineIds.push_back(fromLineId);
    const Model::PaintListPtr& fromPaintList = aFromLine->GetGeodeticPointsList();
    const Model::PaintListPtr& toPaintList = aToLine->GetGeodeticPointsList();
    // Convert to relative coordinates
    std::shared_ptr<Model::NurbsCurve> mergedCurve = mergePaintList(aFromLine->GetLane()->GetRoad()->GetTile()->GetReferencePoint(),
                                                          fromPaintList, toPaintList);
    uint64_t curveId = aFromLine->GetCurve(0)->GetCurveId();
    Model::CurveType curveType = aFromLine->GetCurve(0)->GetCurveType();
    aFromLine->GetMutableCurveList()->clear();
    // Set curve ID
    mergedCurve->SetCurveId(curveId);
    // Keep the curve type, modify curve length.
    mergedCurve->SetCurveType(curveType);
    mergedCurve->SetLength(mergedCurve->GetLineLength());
    aFromLine->GetMutableCurveList()->push_back(mergedCurve);
    aFromLine->SetLength(mergedCurve->GetLength());
    const double samplingInterval = 0.5;
    aFromLine->CreateGeodeticPointsList(aFromLine->GetLane()->GetRoad()->GetTile()->GetReferencePoint(), samplingInterval);
    // Clear the ViewPaintMap, will regenerate when needed.
    aFromLine->GetMutablePaintListMap()->clear();
}

std::shared_ptr<Model::NurbsCurve> Controller::RoadEditCommand::mergePaintList(
        const Model::Point3DPtr& aReferencePoint,
        const Model::PaintListPtr& aFromPaint,
        const Model::PaintListPtr& aToPaint)
{
    if ((aFromPaint->size() == 1) && (aToPaint->size() == 1))
    {
        // Both are solid, will merge to a new solid line.
        Model::Point3DListPtr point3DList = aFromPaint->front();
        Model::Point3DListPtr bPoint3DList = aToPaint->front();
        point3DList->insert(point3DList->end(), bPoint3DList->begin(), bPoint3DList->end());
    }
    else
    {
        // Merge to a dashed line.
        aFromPaint->insert(aFromPaint->end(), aToPaint->begin(), aToPaint->end());
    }
    convertWgs84ToRelative(aReferencePoint, aFromPaint);
    std::string errorInfo;
    std::shared_ptr<Model::NurbsCurve> fitCurve = Model::FitNurbs::FitPointsToCurve(aFromPaint, 3, errorInfo);
    return fitCurve;
}

void Controller::RoadEditCommand::convertWgs84ToRelative(const Model::Point3DPtr& aReferencePoint,
                                                         const Model::PaintListPtr& aPaintList)
{
    auto wgs84ToRelative = CRS::Factory().CreateRelativeTransform(
                                            CRS::CoordinateType::Wgs84,
                                            CRS::CoordinateType::Relative,
                                            aReferencePoint->GetX(),
                                            aReferencePoint->GetY(),
                                            aReferencePoint->GetZ());
    for (auto& point3DList : *aPaintList)
    {
        for (auto& point : *point3DList)
        {
            double x = point->GetX();
            double y = point->GetY();
            double z = point->GetZ();
            wgs84ToRelative->Transform(x, y, z);
            point->SetX(x);
            point->SetY(y);
            point->SetZ(z);
        }
    }
}

bool Controller::RoadEditCommand::isLaneInRoad(const Model::RoadPtr& aRoad, const uint64_t& aLaneId)
{
    for (const auto& lane : *(aRoad->GetLaneList()))
    {
        if (lane->GetLaneId() == aLaneId)
        {
            return true;
        }
    }

    return false;
}

bool Controller::RoadEditCommand::isRoadConnected(const Model::RoadPtr& aFromRoad, const Model::RoadPtr& aToRoad)
{
    // Check if predecessor of lanes in aToRoad is in aFromRoad
    for (const auto& lane : *(aToRoad->GetLaneList()))
    {
        uint64_t predecessorLaneId = lane->GetPredecessorLaneId();
        if (predecessorLaneId == 0 || !isLaneInRoad(aFromRoad, predecessorLaneId))
        {
            return false;
        }
    }
    // Is it enough? Should we check if successor of lanes in aFromRoad is in aToRoad?
    return true;
}

bool Controller::RoadEditCommand::canRoadsBeMerged(Model::RoadPtr& aFromRoad, Model::RoadPtr& aToRoad)
{
    // TODO: shall we check if connected lines have the same type?
    QString message;
    // Check if the roads to merge have the same number of lanes
    if (aFromRoad->GetLaneListSize() != aToRoad->GetLaneListSize())
    {
        message = "Only two roads with the same number of lanes can be merged.";
        ApplicationFacade::SendNotification(ApplicationFacade::NOTIFY_RESULT, &message);
        return false;
    }
    // check which road is in front, should make road1 goes to road2
    if (isRoadConnected(aFromRoad, aToRoad))
    {
        return true;
    }
    else if (isRoadConnected(aToRoad, aFromRoad))
    {
        Model::RoadPtr tempRoad = aFromRoad;
        aFromRoad = aToRoad;
        aToRoad = tempRoad;
        return true;
    }
    else
    {
        message = "Only two connected roads can be merged.";
        ApplicationFacade::SendNotification(ApplicationFacade::NOTIFY_RESULT, &message);
        return false;
    }
}

void Controller::RoadEditCommand::updateRoadConnection(Model::RoadPtr& aFromRoad, Model::RoadPtr& aToRoad)
{
    const std::shared_ptr<Model::MemoryModel>& memoryModel = getMainProxy()->GetMemoryModel();
    for (const auto& lane : *(aToRoad->GetLaneList()))
    {
        // predecessor MUST exist.
        uint64_t predecessorLaneId = lane->GetPredecessorLaneId();
        uint64_t successorLaneId = lane->GetSuccessorLaneId();
        if (predecessorLaneId > 0)
        {
            Model::LanePtr predecessorLane = aFromRoad->GetLaneById(predecessorLaneId);
            if (predecessorLane)
            {
                predecessorLane->SetSuccessorLaneId(successorLaneId);
            }
        }
        // should check if successor exist.
        if (successorLaneId > 0)
        {
            // The successorLane may in different tile, so use memoryModel here.
            Model::LanePtr successorLane = memoryModel->GetLaneById(successorLaneId);
            if (successorLane)
            {
                successorLane->SetPredecessorLaneId(predecessorLaneId);
            }
        }
    }
}
