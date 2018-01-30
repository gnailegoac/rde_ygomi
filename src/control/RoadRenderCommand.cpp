#include <osg/Geometry>
#include <osg/Geode>
#include <osgDB/WriteFile>
#include <QDebug>

#include "RoadRenderCommand.h"
#include "facade/ApplicationFacade.h"
#include "model/MemoryModel.h"
#include "model/Road.h"
#include "model/SceneModel.h"
#include "proxy/MainProxy.h"
#include "CommonFunction.h"

Controller::RoadRenderCommand::RoadRenderCommand()
{
}

Controller::RoadRenderCommand::~RoadRenderCommand()
{
}

void Controller::RoadRenderCommand::OpenRoadRendering()
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
            sceneModel->AddRoadModelToScene(road.second);
        }
    }
}

void Controller::RoadRenderCommand::CloseRoadRendering()
{
    MainProxy& mainProxy = dynamic_cast<MainProxy&>(ApplicationFacade::RetriveProxy(MainProxy::NAME));
    const std::shared_ptr<Model::SceneModel>& sceneModel = mainProxy.GetSceneModel();
    sceneModel->RemoveRoadModelFromScene();
}

