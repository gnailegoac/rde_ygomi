/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    MainWindow.h
 *******************************************************************************
 */

#pragma once

#include <QMainWindow>
#include <QString>

namespace Ui {
class MainWindow;
}

namespace View
{
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* aParent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow();
    void PopupWarningMessage(const QString& aWarning);

protected:
    void resizeEvent(QResizeEvent* aEvent) override;
    void closeEvent(QCloseEvent* aEvent) override;

private:
    void restoreSettings();
    void writeSettings();
    void setupConnections();
    Ui::MainWindow *ui;
};
}

