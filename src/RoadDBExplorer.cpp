/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    RoadDBExplorer.cpp
 *******************************************************************************
 */

#include "AppMeta.h"
#include "view/MainWindow.h"
#include <QApplication>
#include "facade/ApplicationFacade.h"
#include "CommonFunction.h"
#include <osgDB/Registry>

void setupMetaData(QApplication& aApp)
{
    QString version = QString::fromStdString(AppMeta::scVersion);
    QString buildTime = QString("%1 %2").arg(__DATE__).arg(__TIME__);

    aApp.setOrganizationName("Ygomi");
    aApp.setApplicationName("RoadDatabaseExplorer");
    aApp.setApplicationVersion(version);
    aApp.setProperty("buildTime", buildTime);
    aApp.setProperty("commitSha", QString::fromStdString(AppMeta::scGitCommitSha));
    aApp.setProperty("commitCount", AppMeta::scGitCommmitCount);
    aApp.setProperty("applicationDisplayName", "Road Database Explorer");
}

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    setupMetaData(application);
#ifdef QT_DEBUG
    qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9000");
#endif
    osgDB::Registry::instance()->setReadFileCallback(new ReadImageFromResourceCallback);
    View::MainWindow mainWindow;
    ApplicationFacade::GetInstance()->StartUp(&mainWindow);
    return application.exec();
}
