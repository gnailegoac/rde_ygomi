/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DbValidationDialog.h
 *******************************************************************************
 */

#pragma once

#include <QDialog>

namespace Ui
{
class DbValidationDialog;
}

namespace View
{

class DbValidationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DbValidationDialog(QWidget* parent = 0);
    ~DbValidationDialog();
    bool UpdateData(const QString& aFilePath);
    void ResetPos();
    bool IsInterrupt();

private slots:
    void onShowDetails();

private:
    void initTableOverView();

private:
    Ui::DbValidationDialog* mUi;
};
}
