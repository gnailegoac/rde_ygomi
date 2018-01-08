/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    ApplicationFacade.h
 *******************************************************************************
 */

#pragma once

#include <string>
#include <PureMVC/PureMVC.hpp>
#include <map>

class MainProxy;

namespace View
{
class MainWindow;
}

class ApplicationFacade : public PureMVC::Patterns::Facade
{
public:
    static const std::string NAME;
    static const std::string START_UP;
    static const std::string FILE_OPEN;
    static const std::string FOLDER_OPEN;
    static const std::string FILE_OPEN_SUCCESS;
    static const std::string FOLDER_OPEN_SUCCESS;
    static const std::string INIT_SCENE;
    static const std::string REFRESH_SCENE;
    static const std::string REFRESH_WINDOW;
    static const std::string SELECT_NODE;
    static const std::string CHANGE_SELECT_TYPE;
    static const std::string DEHIGHLIGHT_ALL_NODE;

    static ApplicationFacade* GetInstance();

    static void SendNotification(
            const std::string& aNoteName,
            const void* aBody = NULL,
            const std::string& aType = "");

    static void RegisterProxy(PureMVC::Patterns::IProxy* aProxy);
    static PureMVC::Patterns::IProxy& RetriveProxy(const std::string& aProxyName);
    static void RegisterMediator(PureMVC::Patterns::IMediator* aMediator);
    static PureMVC::Patterns::IMediator& RetriveMediator(const std::string& aMediatorName);
    static bool HasMediator(const std::string& aMediatorName);
    static void ReplaceCommand(const std::string& aCommandName,
                               const std::shared_ptr<PureMVC::Patterns::ICommand>& aCommand);
    static std::shared_ptr<PureMVC::Patterns::ICommand> GetCommand(const std::string& aCommandName);
    static MainProxy* GetMainProxy();

    bool StartUp(View::MainWindow* aWindow);
    ~ApplicationFacade();

protected:
    ApplicationFacade();

    void initializeCommands();
    void initializeMessageMap();

private:
    static std::shared_ptr<IFacade> mFacade;
    static std::map<std::string, std::shared_ptr<PureMVC::Patterns::ICommand>> mCommandList;
    static std::map<std::string, std::string> mMessageList;
    static std::map<std::string, PureMVC::Patterns::IMediator*> mMediatorList;
    static std::map<std::string, PureMVC::Patterns::IProxy*> mProxyList;
    static ApplicationFacade* mInstance;

    void registerCommand();
    static void replaceCommand(const std::string& aCommandName,
                               const std::shared_ptr<PureMVC::Patterns::ICommand>& aCommand);
};
