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
#include <QDebug>

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

void Controller::RoadEditCommand::execute(const PureMVC::Interfaces::INotification& aNotification)
{
    if (ApplicationFacade::MERGE_ROAD == aNotification.getName())
    {
        std::pair<uint64_t, uint64_t> selectPair =
                *CommonFunction::ConvertToNonConstType<std::pair<uint64_t, uint64_t>>(aNotification.getBody());
        qDebug() << "To merge road " << selectPair.first << "and " << selectPair.second;
        mergeRoad(selectPair.first, selectPair.second);
    }
    else if (ApplicationFacade::ADD_LINE_TO_ROAD == aNotification.getName())
    {
        QJsonObject lineData = *CommonFunction::ConvertToNonConstType<QJsonObject>(aNotification.getBody());
        const std::shared_ptr<Model::MemoryModel>& memoryModel = getMainProxy()->GetMemoryModel();
        uint64_t roadId = lineData["roadId"].toString().toULongLong();
        Model::RoadPtr road = memoryModel->GetRoadById(roadId);
        addLineToRoad(memoryModel, road, lineData);
    }
}

std::string Controller::RoadEditCommand::GetCommandName()
{
    return "RoadEditCommand";
}

void Controller::RoadEditCommand::mergeRoad(const uint64_t& aFromRoadId, const uint64_t& aToRoadId)
{
    // The pointer should all be valid if come here.
    // No need to check if the two roads are in the same tile.
    // We will convert the points' geodetic coordinate to the same relative coordinate.
    const std::shared_ptr<Model::MemoryModel>& memoryModel = getMainProxy()->GetMemoryModel();
    const std::shared_ptr<Model::SceneModel>& sceneModel = getMainProxy()->GetSceneModel();
    std::shared_ptr<Model::Road> fromRoad = memoryModel->GetRoadById(aFromRoadId);
    std::shared_ptr<Model::Road> toRoad = memoryModel->GetRoadById(aToRoadId);
    if (canRoadsBeMerged(fromRoad, toRoad))
    {
        // Remove road from scene model
        sceneModel->RemoveRoadFromScene(aFromRoadId);
        sceneModel->RemoveRoadFromScene(aToRoadId);
        size_t laneCount = fromRoad->GetLaneListSize();
        const Model::TilePtr& tile = fromRoad->GetTile();
        // Merge roads, update road1, remove road2
        // 1. Merge line
        QVector<uint64_t> lineIdVec;
        for (const auto& lane : *(fromRoad->GetLaneList()))
        {
            uint64_t successorLaneId = lane->GetSuccessorLaneId();
            Model::LanePtr successorLane = toRoad->GetLaneById(successorLaneId);
            mergeLine(lineIdVec, lane->GetLeftLine(), successorLane->GetLeftLine());
            mergeLine(lineIdVec, lane->GetRightLine(), successorLane->GetRightLine());
            mergeLine(lineIdVec, lane->GetCenterLine(), successorLane->GetCenterLine());
            mergeLine(lineIdVec, lane->GetAvgSlamTrace(), successorLane->GetAvgSlamTrace());
        }
        // 2. Update lane predecessor/successor connection
        // Update successor of lanes in fromRoad and predecessor of lanes in toRoad's successor
        updateRoadConnection(fromRoad, toRoad);
        // Remove toRoad from memory model

        // Update scene model
        // Add merged fromRoad to scene model again
        sceneModel->AddRoadToScene(fromRoad);
        // Update tree model
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
    qDebug() << "To merge line " << fromLineId << " and " << aToLine->GetLineId();
    // Keep the curve type, modify curve length.
    const Model::PaintListPtr& fromPaintList = aFromLine->GetGeodeticPointsList();
    const Model::PaintListPtr& toPaintList = aToLine->GetGeodeticPointsList();
    // Convert to relative coordinates
    std::shared_ptr<Model::NurbsCurve> mergedCurve = mergePaintList(aFromLine->GetLane()->GetRoad()->GetTile()->GetReferencePoint(),
                                                          fromPaintList, toPaintList);
    aFromLine->GetMutableCurveList()->clear();
    aFromLine->GetMutableCurveList()->push_back(mergedCurve);
    aFromLine->SetLength(mergedCurve->GetLength());
    const double samplingInterval = 0.5;
    aFromLine->CreateGeodeticPointsList(aFromLine->GetLane()->GetRoad()->GetTile()->GetReferencePoint(), samplingInterval);
    // Convert geodetic coordinates into ECEF coordinates
    auto ecef = CRS::Factory().CreateEcefProjection(
                               CRS::CoordinateType::Wgs84,
                               CRS::CoordinateType::Ecef);
    aFromLine->GenerateViewPaintMap(ecef);
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
//    convertWgs84ToRelative(aReferencePoint, aFromPaint);
//    std::string errorInfo;
//    std::shared_ptr<Model::NurbsCurve> fitCurve = Model::FitNurbs::FitPointsToCurve(aFromPaint, NurbsOrder, errorInfo);
//    return fitCurve;
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

void Controller::RoadEditCommand::addLineToRoad(const std::shared_ptr<Model::MemoryModel>& aMemoryModel,
                                                Model::RoadPtr& aRoad, const QJsonObject& aData)
{
    uint64_t roadId = aRoad->GetRoadId();
    QString lineType = aData["type"].toString();
    QString position = aData["position"].toString();
    QJsonArray paintArray = aData["line"].toArray();
    uint64_t laneId = aMemoryModel->GenerateNewLaneId(roadId);
    uint64_t lineId = aMemoryModel->GenerateNewLineId(roadId);
    uint64_t curveId = aMemoryModel->GenerateNewCurveId(lineId);
    const Model::Point3DPtr& referencePoint = aRoad->GetTile()->GetReferencePoint();
    std::shared_ptr<Model::NurbsCurve> curve = getFittedCurve(referencePoint, paintArray);
    if (nullptr == curve)
    {
        QString message = "Too few points to fit to a curve.";
        ApplicationFacade::SendNotification(ApplicationFacade::NOTIFY_RESULT, &message);
        return;
    }
    curve->SetCurveId(curveId);
    curve->SetIndexInLine(0);
    curve->SetCurveType(lineType == "solid" ? Model::CurveType::Solid : Model::CurveType::Dashed);
    Model::LinePtr line = std::make_shared<Model::Line>();
    line->SetLineId(lineId);
    line->SetLength(curve->GetLineLength());
    line->GetMutableCurveList()->push_back(curve);
    Model::LanePtr lane = std::make_shared<Model::Lane>();
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
    }
    else
    {
        Model::LanePtr rightMostLane = Model::GetRightMostLane(aRoad);
        lane->SetLeftLine(rightMostLane->GetRightLine());
        lane->SetRightLine(line);
        rightMostLane->SetRightLaneId(laneId);
        lane->SetLeftLaneId(rightMostLane->GetLaneId());
    }
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
                    relativePaint->push_back(std::make_shared<Model::Point3D>(x, y, z ));
                }
            }
            paintList->push_back(relativePaint);
        }
    }
    std::string errorInfo;
    std::shared_ptr<Model::NurbsCurve> fitCurve = Model::FitNurbs::FitPointsToCurve(paintList, NurbsOrder, errorInfo);
    return fitCurve;
}
