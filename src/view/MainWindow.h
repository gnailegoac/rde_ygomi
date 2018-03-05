/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    MainWindow.h
 *******************************************************************************
 */

#pragma once

#include <memory>

#include <QJsonArray>
#include <QMainWindow>
#include <QString>
#include <QTreeView>

#include <osg/Polytope>
#include "model/Common.h"

namespace Ui
{
class MainWindow;
}

namespace Model
{
class TreeModel;
}

namespace View
{
class DbValidationDialog;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* aParent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();
    void PopupWarningMessage(const QString& aWarning);
    void PopupInfoMessage(const QString& aMessage);
    osg::Polytope GetPolytope();
    void UpdateView();
    void ShowRoadInfo();
    void SetTreeModel(const std::shared_ptr<Model::TreeModel>& aTreeModel);
    void ChangeCameraMatrix(const QJsonArray& aMatrix);
    void SendRoadsInTile(int aLevel, const QJsonArray& aRoadArray);
    void PushEntireRoadTilesExtent(const QJsonArray& aTileArray);

    QTreeView* GetTreeView() const;
    void JumpToCenter(const osg::Vec3d& aCenter);
    void EnableSaveAction(bool aEnable);
    std::uint8_t GetLevel();
    DbValidationDialog* GetDbValidationDialog() const;

protected:
    void resizeEvent(QResizeEvent* aEvent) override;
    void closeEvent(QCloseEvent* aEvent) override;

private:
    void onSelectTypeChange(const Model::SelectType& aSelectType, bool aIsChecked);
    void onRenderRoad(bool aIsChecked);
    void restoreSettings();
    void writeSettings();
    void setupConnections();
    Ui::MainWindow* ui;
    QTreeView* mRoadInfoView;
    DbValidationDialog* mDbValidationDialog;

};
}
