/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    AboutDialog.cpp
 * @brief
 *******************************************************************************
 */

#include "AboutDialog.h"
#include "ui_AboutDialog.h"

View::AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    QString version = tr("%1 (%2)")
            .arg(qApp->applicationVersion(),
                 qApp->property("commitCount").toString());

    const QString description
            = tr("<h3>%1</h3>"
                 "Version: %2<br/><br/>"
                 "%3<br/><br/>"
                 "Build on %4<br/><br/>"
                 "Copyright (C) 2018 Ygomi. All rights reserved.<br/>")
            .arg(qApp->property("applicationDisplayName").toString(),
                 version,
                 qApp->property("commitSha").toString(),
                 qApp->property("buildTime").toString());

    ui->description->setText(description);
}

View::AboutDialog::~AboutDialog()
{
    delete ui;
}
