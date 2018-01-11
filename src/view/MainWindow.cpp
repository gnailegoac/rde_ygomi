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
#include "model/TreeModel.h"

#include "proxy/MainProxy.h"

View::MainWindow::MainWindow(QWidget *aParent, Qt::WindowFlags flags) : QMainWindow(aParent, flags),
                                                                        ui(new Ui::MainWindow),
                                                                        mRoadInfoView(new QTreeView(this))
{
    ui->setupUi(this);
    this->setCentralWidget(new View::OsgWidget(this));
    restoreSettings();
    setupConnections();
    auto networkSettings = Service::NetworkPreferenceProvider::Instance();
    ui->webRoadEditor->load(QUrl(networkSettings->GetWebServer()));
    mRoadInfoView->setStyleSheet("QTreeView{background-color:rgba(185,185,185,195);}");
    mRoadInfoView->raise();
    mRoadInfoView->setVisible(false);
}

void View::MainWindow::PopupWarningMessage(const QString &aWarning)
{
    QMessageBox::warning(this, windowTitle(), aWarning, "Close");
}

osg::Polytope View::MainWindow::GetPolytope()
{
    View::OsgWidget *viewer = dynamic_cast<View::OsgWidget *>(centralWidget());
    return viewer->GetPolytope();
}

void View::MainWindow::UpdateView()
{
    View::OsgWidget *viewer = dynamic_cast<View::OsgWidget *>(centralWidget());
    viewer->Refresh();
}

void View::MainWindow::ShowRoadInfo()
{
    bool isVisible = mRoadInfoView->isVisible();
    mRoadInfoView->setVisible(!isVisible);
}

void View::MainWindow::SetTreeModel(const std::shared_ptr<Model::TreeModel>& aTreeModel)
{
    mRoadInfoView->setModel(aTreeModel.get());
    mRoadInfoView->setColumnWidth(0, 171);
}

void View::MainWindow::ChangeCameraMatrix(const QJsonArray& aMatrix)
{
    ui->webRoadEditor->ChangeCameraMatrix(aMatrix);
}

void View::MainWindow::setupConnections()
{
    connect(ui->actionOpen, &QAction::triggered, [=]() {
        QString path = QFileDialog::getOpenFileName(this, tr("Open File"), "/", tr("Files(*.db *.xodr *.kml *.xml *.pb)"));
        if (path.length() > 0)
        {
            ApplicationFacade::SendNotification(ApplicationFacade::FILE_OPEN, &path);
        }
    });

    connect(ui->actionOpenFolder, &QAction::triggered, [=]() {
        QString folderPath = QFileDialog::getExistingDirectory(0, ("Select Folder"), "/");
        if (folderPath.length() > 0)
        {
            ApplicationFacade::SendNotification(ApplicationFacade::FOLDER_OPEN, &folderPath);
        }
    });

    connect(ui->actionPreference, &QAction::triggered, [=]() {
        View::NetworkPreferenceDialog networkPreferenceDialog;
        networkPreferenceDialog.exec();
    });

    connect(ui->actionRoadInfo, &QAction::triggered, [=]() {
        ShowRoadInfo();
    });

    connect(ui->actionSelectRoad, &QAction::triggered, [=]() {
        onSelectTypeChange(Model::SelectType::Road, ui->actionSelectRoad->isChecked());
    });

    connect(ui->actionSelectLane, &QAction::triggered, [=]() {
        onSelectTypeChange(Model::SelectType::Lane, ui->actionSelectLane->isChecked());
    });

    connect(ui->actionSelectLine, &QAction::triggered, [=]() {
        onSelectTypeChange(Model::SelectType::Line, ui->actionSelectLine->isChecked());
    });

    connect(ui->webRoadEditor, &WebRoadEditor::cameraMatrixChanged,
            [=](const osg::Matrixd& aMatrix)
    {
        View::OsgWidget* viewer = dynamic_cast<View::OsgWidget*>(centralWidget());
        viewer->CameraMatrixChanged(aMatrix);
    });
}

void View::MainWindow::onSelectTypeChange(const Model::SelectType &aSelectType, bool aIsChecked)
{
    ApplicationFacade::SendNotification(ApplicationFacade::DEHIGHLIGHT_ALL_NODE);
    Model::SelectType selectType = aSelectType;
    if (aSelectType == Model::SelectType::Road)
    {
        if (!aIsChecked)
        {
            ui->actionSelectLine->setChecked(true);
            selectType = Model::SelectType::Line;
        }
        else
        {
            ui->actionSelectLine->setChecked(false);
            ui->actionSelectLane->setChecked(false);
        }
    }
    else if (aSelectType == Model::SelectType::Lane)
    {
        if (!aIsChecked)
        {
            ui->actionSelectLine->setChecked(true);
            selectType = Model::SelectType::Line;
        }
        else
        {
            ui->actionSelectLine->setChecked(false);
            ui->actionSelectRoad->setChecked(false);
        }
    }
    else if (aSelectType == Model::SelectType::Line)
    {
        if (!aIsChecked)
        {
            ui->actionSelectLine->setChecked(true);
            selectType = Model::SelectType::Line;
        }
        else
        {
            ui->actionSelectRoad->setChecked(false);
            ui->actionSelectLane->setChecked(false);
        }
    }
    else
    {
        return;
    }

    ApplicationFacade::SendNotification(ApplicationFacade::CHANGE_SELECT_TYPE, &selectType);
}

View::MainWindow::~MainWindow()
{
    delete ui;
    delete mRoadInfoView;
}

void View::MainWindow::resizeEvent(QResizeEvent* aEvent)
{
    resizeDocks({ui->dockWidget}, {width() / 2}, Qt::Horizontal);
    mRoadInfoView->resize(300, height());
    mRoadInfoView->move(width() - 300, 0);
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

    ui->actionSelectLine->setChecked(true);
}

void View::MainWindow::writeSettings()
{
    QSettings settings;
    settings.beginGroup("ui");
    settings.setValue("mainwindow/geometry", saveGeometry());
    settings.endGroup();
}
