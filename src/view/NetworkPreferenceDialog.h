/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    NetworkPreferenceDialog.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include <QDialog>
#include <QNetworkProxy>

namespace Ui {
class NetworkPreferenceDialog;
}

namespace View {

class NetworkPreferenceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkPreferenceDialog(QWidget *parent = 0);
    ~NetworkPreferenceDialog();

    void SaveSettings();
private:
    void setPreferenceChanged();
    void populateSettings();
    QNetworkProxy::ProxyType currentProxyType();

    bool mSettingChanged;
    Ui::NetworkPreferenceDialog *mUi;
};
}
