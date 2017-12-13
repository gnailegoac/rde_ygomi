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

#include "view/MainWindow.h"
#include "ui_MainWindow.h"

#include "OsgWidget.h"

View::MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
    QMainWindow(parent, flags),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(new View::OsgWidget(this));
}

View::MainWindow::~MainWindow()
{
    delete ui;
}

void View::MainWindow::resizeEvent(QResizeEvent *event)
{
    resizeDocks({ui->dockWidget}, {width() / 2}, Qt::Horizontal);
}
