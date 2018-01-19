/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    ApplicationFacade.cpp
 *******************************************************************************
 */

#include "ApplicationFacade.h"
#include "proxy/MainProxy.h"
#include "view/MainWindow.h"
#include "control/StartUpCommand.h"
#include "control/FileParseCommand.h"
#include "control/SceneManageCommand.h"
#include "control/NodeHighlightCommand.h"

const std::string ApplicationFacade::NAME = "ApplicationFacade";
std::map<std::string, PureMVC::Patterns::IProxy*> ApplicationFacade::mProxyList;
std::map<std::string, PureMVC::Patterns::IMediator*> ApplicationFacade::mMediatorList;
std::map<std::string, std::shared_ptr<PureMVC::Patterns::ICommand>> ApplicationFacade::mCommandList;
std::map<std::string, std::string> ApplicationFacade::mMessageList;
std::shared_ptr<PureMVC::Patterns::IFacade> ApplicationFacade::mFacade(&Facade::getInstance(NAME));

ApplicationFacade* ApplicationFacade::mInstance = NULL;
const std::string ApplicationFacade::START_UP = "StartUp";
const std::string ApplicationFacade::FILE_OPEN = "FileOpen";
const std::string ApplicationFacade::FOLDER_OPEN = "FolderOpen";
const std::string ApplicationFacade::FILE_OPEN_SUCCESS = "FileOpenSuccess";
const std::string ApplicationFacade::FOLDER_OPEN_SUCCESS = "FolderOpenSuccess";
const std::string ApplicationFacade::INIT_SCENE = "InitScene";
const std::string ApplicationFacade::REFRESH_SCENE = "RefreshScene";
const std::string ApplicationFacade::REFRESH_WINDOW = "RefreshWindow";
const std::string ApplicationFacade::SELECT_NODE = "SelectNode";
const std::string ApplicationFacade::CHANGE_SELECT_TYPE = "ChangeSelectType";
const std::string ApplicationFacade::DEHIGHLIGHT_ALL_NODE = "DehighlightAllNode";
const std::string ApplicationFacade::CHANGE_CAMERA = "ChangeCamera";
const std::string ApplicationFacade::SELECT_ROAD_ON_TREE = "SelectRoadOnTree";
const std::string ApplicationFacade::SELECT_LANE_ON_TREE = "SelectLaneOnTree";
const std::string ApplicationFacade::SELECT_LINE_ON_TREE = "SelectLineOnTree";
const std::string ApplicationFacade::UNSELECT_NODE_ON_TREE = "UnselectNodeOnTree";
const std::string ApplicationFacade::SELECT_NODE_IN_3DVIEW = "SelectNodeIn3DView";
const std::string ApplicationFacade::JUMP_TO_CENTER = "JumpToCenter";
const std::string ApplicationFacade::REQUEST_ROADS_IN_TILE = "RequestRoadsInTile";

bool ApplicationFacade::StartUp(View::MainWindow* aWindow)
{
    mFacade->sendNotification(START_UP, aWindow);
    return true;
}

ApplicationFacade::ApplicationFacade()
{
    initializeCommands();
    initializeMessageMap();
    registerCommand();
}

ApplicationFacade* ApplicationFacade::GetInstance()
{
    if (mInstance == NULL)
    {
        mInstance = new ApplicationFacade;
        if (mFacade.get() == NULL)
        {
            mFacade = std::shared_ptr<IFacade>(&Facade::getInstance(NAME));
        }
    }

    return mInstance;
}

void ApplicationFacade::registerCommand()
{
    foreach (auto message, mMessageList)
    {
        mFacade->registerCommand(message.first, mCommandList[message.second].get());
    }
}

ApplicationFacade::~ApplicationFacade()
{
    for (auto meditor : mMediatorList)
    {
        delete meditor.second;
    }

    for (auto proxy : mProxyList)
    {
        delete proxy.second;
    }
}

void ApplicationFacade::initializeCommands()
{
    mCommandList[Controller::StartUpCommand::GetCommandName()] =
        std::shared_ptr<Controller::StartUpCommand>(new Controller::StartUpCommand);
    mCommandList[Controller::FileParseCommand::GetCommandName()] =
        std::shared_ptr<Controller::FileParseCommand>(new Controller::FileParseCommand);
    mCommandList[Controller::SceneManageCommand::GetCommandName()] =
        std::shared_ptr<Controller::SceneManageCommand>(new Controller::SceneManageCommand);
    mCommandList[Controller::NodeHighlightCommand::GetCommandName()] =
        std::shared_ptr<Controller::NodeHighlightCommand>(new Controller::NodeHighlightCommand);
}

void ApplicationFacade::initializeMessageMap()
{
    mMessageList[START_UP] = Controller::StartUpCommand::GetCommandName();
    mMessageList[FOLDER_OPEN_SUCCESS] = Controller::FileParseCommand::GetCommandName();
    mMessageList[FILE_OPEN_SUCCESS] = Controller::FileParseCommand::GetCommandName();
    mMessageList[REFRESH_SCENE] = Controller::SceneManageCommand::GetCommandName();
    mMessageList[SELECT_NODE] = Controller::NodeHighlightCommand::GetCommandName();
    mMessageList[CHANGE_SELECT_TYPE] = Controller::NodeHighlightCommand::GetCommandName();
    mMessageList[DEHIGHLIGHT_ALL_NODE] = Controller::NodeHighlightCommand::GetCommandName();
    mMessageList[SELECT_NODE_IN_3DVIEW] = Controller::NodeHighlightCommand::GetCommandName();
}

void ApplicationFacade::RegisterProxy(PureMVC::Patterns::IProxy* aProxy)
{
    mProxyList[aProxy->getProxyName()] = aProxy;
    mFacade->registerProxy(aProxy);
}

void ApplicationFacade::RegisterMediator(PureMVC::Patterns::IMediator* aMediator)
{
    std::string meditorName = aMediator->getMediatorName();
    mMediatorList[meditorName] = aMediator;
    mFacade->registerMediator(aMediator);
}

void ApplicationFacade::SendNotification(const std::string& aNoteName,
                                         const void* aBody,
                                         const std::string& aType)
{
    mFacade->sendNotification(aNoteName, aBody, aType);
}

PureMVC::Patterns::IProxy& ApplicationFacade::RetriveProxy(const std::string& aProxyName)
{
    return mFacade->retrieveProxy(aProxyName);
}

PureMVC::Patterns::IMediator& ApplicationFacade::RetriveMediator(const std::string& aMediatorName)
{
    return mFacade->retrieveMediator(aMediatorName);
}

bool ApplicationFacade::HasMediator(const std::string& aMediatorName)
{
    return mFacade->hasMediator(aMediatorName);
}

MainProxy* ApplicationFacade::GetMainProxy()
{
    MainProxy& mainProxy = dynamic_cast<MainProxy&>(RetriveProxy(MainProxy::NAME));
    return &mainProxy;
}
