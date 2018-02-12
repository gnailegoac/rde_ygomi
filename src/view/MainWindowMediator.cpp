/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    MainWindowMediator.cpp
 *******************************************************************************
 */

#include <QDir>
#include <QDebug>
#include <QJsonArray>
#include "MainWindowMediator.h"
#include "facade/ApplicationFacade.h"
#include "proxy/MainProxy.h"
#include "MainWindow.h"
#include "CommonFunction.h"
#include "model/Road.h"
#include "model/TreeModel.h"
#include "model/Tile.h"
#include "model/Lane.h"
#include "model/Line.h"
#include "model/MemoryModel.h"
#include "model/GeoJsonConverter.h"
#include "model/SceneModel.h"

const std::string View::MainWindowMediator::NAME = "MainWindowMediator";

View::MainWindowMediator::MainWindowMediator(const void* aViewComponent)
    : Mediator(NAME, aViewComponent)
{

}

PureMVC::Patterns::Mediator::NotificationNames View::MainWindowMediator::listNotificationInterests() const
{
    PureMVC::Patterns::StdContainerAggregate<std::list<NotificationNames::element_type::type>>* result =
                            new PureMVC::Patterns::StdContainerAggregate<std::list<NotificationNames::element_type::type>>;
    result->get().push_back(ApplicationFacade::FILE_OPEN);
    result->get().push_back(ApplicationFacade::FOLDER_OPEN);
    result->get().push_back(ApplicationFacade::INIT_SCENE);
    result->get().push_back(ApplicationFacade::REFRESH_WINDOW);
    result->get().push_back(ApplicationFacade::CHANGE_CAMERA);
    result->get().push_back(ApplicationFacade::SELECT_ROAD_ON_TREE);
    result->get().push_back(ApplicationFacade::SELECT_LANE_ON_TREE);
    result->get().push_back(ApplicationFacade::SELECT_LINE_ON_TREE);
    result->get().push_back(ApplicationFacade::SELECT_SIGN_ON_TREE);
    result->get().push_back(ApplicationFacade::UNSELECT_NODE_ON_TREE);
    result->get().push_back(ApplicationFacade::JUMP_TO_CENTER);
    result->get().push_back(ApplicationFacade::NOTIFY_RESULT);
    result->get().push_back(ApplicationFacade::REQUEST_ROADS_IN_TILE);
    result->get().push_back(ApplicationFacade::OPEN_ROAD_RENDERING);
    result->get().push_back(ApplicationFacade::CLOSE_ROAD_RENDERING);
    return NotificationNames(result);
}

MainProxy* View::MainWindowMediator::getMainProxy()
{
    MainProxy& mainProxy = dynamic_cast<MainProxy&>(ApplicationFacade::RetriveProxy(MainProxy::NAME));
    return &mainProxy;
}

std::vector<std::string> View::MainWindowMediator::searchDatabaseFileList(const QString& aFolderpath)
{
    std::vector<std::string> databaseFileList;
    QDir folder(aFolderpath);
    if(!folder.exists())
    {
        return databaseFileList;
    }

    folder.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList fileList = folder.entryInfoList();

    for(auto& file : fileList)
    {
        QString suffix = file.suffix();
        if(QString::compare(suffix, QString("db"), Qt::CaseInsensitive) == 0)
        {
            QString absoluteFilePath = file.absoluteFilePath();
            databaseFileList.push_back(absoluteFilePath.toStdString());
        }
    }

    QFileInfoList folderList = folder.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for(const auto& subFolder : folderList)
    {
        std::vector<std::string> childFileList = searchDatabaseFileList(subFolder.absoluteFilePath());
        databaseFileList.insert(databaseFileList.end(), childFileList.begin(), childFileList.end());
    }

    return databaseFileList;
}

View::MainWindow* View::MainWindowMediator::getMainWindow()
{
    View::MainWindow* mainWindow = CommonFunction::ConvertToNonConstType<View::MainWindow>(getViewComponent());
    return mainWindow;
}

void View::MainWindowMediator::selectNodeOnTree(const std::shared_ptr<Model::TrafficSign>& aTrafficSign)
{
    if(!getMainWindow()->GetTreeView()->isVisible() || aTrafficSign == nullptr)
    {
        return;
    }

    QModelIndex currentIndex;
    QTreeView* roadInfoView = getMainWindow()->GetTreeView();
    QAbstractItemModel* model = roadInfoView->model();
    Model::TreeModel* treeModel = dynamic_cast<Model::TreeModel*>(model);
    QModelIndex segmentIndex = treeModel->GetItemIndex("Segment",
                                                       aTrafficSign->GetTile()->GetTileId(),
                                                       roadInfoView->rootIndex());
    roadInfoView->expand(segmentIndex);

    QModelIndex signModelIndex = treeModel->GetItemIndex("TrafficSign",
                                                         aTrafficSign->GetTrafficSignId(),
                                                         segmentIndex);
    roadInfoView->setCurrentIndex(signModelIndex);
}

void View::MainWindowMediator::selectNodeOnTree(const std::shared_ptr<Model::Tile>& aSegment,
                                                const std::shared_ptr<Model::Road> aRoad,
                                                const std::shared_ptr<Model::Lane>& aLane,
                                                const std::shared_ptr<Model::Line>& aLine)
{
    if(!getMainWindow()->GetTreeView()->isVisible()
            || aSegment == nullptr
            || aRoad == nullptr)
    {
        return;
    }

    QModelIndex currentIndex;
    QTreeView* roadInfoView = getMainWindow()->GetTreeView();
    QAbstractItemModel* model = roadInfoView->model();
    Model::TreeModel* treeModel = dynamic_cast<Model::TreeModel*>(model);
    QModelIndex segmentIndex = treeModel->GetItemIndex("Segment",
                                                       aSegment->GetTileId(),
                                                       roadInfoView->rootIndex());
    roadInfoView->expand(segmentIndex);

    QModelIndex roadModelIndex = treeModel->GetItemIndex("Road",
                                                         aRoad->GetRoadId(),
                                                         segmentIndex);
    roadInfoView->expand(roadModelIndex);
    currentIndex = roadModelIndex;

    QModelIndex laneModelIndex;
    if(aLane != nullptr)
    {
        laneModelIndex = treeModel->GetItemIndex("Lane",
                                                 aLane->GetLaneId(),
                                                 roadModelIndex);
        roadInfoView->expand(laneModelIndex);
        currentIndex = laneModelIndex;
    }

    if(aLine != nullptr)
    {
        QModelIndex lineModelIndex = treeModel->GetItemIndex("Line",
                                                             aLine->GetLineId(),
                                                             laneModelIndex);
        roadInfoView->expand(lineModelIndex);
        currentIndex = lineModelIndex;
    }

    roadInfoView->setCurrentIndex(currentIndex);
}

void View::MainWindowMediator::handleNotification(PureMVC::Patterns::INotification const& aNotification)
{
    std::string noteName = aNotification.getName();
    if (noteName == ApplicationFacade::FILE_OPEN)
    {
        getMainWindow()->EnableSaveAction(true);
        std::string filePath = CommonFunction::ConvertToNonConstType<QString>(aNotification.getBody())->toStdString();
        ApplicationFacade::SendNotification(ApplicationFacade::FILE_OPEN_SUCCESS, &filePath);
    }
    else if (noteName == ApplicationFacade::FOLDER_OPEN)
    {
        QString folderPath = *CommonFunction::ConvertToNonConstType<QString>(aNotification.getBody());
        std::vector<std::string> databaseFileList = searchDatabaseFileList(folderPath);
        if (databaseFileList.size() > 0)
        {
            getMainWindow()->EnableSaveAction(true);
            ApplicationFacade::SendNotification(ApplicationFacade::FOLDER_OPEN_SUCCESS, &databaseFileList);
        }
        else
        {
            View::MainWindow* mainWindow = getMainWindow();
            mainWindow->PopupWarningMessage("No DB file found!");
        }
    }
    else if (noteName == ApplicationFacade::INIT_SCENE)
    {
        View::MainWindow* mainWindow = getMainWindow();
        // send tiles to web viewer.
        QJsonArray tileArray;
        const std::shared_ptr<Model::MemoryModel>& memoryModel = getMainProxy()->GetMemoryModel();
        if (memoryModel != nullptr)
        {
            tileArray = Model::GeoJsonConverter().GetTileExtent(memoryModel);
            getMainWindow()->PushEntireRoadTilesExtent(tileArray);
        }
        mainWindow->SetTreeModel(getMainProxy()->GetTreeModel());
        osg::Polytope polytope = mainWindow->GetPolytope();
        ApplicationFacade::SendNotification(ApplicationFacade::REFRESH_SCENE, &polytope);
    }
    else if (noteName == ApplicationFacade::REFRESH_WINDOW)
    {
        View::MainWindow* mainWindow = getMainWindow();
        mainWindow->UpdateView();
    }
    else if (noteName == ApplicationFacade::CHANGE_CAMERA)
    {
        View::MainWindow* mainWindow = getMainWindow();
        QJsonArray cameraMatrix = *CommonFunction::ConvertToNonConstType<QJsonArray>(aNotification.getBody());
        mainWindow->ChangeCameraMatrix(cameraMatrix);

        MainProxy* mainProxy = getMainProxy();
        const std::shared_ptr<Model::SceneModel>& sceneModel = mainProxy->GetSceneModel();
        const std::shared_ptr<Model::MemoryModel>& memoryModel = mainProxy->GetMemoryModel();
        //sceneModel->RedrawRoadMarks(mainWindow->GetDistance());
        if(sceneModel != nullptr && memoryModel != nullptr)
        {
            sceneModel->RedrawSceneByLOD(memoryModel, mainWindow->GetDistance());
        }
    }
    else if (noteName == ApplicationFacade::SELECT_ROAD_ON_TREE)
    {
        uint64_t id = *CommonFunction::ConvertToNonConstType<uint64_t>(aNotification.getBody());
        Model::RoadPtr road = getMainProxy()->GetMemoryModel()->GetRoadById(id);
        if(road != nullptr)
        {
            selectNodeOnTree(road->GetTile(), road);
        }
    }
    else if (noteName == ApplicationFacade::SELECT_LANE_ON_TREE)
    {
        uint64_t id = *CommonFunction::ConvertToNonConstType<uint64_t>(aNotification.getBody());
        Model::LanePtr lane = getMainProxy()->GetMemoryModel()->GetLaneById(id);
        if(lane != nullptr)
        {
            Model::RoadPtr road = lane->GetRoad();
            selectNodeOnTree(road->GetTile(), road, lane);
        }
    }
    else if (noteName == ApplicationFacade::SELECT_LINE_ON_TREE)
    {
        uint64_t id = *CommonFunction::ConvertToNonConstType<uint64_t>(aNotification.getBody());
        Model::LinePtr line = getMainProxy()->GetMemoryModel()->GetLineById(id);
        if(line != nullptr)
        {
            Model::LanePtr lane = line->GetLane();
            Model::RoadPtr road = lane->GetRoad();
            selectNodeOnTree(road->GetTile(), road, lane, line);
        }
    }
    else if (noteName == ApplicationFacade::SELECT_SIGN_ON_TREE)
    {
        uint64_t id = *CommonFunction::ConvertToNonConstType<uint64_t>(aNotification.getBody());
        selectNodeOnTree(getMainProxy()->GetMemoryModel()->GetTrafficSignById(id));
    }
    else if (noteName == ApplicationFacade::UNSELECT_NODE_ON_TREE)
    {
        getMainWindow()->GetTreeView()->clearSelection();
    }
    else if (noteName == ApplicationFacade::JUMP_TO_CENTER)
    {
        osg::Vec3d center = *CommonFunction::ConvertToNonConstType<osg::Vec3d>(aNotification.getBody());
        getMainWindow()->JumpToCenter(center);
    }
    else if (noteName == ApplicationFacade::NOTIFY_RESULT)
    {
        QString message = *CommonFunction::ConvertToNonConstType<QString>(aNotification.getBody());
        getMainWindow()->PopupInfoMessage(message);
    }
    else if (noteName == ApplicationFacade::REQUEST_ROADS_IN_TILE)
    {
        std::pair<std::uint64_t, std::uint64_t> tileInfo =
                        *CommonFunction::ConvertToNonConstType<std::pair<std::uint64_t, std::uint64_t>>(aNotification.getBody());
        QJsonArray roadsArray;
        const std::shared_ptr<Model::MemoryModel>& memoryModel = getMainProxy()->GetMemoryModel();
        if (memoryModel != nullptr)
        {
            Model::TileConstPtr tilePtr = memoryModel->GetTile(tileInfo.second);
            if (tilePtr != nullptr)
            {
                roadsArray = Model::GeoJsonConverter().Convert(tileInfo.first, tilePtr);
            }
            getMainWindow()->SendRoadsInTile(tileInfo.first, roadsArray);
        }
    }
    else if(noteName == ApplicationFacade::OPEN_ROAD_RENDERING)
    {
        openRoadRendering();
    }
    else if(noteName == ApplicationFacade::CLOSE_ROAD_RENDERING)
    {
        closeRoadRendering();
    }
}

void View::MainWindowMediator::openRoadRendering()
{
    MainProxy* mainProxy = getMainProxy();
    const std::shared_ptr<Model::SceneModel>& sceneModel = mainProxy->GetSceneModel();
    const std::shared_ptr<Model::MemoryModel>& memoryModel = mainProxy->GetMemoryModel();
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
    getMainWindow()->centralWidget()->repaint();
}

void View::MainWindowMediator::closeRoadRendering()
{
    MainProxy* mainProxy = getMainProxy();
    const std::shared_ptr<Model::SceneModel>& sceneModel = mainProxy->GetSceneModel();
    if (!sceneModel)
    {
        return;
    }
    sceneModel->RemoveRoadModelFromScene();
    getMainWindow()->centralWidget()->repaint();
}

void View::MainWindowMediator::onRemove()
{

}

void View::MainWindowMediator::onRegister()
{

}
