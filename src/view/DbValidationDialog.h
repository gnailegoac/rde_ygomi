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

static const QString scLevelError = "Serious Error";
static const QString scLevelWarning = "Warning";

class DbValidationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DbValidationDialog(QWidget* parent = 0);
    ~DbValidationDialog();
    bool UpdateData(const QString& aFilePath);
    void ResetPos();
    void setBtnShowDetailsEnabled(bool aIsEnabled);
    void setBtnContinueEnabled(bool aIsEnabled);
    void setBtnCancelEnabled(bool aIsEnabled);
    void setLabelWarningVisible(bool aIsVisible);
    void getErrorNumberOfLevel(std::map<QString, std::uint32_t>& aErrorNumberOfLevelMap);

private slots:
    void onShowDetails();
    void onContinue();
    void onCancel();

private:
    void initTableOverView();
    void initTableDetails();

private:
    Ui::DbValidationDialog* mUi;
};
}
