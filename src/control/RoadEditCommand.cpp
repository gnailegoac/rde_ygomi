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

void Controller::RoadEditCommand::mergeRoad(const uint64_t& aRoad1Id, const uint64_t& aRoad2Id)
{
    // The pointer should all be valid if come here.
    const std::shared_ptr<Model::MemoryModel>& memoryModel = getMainProxy()->GetMemoryModel();
    const std::shared_ptr<Model::SceneModel>& sceneModel = getMainProxy()->GetSceneModel();
    std::shared_ptr<Model::Road> road1 = memoryModel->GetRoadById(aRoad1Id);
    std::shared_ptr<Model::Road> road2 = memoryModel->GetRoadById(aRoad2Id);
    if (canRoadsBeMerged(road1, road2))
    {
        size_t laneCount = road1->GetLaneListSize();
        const Model::TilePtr& tile = road1->GetTile();
    // TODO: check if the lanes of the two roads are connected.
    // Remove road from scene model
//        sceneModel->RemoveRoadFromScene(aRoad1Id);
//        sceneModel->RemoveRoadFromScene(aRoad2Id);
    // Merge roads, update road1, remove road2
    // Update lanes in road1 predecessor and successor
    // Remove road from memory model
    // Merge road in memory model
    // Update scene model
    // Update tree model
    }
}

bool Controller::RoadEditCommand::isRoadConnected(const Model::RoadPtr& aFromRoad, const Model::RoadPtr& aToRoad)
{
    uint64_t fromLaneId = aFromRoad->GetLane(0)->GetLaneId();
    for (const auto& lane : *(aToRoad->GetLaneList()))
    {
        if (lane->GetPredecessorLaneId() == fromLaneId)
        {
            return true;
        }
    }
    return false;
}

bool Controller::RoadEditCommand::canRoadsBeMerged(Model::RoadPtr& aFromRoad, Model::RoadPtr& aToRoad)
{
    QString message;
    // Check if the roads to merge have the same number of lanes
    if (aFromRoad->GetLaneListSize() != aToRoad->GetLaneListSize())
    {
        message = "Only can merge two roads with the same number of lanes";
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
        qDebug() << "Original from " << aFromRoad->GetRoadId() << " to " << aToRoad->GetRoadId();
        Model::RoadPtr tempRoad = aFromRoad;
        aFromRoad = aToRoad;
        aToRoad = tempRoad;
        qDebug() << "Reordered from " << aFromRoad->GetRoadId() << " to " << aToRoad->GetRoadId();
        return true;
    }
    else
    {
        message = "The two roads are not connected.";
        ApplicationFacade::SendNotification(ApplicationFacade::NOTIFY_RESULT, &message);
        return false;
    }
}
