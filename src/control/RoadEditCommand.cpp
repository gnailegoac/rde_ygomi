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
#include <PureMVC/PureMVC.hpp>
#include "facade/ApplicationFacade.h"
#include "CommonFunction.h"
#include <QDebug>

#include "model/Lane.h"
#include "model/MemoryModel.h"
#include "model/SceneModel.h"
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

}

std::string Controller::RoadEditCommand::GetCommandName()
{
    return "RoadEditCommand";
}

void Controller::RoadEditCommand::mergeRoad(const uint64_t& aFromRoadId, const uint64_t& aToRoadId)
{
    // The pointer should all be valid if come here.
    const std::shared_ptr<Model::MemoryModel>& memoryModel = getMainProxy()->GetMemoryModel();
    const std::shared_ptr<Model::SceneModel>& sceneModel = getMainProxy()->GetSceneModel();
    std::shared_ptr<Model::Road> fromRoad = memoryModel->GetRoadById(aFromRoadId);
    std::shared_ptr<Model::Road> toRoad = memoryModel->GetRoadById(aToRoadId);
    if (canRoadsBeMerged(fromRoad, toRoad))
    {
        size_t laneCount = fromRoad->GetLaneListSize();
        const Model::TilePtr& tile = fromRoad->GetTile();
        // Merge roads, update road1, remove road2
        // 1. Merge line
        std::vector<uint64_t> lineIdVec;
        for (const auto& lane : *(fromRoad->GetLaneList()))
        {
            uint64_t successorLaneId = lane->GetSuccessorLaneId();
            Model::LanePtr successorLane = toRoad->GetLaneById(successorLaneId);
        }
        // 2. Update lane predecessor/successor connection
        // Update successor of lanes in fromRoad and predecessor of lanes in toRoad's successor
        updateRoadConnection(fromRoad, toRoad);
        // Remove toRoad from memory model

        // Update scene model
        // Remove road from scene model
        sceneModel->RemoveRoadFromScene(aFromRoadId);
        sceneModel->RemoveRoadFromScene(aToRoadId);
        // Add merged fromRoad to scene model again
        sceneModel->AddRoadToScene(fromRoad);
        // Update tree model
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
