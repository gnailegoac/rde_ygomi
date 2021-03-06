/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    RoadEditCommand.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "BasicCommand.h"
#include "model/Road.h"
#include "model/Point3D.h"
#include "model/NurbsCurve.h"
#include "model/MemoryModel.h"

#include <QJsonObject>
#include <QVector>

namespace Controller
{

static const uint8_t NurbsOrder = 3;
static const double SamplingInterval = 0.5;
class RoadEditCommand : public BasicCommand
{
public:
    void execute(PureMVC::Interfaces::INotification const& aNotification) override;
    static std::string GetCommandName();

private:
    void mergeRoad(const uint64_t& aFromRoadId, const uint64_t& aToRoadId);
    void mergeLine(QVector<uint64_t>& aMergedLineIds, const Model::LinePtr& aFromLine, const Model::LinePtr& aToLine);
    std::shared_ptr<Model::NurbsCurve> mergePaintList(const Model::Point3DPtr& aReferencePoint,
                                              const Model::PaintListPtr& aFromPaint, const Model::PaintListPtr& aToPaint);
    Model::PaintListPtr convertWgs84ToRelative(const Model::Point3DPtr& aReferencePoint, const Model::PaintListPtr& aPaintList);
    bool isLaneInRoad(const Model::RoadPtr& aRoad, const uint64_t& aLaneId);
    bool isRoadConnected(const Model::RoadPtr& aFromRoad, const Model::RoadPtr& aToRoad);
    bool canRoadsBeMerged(Model::RoadPtr& aFromRoad, Model::RoadPtr& aToRoad);
    void updateRoadConnection(Model::RoadPtr& aFromRoad, Model::RoadPtr& aToRoad);
    void addLineToRoad(const std::shared_ptr<Model::MemoryModel>& aMemoryModel, Model::RoadPtr& aRoad, const QJsonObject& aData);
    std::shared_ptr<Model::NurbsCurve> getFittedCurve(const Model::Point3DPtr& aReferencePoint,
                                                      const Model::PaintListPtr& aPaintList);
    std::shared_ptr<Model::NurbsCurve> getFittedCurve(const Model::Point3DPtr& aReferencePoint,
                                                      const QJsonArray& aPaintArray);
};

}
