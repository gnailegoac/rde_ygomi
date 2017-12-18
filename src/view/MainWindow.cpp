/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    MainWindow.cpp
 *******************************************************************************
 */
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

#include "service/NetworkPreferenceProvider.h"
#include "view/MainWindow.h"
#include "view/NetworkPreferenceDialog.h"
#include "ui_MainWindow.h"

#include "OsgWidget.h"
#include "MainWindowMediator.h"
#include "facade/ApplicationFacade.h"

View::MainWindow::MainWindow(QWidget* aParent, Qt::WindowFlags flags) :
    QMainWindow(aParent, flags),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(new View::OsgWidget(this));
    restoreSettings();
    setupConnections();
    auto networkSettings = Service::NetworkPreferenceProvider::Instance();
    ui->webRoadEditor->load(QUrl(networkSettings->GetWebServer()));
}

void View::MainWindow::PopupWarningMessage(const QString& aWarning)
{
    QMessageBox::warning(this, windowTitle(), aWarning, "Close");
}

void View::MainWindow::setupConnections()
{
    connect(ui->actionOpen, &QAction::triggered, [=]()
    {
        QString path = QFileDialog::getOpenFileName(this, tr("Open File"), "/", tr("Files(*.db *.xodr *.kml *.xml *.pb)"));
        if(path.length() > 0)
        {
            ApplicationFacade::SendNotification(ApplicationFacade::FILE_OPEN, &path);
        }
    });

    connect(ui->actionOpenFolder, &QAction::triggered, [=]()
    {
        QString folderPath = QFileDialog::getExistingDirectory(0, ("Select Folder"), "/");
        if(folderPath.length() > 0)
        {
            ApplicationFacade::SendNotification(ApplicationFacade::FOLDER_OPEN, &folderPath);
        }
    });

    connect(ui->actionPreference, &QAction::triggered, [=]()
    {
        View::NetworkPreferenceDialog networkPreferenceDialog;
        networkPreferenceDialog.exec();
    });
}

View::MainWindow::~MainWindow()
{
    delete ui;
}

void View::MainWindow::resizeEvent(QResizeEvent* aEvent)
{
    resizeDocks({ui->dockWidget}, {width() / 2}, Qt::Horizontal);
}

void View::MainWindow::closeEvent(QCloseEvent* aEvent)
{
    writeSettings();
    QMainWindow::closeEvent(aEvent);
}

void View::MainWindow::restoreSettings()
{
    QSettings settings;
    settings.beginGroup("ui");
    const QByteArray mainWindowGeometry = settings.value("mainwindow/geometry").toByteArray();
    settings.endGroup();
    if (!mainWindowGeometry.isEmpty())
    {
        restoreGeometry(mainWindowGeometry);
    }
}

void View::MainWindow::writeSettings()
{
    QSettings settings;
    settings.beginGroup("ui");
    settings.setValue("mainwindow/geometry", saveGeometry());
    settings.endGroup();
}
