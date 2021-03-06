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

#include <memory>

#include "CommonFunction.h"
#include "facade/ApplicationFacade.h"
#include "model/FitNurbs.h"
#include "model/Lane.h"
#include "model/MemoryModel.h"
#include "model/SceneModel.h"
#include "model/RoadModelUtilities.h"
#include "model/Utilities.h"
#include "proxy/MainProxy.h"
#include "view/MainWindow.h"

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
    else if (ApplicationFacade::ADD_LINE_TO_ROAD == aNotification.getName())
    {
        ApplicationFacade::SendNotification(ApplicationFacade::DEHIGHLIGHT_ALL_NODE);
        QJsonObject lineData = *CommonFunction::ConvertToNonConstType<QJsonObject>(aNotification.getBody());
        const std::shared_ptr<Model::MemoryModel>& memoryModel = getMainProxy()->GetMemoryModel();
        const std::shared_ptr<Model::SceneModel>& sceneModel = getMainProxy()->GetSceneModel();
        uint64_t roadId = lineData["roadId"].toString().toULongLong();
        Model::RoadPtr road = memoryModel->GetRoadById(roadId);
        sceneModel->RemoveRoadFromScene(roadId);
        addLineToRoad(memoryModel, road, lineData);
        sceneModel->AddRoadToScene(road);
        getMainWindow()->centralWidget()->repaint();
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
    aFromLine->CreateGeodeticPointsList(aFromLine->GetLane()->GetRoad()->GetTile()->GetReferencePoint(), SamplingInterval);
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
    return getFittedCurve(aReferencePoint, aFromPaint);
}

Model::PaintListPtr Controller::RoadEditCommand::convertWgs84ToRelative(const Model::Point3DPtr& aReferencePoint,
                                                                        const Model::PaintListPtr& aPaintList)
{
    auto wgs84ToRelative = CRS::Factory().CreateRelativeTransform(
                                            CRS::CoordinateType::Wgs84,
                                            CRS::CoordinateType::Relative,
                                            aReferencePoint->GetX(),
                                            aReferencePoint->GetY(),
                                            aReferencePoint->GetZ());
    Model::PaintListPtr paintList = std::make_shared<Model::PaintList>();
    for (auto& point3DList : *aPaintList)
    {
        Model::Point3DListPtr pointList = std::make_shared<Model::Point3DList>();
        for (auto& point : *point3DList)
        {
            double x = point->GetX();
            double y = point->GetY();
            double z = point->GetZ();
            wgs84ToRelative->Transform(x, y, z);
            pointList->push_back(std::make_shared<Model::Point3D>(x, y, z));
        }
        paintList->push_back(pointList);
    }

    return paintList;
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

    return true;
}

bool Controller::RoadEditCommand::canRoadsBeMerged(Model::RoadPtr& aFromRoad, Model::RoadPtr& aToRoad)
{
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

void Controller::RoadEditCommand::addLineToRoad(const std::shared_ptr<Model::MemoryModel>& aMemoryModel,
                                                Model::RoadPtr& aRoad, const QJsonObject& aData)
{
    const Model::Point3DPtr& referencePoint = aRoad->GetTile()->GetReferencePoint();
    QJsonArray paintArray = aData["line"].toArray();
    std::shared_ptr<Model::NurbsCurve> curve = getFittedCurve(referencePoint, paintArray);
    if (nullptr == curve)
    {
        QString message = "Too few points to fit to a curve.";
        ApplicationFacade::SendNotification(ApplicationFacade::NOTIFY_RESULT, &message);
        return;
    }
    uint64_t roadId = aRoad->GetRoadId();
    QString lineType = aData["type"].toString();
    QString position = aData["position"].toString();
    uint64_t laneId = aMemoryModel->GenerateNewLaneId(roadId);
    uint64_t lineId = aMemoryModel->GenerateNewLineId(roadId);
    uint64_t curveId = aMemoryModel->GenerateNewCurveId(lineId);
    curve->SetCurveId(curveId);
    curve->SetIndexInLine(0);
    curve->SetLength(curve->GetLineLength());
    curve->SetCurveType(lineType == "solid" ? Model::CurveType::Solid : Model::CurveType::Dashed);
    Model::LinePtr line = aRoad->GetTile()->GetMutableLine(lineId);
    line->SetLineId(lineId);
    line->SetLength(curve->GetLineLength());
    line->GetMutableCurveList()->push_back(curve);
    line->CreateGeodeticPointsList(referencePoint, SamplingInterval);
    line->GetMutablePaintListMap()->clear();
    Model::LanePtr lane = aRoad->GetTile()->GetMutableLane(laneId);
    lane->SetLaneId(laneId);
    lane->SetRoad(aRoad);
    aRoad->GetMutableLaneList()->push_back(lane);
    line->SetLane(lane);
    // Only set left/right lane, predecessor/successor should be edited on TreeView.
    if (position == "left")
    {
        Model::LanePtr leftMostLane = Model::GetLeftMostLane(aRoad);
        lane->SetLeftLine(line);
        lane->SetRightLine(leftMostLane->GetLeftLine());
        leftMostLane->SetLeftLaneId(laneId);
        lane->SetRightLaneId(leftMostLane->GetLaneId());
        leftMostLane->GetLeftLine()->SetLane(lane);
    }
    else
    {
        Model::LanePtr rightMostLane = Model::GetRightMostLane(aRoad);
        lane->SetLeftLine(rightMostLane->GetRightLine());
        lane->SetRightLine(line);
        rightMostLane->SetRightLaneId(laneId);
        lane->SetLeftLaneId(rightMostLane->GetLaneId());
    }
    // AvgSlamTrace
    std::shared_ptr<Model::NurbsCurve> avgCurve = getFittedCurve(referencePoint, paintArray);
    uint64_t avgLineId = aMemoryModel->GenerateNewLineId(roadId);
    uint64_t avgCurveId = aMemoryModel->GenerateNewCurveId(avgLineId);
    avgCurve->SetCurveId(avgCurveId);
    avgCurve->SetIndexInLine(0);
    avgCurve->SetLength(avgCurve->GetLineLength());
    avgCurve->SetCurveType(Model::CurveType::SlamTrace);
    Model::LinePtr avgLine = aRoad->GetTile()->GetMutableLine(avgLineId);
    avgLine->SetLineId(avgLineId);
    avgLine->SetLength(avgCurve->GetLineLength());
    avgLine->GetMutableCurveList()->push_back(avgCurve);
    lane->SetAvgSlamTrace(avgLine);
    avgLine->SetLane(lane);
}

std::shared_ptr<Model::NurbsCurve> Controller::RoadEditCommand::getFittedCurve(const Model::Point3DPtr& aReferencePoint,
                                                                               const Model::PaintListPtr& aPaintList)
{
    Model::PaintListPtr paintList = convertWgs84ToRelative(aReferencePoint, aPaintList);
    std::string errorInfo;
    std::shared_ptr<Model::NurbsCurve> fitCurve = Model::FitNurbs::FitPointsToCurve(paintList, NurbsOrder, errorInfo);
    return fitCurve;
}

std::shared_ptr<Model::NurbsCurve> Controller::RoadEditCommand::getFittedCurve(const Model::Point3DPtr& aReferencePoint,
                                                                               const QJsonArray& aPaintArray)
{
    auto wgs84ToRelative = CRS::Factory().CreateRelativeTransform(
                                            CRS::CoordinateType::Wgs84,
                                            CRS::CoordinateType::Relative,
                                            aReferencePoint->GetX(),
                                            aReferencePoint->GetY(),
                                            aReferencePoint->GetZ());
    Model::PaintListPtr paintList = std::make_shared<Model::PaintList>();
    for (const auto& paint : aPaintArray)
    {
        if (paint.isArray())
        {
            Model::Point3DListPtr relativePaint = std::make_shared<Model::Point3DList>();
            for (const auto& point : paint.toArray())
            {
                if (point.isArray())
                {
                    double x = point.toArray().at(0).toDouble();
                    double y = point.toArray().at(1).toDouble();
                    double z = aReferencePoint->GetZ();
                    wgs84ToRelative->Transform(x, y, z);
                    relativePaint->push_back(std::make_shared<Model::Point3D>(x, y, z));
                }
            }
            paintList->push_back(relativePaint);
        }
    }
    std::string errorInfo;
    std::shared_ptr<Model::NurbsCurve> fitCurve = Model::FitNurbs::FitPointsToCurve(paintList, NurbsOrder, errorInfo);
    return fitCurve;
}
