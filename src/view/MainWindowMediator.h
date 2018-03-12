/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    MainWindowMediator.h
 *******************************************************************************
 */

#pragma once

#include <PureMVC/PureMVC.hpp>

namespace View
{
class MainWindow;
}

namespace Model
{
class Tile;
class Road;
class Lane;
class Line;
class TrafficSign;
}

class MainProxy;

namespace View
{
class MainWindowMediator: public PureMVC::Patterns::Mediator
{
public:
    static const std::string NAME;
    MainWindowMediator(const void* aViewComponent);
    Mediator::NotificationNames listNotificationInterests(void) const override;
    void handleNotification(PureMVC::Patterns::INotification const& aNotification);
    void onRegister();
    void onRemove();
private:
    std::vector<std::string> searchDatabaseFileList(const QString& aFolderpath);
    void selectNodeOnTree(const std::shared_ptr<Model::Tile>& aSegment,
                          const std::shared_ptr<Model::Road> aRoad,
                          const std::shared_ptr<Model::Lane>& aLane = nullptr,
                          const std::shared_ptr<Model::Line>& aLine = nullptr);
    void selectNodeOnTree(const std::shared_ptr<Model::TrafficSign>& aTrafficSign);
    void openRoadRendering();
    void closeRoadRendering();
    bool dbValidation(const std::list<std::string>& aDbPathList);
private:
    View::MainWindow* getMainWindow();
    MainProxy* getMainProxy();
};
}

