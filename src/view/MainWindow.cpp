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
#include <QTreeWidget>

#include "service/NetworkPreferenceProvider.h"
#include "view/MainWindow.h"
#include "view/NetworkPreferenceDialog.h"
#include "ui_MainWindow.h"
#include "view/AboutDialog.h"
#include "view/DbValidationDialog.h"
#include "view/LogicCheckDialog.h"

#include "OsgWidget.h"
#include "MainWindowMediator.h"
#include "facade/ApplicationFacade.h"
#include "model/TreeModel.h"

#include "proxy/MainProxy.h"
#include "service/RoadEditParameters.h"
#include "model/QIModel.h"

View::MainWindow::MainWindow(QWidget* aParent, Qt::WindowFlags flags) : QMainWindow(aParent, flags),
    ui(new Ui::MainWindow),
    mRoadInfoView(new QTreeView(this)),
    mDbValidationDialog(new DbValidationDialog(this))
{
    ui->setupUi(this);
    this->setCentralWidget(new View::OsgWidget(this));
    restoreSettings();
    setupConnections();
    EnableSaveAction(false);
    auto networkSettings = Service::NetworkPreferenceProvider::Instance();
    ui->webRoadEditor->load(QUrl(networkSettings->GetWebServer()));
    mRoadInfoView->setStyleSheet("QTreeView{background-color:rgba(185,185,185,195);}");
    mRoadInfoView->raise();
    mRoadInfoView->setVisible(false);
    mDbValidationDialog->hide();
    ui->actionWarning->setIcon(QIcon(QPixmap(":/resource/image/warning.png")));
}

void View::MainWindow::PopupWarningMessage(const QString& aWarning)
{
    QMessageBox::warning(this, windowTitle(), aWarning, "Close");
}

void View::MainWindow::PopupInfoMessage(const QString& aMessage)
{
    QMessageBox::information(this, windowTitle(), aMessage, "Close");
}

osg::Polytope View::MainWindow::GetPolytope()
{
    View::OsgWidget* viewer = dynamic_cast<View::OsgWidget*>(centralWidget());
    return viewer->GetPolytope();
}

void View::MainWindow::UpdateView()
{
    View::OsgWidget* viewer = dynamic_cast<View::OsgWidget*>(centralWidget());
    viewer->Refresh();
}

void View::MainWindow::JumpToCenter(const osg::Vec3d& aCenter)
{
    View::OsgWidget* viewer = dynamic_cast<View::OsgWidget*>(centralWidget());
    viewer->JumpToCenter(aCenter);
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

void View::MainWindow::SendRoadsInTile(int aLevel, const QJsonArray& aRoadArray)
{
    ui->webRoadEditor->SendRoadsInTile(aLevel, aRoadArray);
}

void View::MainWindow::PushEntireRoadTilesExtent(const QJsonArray& aTileArray)
{
    ui->webRoadEditor->PushEntireRoadTilesExtent(aTileArray);
}

void View::MainWindow::SendRoadToEdit(const QJsonObject& aRoad)
{
    ui->webRoadEditor->SendRoadToEdit(aRoad);
}

QTreeView* View::MainWindow::GetTreeView() const
{
    return mRoadInfoView;
}

void View::MainWindow::setupConnections()
{
    connect(ui->actionOpen, &QAction::triggered, [ = ]()
    {
        QString path = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"), "/",
                                                    tr("Files(*.db *.xodr *.kml *.xml *.pb)"));
        if (path.length() > 0)
        {
            ApplicationFacade::SendNotification(ApplicationFacade::FILE_OPEN, &path);
        }
    });

    connect(ui->actionOpenFolder, &QAction::triggered, [ = ]()
    {
        QString folderPath = QFileDialog::getExistingDirectory(0, ("Select Folder"), "/");
        if (folderPath.length() > 0)
        {
            ApplicationFacade::SendNotification(ApplicationFacade::FOLDER_OPEN, &folderPath);
        }
    });

    connect(ui->actionSave, &QAction::triggered, [ = ]()
    {
        QString folderPath = QFileDialog::getExistingDirectory(0, ("Select Folder"), "/");
        if (folderPath.length() > 0)
        {
            ApplicationFacade::SendNotification(ApplicationFacade::SAVE_LOGICDB, &folderPath);
        }
    });

    connect(ui->actionKML, &QAction::triggered, [ = ]()
    {
        QString folderPath = QFileDialog::getExistingDirectory(0, ("Select Folder"), "/");
        if (folderPath.length() > 0)
        {
            ApplicationFacade::SendNotification(ApplicationFacade::EXPORT_TO_KML, &folderPath);
        }
    });

    connect(ui->actionProtoBuffer, &QAction::triggered, [ = ]()
    {
        QString path = QFileDialog::getSaveFileName(this,
                                                    tr("Export to ProtoBuffer"), "/");
        if (path.length() > 0)
        {
            ApplicationFacade::SendNotification(ApplicationFacade::EXPORT_TO_PROTOBUF, &path);
        }
    });

    connect(ui->actionDLM, &QAction::triggered, [ = ]()
    {
        QString path = QFileDialog::getSaveFileName(this,
                                                    tr("Export to DLM"), "/");
        if (path.length() > 0)
        {
            ApplicationFacade::SendNotification(ApplicationFacade::EXPORT_TO_DLM, &path);
        }
    });

    connect(ui->actionPreference, &QAction::triggered, [ = ]()
    {
        View::NetworkPreferenceDialog networkPreferenceDialog;
        networkPreferenceDialog.exec();
    });

    connect(ui->actionRoadInfo, &QAction::triggered, [ = ]()
    {
        ShowRoadInfo();
    });

    connect(ui->actionSelectRoad, &QAction::triggered, [ = ]()
    {
        onSelectTypeChange(Model::SelectType::Road, ui->actionSelectRoad->isChecked());
    });

    connect(ui->actionSelectLane, &QAction::triggered, [ = ]()
    {
        onSelectTypeChange(Model::SelectType::Lane, ui->actionSelectLane->isChecked());
    });

    connect(ui->actionSelectLine, &QAction::triggered, [ = ]()
    {
        onSelectTypeChange(Model::SelectType::Line, ui->actionSelectLine->isChecked());
    });

    connect(ui->actionSelectSign, &QAction::triggered, [ = ]()
    {
        onSelectTypeChange(Model::SelectType::TrafficSign, ui->actionSelectSign->isChecked());
    });

    connect(ui->webRoadEditor, &WebRoadEditor::cameraMatrixChanged,
            [ = ](const osg::Matrixd & aMatrix)
    {
        View::OsgWidget* viewer = dynamic_cast<View::OsgWidget*>(centralWidget());
        viewer->CameraMatrixChanged(aMatrix);
    });

    connect(mRoadInfoView, &QTreeView::pressed, [ = ](const QModelIndex & aModelIndex)
    {
        std::pair<QString, QString> selectItemPair = std::make_pair(aModelIndex.data().toString(),
                                                                    aModelIndex.sibling(aModelIndex.row(), 1).data().toString());
        if(selectItemPair.first == "LeftLine" || selectItemPair.first == "RightLine")
        {
            selectItemPair.second = aModelIndex.child(0, 1).data().toString();
        }
        ApplicationFacade::SendNotification(ApplicationFacade::SELECT_NODE_IN_3DVIEW, &selectItemPair);
    });

    connect(ui->actionRenderRoad, &QAction::triggered, [ = ]()
    {
        onRenderRoad(ui->actionRenderRoad->isChecked());
    });

    connect(ui->actionWarning, &QAction::triggered, [ = ]()
    {
        mDbValidationDialog->ResetPos();
        mDbValidationDialog->setBtnContinueEnabled(false);
        mDbValidationDialog->setLabelWarningVisible(false);
        if(mDbValidationDialog->isHidden())
        {
            mDbValidationDialog->show();
            mDbValidationDialog->setBtnContinueEnabled(false);
            mDbValidationDialog->setLabelWarningVisible(false);
        }
        else
        {
            mDbValidationDialog->hide();
            mDbValidationDialog->setBtnContinueEnabled(false);
            mDbValidationDialog->setLabelWarningVisible(false);
        }
    });

    connect(ui->actionAbout, &QAction::triggered, [ = ]()
    {
        View::AboutDialog aboutDialog;
        aboutDialog.exec();
    });

    connect(ui->actionCheck_Logic_Consistency, &QAction::triggered, [ = ]()
    {
        ApplicationFacade::SendNotification(ApplicationFacade::CHECK_LOGIC_CONSISTENCY);
    });
}

void View::MainWindow::onSelectTypeChange(const Model::SelectType& aSelectType, bool aIsChecked)
{
    ApplicationFacade::SendNotification(ApplicationFacade::DEHIGHLIGHT_ALL_NODE);
    Model::SelectType selectType = aSelectType;
    Service::RoadEditParameters::Instance()->ClearSelectedElement();
    if (aSelectType == Model::SelectType::Road)
    {
        if (!aIsChecked)
        {
            ui->actionSelectLine->setChecked(true);
            selectType = Model::SelectType::Line;
        }
        else
        {
            Service::RoadEditParameters::Instance()->SetEditType(Service::EditType::Road);
            ui->actionSelectLine->setChecked(false);
            ui->actionSelectLane->setChecked(false);
            ui->actionSelectSign->setChecked(false);
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
            Service::RoadEditParameters::Instance()->SetEditType(Service::EditType::Lane);
            ui->actionSelectLine->setChecked(false);
            ui->actionSelectRoad->setChecked(false);
            ui->actionSelectSign->setChecked(false);
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
            Service::RoadEditParameters::Instance()->SetEditType(Service::EditType::Line);
            ui->actionSelectRoad->setChecked(false);
            ui->actionSelectLane->setChecked(false);
            ui->actionSelectSign->setChecked(false);
        }
    }
    else if (aSelectType == Model::SelectType::TrafficSign)
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
            ui->actionSelectLine->setChecked(false);
        }
    }
    else
    {
        return;
    }

    View::OsgWidget* viewer = dynamic_cast<View::OsgWidget*>(centralWidget());
    viewer->SetSelectType(aSelectType);
    ApplicationFacade::SendNotification(ApplicationFacade::CHANGE_SELECT_TYPE, &selectType);
}

void View::MainWindow::onRenderRoad(bool aIsChecked)
{
    if (aIsChecked)
    {
        ApplicationFacade::SendNotification(ApplicationFacade::OPEN_ROAD_RENDERING);
    }
    else
    {
        ApplicationFacade::SendNotification(ApplicationFacade::CLOSE_ROAD_RENDERING);
    }
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

void View::MainWindow::EnableSaveAction(bool aEnable)
{
    ui->actionSave->setEnabled(aEnable);
    ui->menuExport->setEnabled(aEnable);
}

uint8_t View::MainWindow::GetLevel()
{
    View::OsgWidget* viewer = dynamic_cast<View::OsgWidget*>(centralWidget());
    return viewer->GetLevel();
}

QJsonArray View::MainWindow::GetCameraMatrix()
{
    View::OsgWidget* viewer = dynamic_cast<View::OsgWidget*>(centralWidget());
    return viewer->GetCameraMatrix();
}

View::DbValidationDialog* View::MainWindow::GetDbValidationDialog() const
{
    return mDbValidationDialog;
}

void View::MainWindow::setActionWarningIcon(unsigned int aStatus)
{
    if(aStatus == 0)
    {
        ui->actionWarning->setEnabled(true);
        mDbValidationDialog->setBtnShowDetailsEnabled(false);
        ui->actionWarning->setIcon(QIcon(QPixmap(":/resource/image/pass.png")));
    }
    if(aStatus == 1)
    {
        ui->actionWarning->setEnabled(true);
        mDbValidationDialog->setBtnShowDetailsEnabled(true);
        ui->actionWarning->setIcon(QIcon(QPixmap(":/resource/image/warning.png")));
    }
    if(aStatus == 2)
    {
        ui->actionWarning->setEnabled(true);
        mDbValidationDialog->setBtnShowDetailsEnabled(true);
        ui->actionWarning->setIcon(QIcon(QPixmap(":/resource/image/error.png")));
    }
}

void View::MainWindow::ShowCheckLogicConsistencyStart()
{
    ui->actionCheckLogic->setText("Checking...");
}

void View::MainWindow::ShowCheckLogicConsistencyResult()
{
    ui->actionCheckLogic->setText("Done!");
    View::LogicCheckDialog* dialog = new View::LogicCheckDialog();
    dialog->show();
}
