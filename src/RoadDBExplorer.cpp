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

#include "view/MainWindow.h"
#include <QApplication>
#include "facade/ApplicationFacade.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
#ifdef QT_DEBUG
    qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9000");
#endif
    View::MainWindow mainWindow;
    ApplicationFacade::GetInstance()->StartUp(&mainWindow);
    return application.exec();
}
