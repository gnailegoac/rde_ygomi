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
#include "MainWindowMediator.h"
#include "facade/ApplicationFacade.h"
#include "proxy/MainProxy.h"
#include "MainWindow.h"
#include "CommonFunction.h"

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
    QFileInfoList fileList =folder.entryInfoList();

    for(auto& file : fileList)
    {
        QString suffix = file.suffix();
        if(QString::compare(suffix, QString("db"),Qt::CaseInsensitive) == 0)
        {
            QString absoluteFilePath= file.absoluteFilePath();
            databaseFileList.push_back(absoluteFilePath.toStdString());
        }
    }

    return databaseFileList;
}

void View::MainWindowMediator::handleNotification(PureMVC::Patterns::INotification const& aNotification)
{
    std::string noteName = aNotification.getName();
    if (noteName == ApplicationFacade::FILE_OPEN)
    {
        std::string filePath = CommonFunction::ConvertToNonConstType<QString>(aNotification.getBody())->toStdString();
        ApplicationFacade::SendNotification(ApplicationFacade::FILE_OPEN_SUCCESS, &filePath);
    }
    else if(noteName == ApplicationFacade::FOLDER_OPEN)
    {
        QString folderPath = *CommonFunction::ConvertToNonConstType<QString>(aNotification.getBody());
        std::vector<std::string> databaseFileList = searchDatabaseFileList(folderPath);
        if(databaseFileList.size() > 0)
        {
            ApplicationFacade::SendNotification(ApplicationFacade::FOLDER_OPEN_SUCCESS, &databaseFileList);
        }
        else
        {
            View::MainWindow* mainWindow = CommonFunction::ConvertToNonConstType<View::MainWindow>(getViewComponent());
            mainWindow->PopupWarningMessage("No DB file found!");
        }
    }
}

void View::MainWindowMediator::onRemove()
{

}

void View::MainWindowMediator::onRegister()
{

}
