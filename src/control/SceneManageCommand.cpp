/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    SceneManageCommand.cpp
 *******************************************************************************
 */

#include <osg/Geometry>
#include <osg/Geode>

#include "SceneManageCommand.h"
#include "facade/ApplicationFacade.h"
#include "model/MemoryModel.h"
#include "model/Road.h"
#include "model/SceneModel.h"
#include "proxy/MainProxy.h"
#include "CommonFunction.h"

void Controller::SceneManageCommand::execute(PureMVC::Interfaces::INotification const& aNotification)
{
    /*if (aNotification.getName() == ApplicationFacade::INIT_SCENE)
    {
        //use memory model create quadtree
    }
    else */
    if (aNotification.getName() == ApplicationFacade::REFRESH_SCENE)
    {
        // Add roads in view to mainViewer.
        osg::Polytope polytope = *CommonFunction::ConvertToNonConstType<osg::Polytope>(aNotification.getBody());
        constructScene(polytope);
        ApplicationFacade::SendNotification(ApplicationFacade::REFRESH_WINDOW);
    }
}

std::string Controller::SceneManageCommand::GetCommandName()
{
    return "SceneManageCommand";
}

void Controller::SceneManageCommand::constructScene(osg::Polytope& aPolytope)
{
    MainProxy& mainProxy = dynamic_cast<MainProxy&>(ApplicationFacade::RetriveProxy(MainProxy::NAME));
    const std::shared_ptr<Model::SceneModel>& sceneModel = mainProxy.GetSceneModel();
    const std::shared_ptr<Model::MemoryModel>& memoryModel = mainProxy.GetMemoryModel();
    if (!sceneModel || !memoryModel)
    {
        return;
    }
    const Model::TileMapPtr& tileMap = memoryModel->GetTileMap();
    for (const auto& tile : *tileMap)
    {
        const Model::RoadMapPtr& roadMap = tile.second->GetRoadMap();
        for (const auto& road : *roadMap)
        {
            // Get bounding box's minimum and maximum x,y,z
            osg::BoundingBox boundingBox;
            convertBoundingbox(road.second->GetBox(), boundingBox);
            if (aPolytope.contains(boundingBox))
            {
                sceneModel->AddRoadToScene(road.second);
            }
        }
    }
}

void Controller::SceneManageCommand::convertBoundingbox(const Model::BoundingBoxPtr& aBoundingBox, osg::BoundingBox& aResult)
{
    const double heading = aBoundingBox->GetHeading();
    float width = aBoundingBox->GetWidth(); // along the heading direction
    float length = aBoundingBox->GetLength();   // perpendicular with the heading direction
    float height = aBoundingBox->GetHeight();
    const Model::Point3DPtr& center = aBoundingBox->GetPosition();
}
