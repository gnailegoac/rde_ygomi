/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    NetworkPreferenceDialog.cpp
 * @brief
 *******************************************************************************
 */

#include "NetworkPreferenceDialog.h"
#include "ui_NetworkPreferenceDialog.h"
#include <QVariant>
#include "service/NetworkPreferenceProvider.h"

Q_DECLARE_METATYPE(QNetworkProxy::ProxyType)

View::NetworkPreferenceDialog::NetworkPreferenceDialog(QWidget *parent) :
    QDialog(parent),
    mSettingChanged(false),
    mUi(new Ui::NetworkPreferenceDialog)
{
    mUi->setupUi(this);
    mUi->proxyType->addItem("Use system proxy", QNetworkProxy::DefaultProxy);
    mUi->proxyType->addItem("Http proxy", QNetworkProxy::HttpProxy);

    connect(mUi->proxyType, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            [=]()
    {
        QNetworkProxy::ProxyType proxyType = currentProxyType();
        bool enableInput = (proxyType == QNetworkProxy::HttpProxy);
        mUi->proxyServer->setEnabled(enableInput);
        mUi->proxyPort->setEnabled(enableInput);

        mUi->credential->setEnabled(enableInput);
        if (!enableInput)
        {
            mUi->credential->setChecked(enableInput);
        }

        setPreferenceChanged();
    });

    auto valueChanged = [=]()
    {
        setPreferenceChanged();
    };

    connect(mUi->proxyServer, &QLineEdit::textChanged, valueChanged);
    connect(mUi->proxyPort, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), valueChanged);
    connect(mUi->credential, &QCheckBox::stateChanged, [=]
    {
        mSettingChanged = true;
        mUi->username->setEnabled(mUi->credential->isChecked());
        mUi->password->setEnabled(mUi->credential->isChecked());
    });
    connect(mUi->username, &QLineEdit::textChanged, valueChanged);
    connect(mUi->password, &QLineEdit::textChanged, valueChanged);
    connect(mUi->cancelBtn, &QPushButton::clicked, this, &QDialog::close);
    connect(mUi->okBtn, &QPushButton::clicked, [=]
    {
        if (mSettingChanged)
        {
            SaveSettings();
        }
        accept();
    });

    populateSettings();
}

View::NetworkPreferenceDialog::~NetworkPreferenceDialog()
{
    delete mUi;
}

void View::NetworkPreferenceDialog::populateSettings()
{
    auto networkSettings = Service::NetworkPreferenceProvider::Instance();
    mUi->webServer->setText(networkSettings->GetWebServer());
    QNetworkProxy networkProxy = networkSettings->GetNetworkProxy();
    if (networkProxy.type() == QNetworkProxy::HttpProxy)
    {
        mUi->proxyServer->setText(networkProxy.hostName());
        mUi->proxyPort->setValue(networkProxy.port());
        bool useCredential = !networkProxy.user().isEmpty() &&
                             !networkProxy.password().isEmpty();
        mUi->credential->setChecked(useCredential);
        if (useCredential)
        {
            mUi->username->setText(networkProxy.user());
            mUi->password->setText(networkProxy.password());
        }
        mUi->proxyType->setCurrentIndex(1);
    }
    else
    {
        mUi->proxyType->setCurrentIndex(0);
    }
}

QNetworkProxy::ProxyType View::NetworkPreferenceDialog::currentProxyType()
{
    QVariant userData = mUi->proxyType->currentData();
    return userData.value<QNetworkProxy::ProxyType>();
}

void View::NetworkPreferenceDialog::SaveSettings()
{
    QNetworkProxy networkProxy(currentProxyType());
    if (currentProxyType() == QNetworkProxy::HttpProxy)
    {
        networkProxy.setHostName(mUi->proxyServer->text());
        networkProxy.setPort(quint16(mUi->proxyPort->value()));
        if (mUi->credential->isChecked() &&
            !mUi->username->text().isEmpty() &&
            !mUi->password->text().isEmpty())
        {
            networkProxy.setUser(mUi->username->text());
            networkProxy.setPassword(mUi->password->text());
        }
    }

    auto networkSettings = Service::NetworkPreferenceProvider::Instance();
    networkSettings->SetNetworkProxy(networkProxy);
    networkSettings->SetWebServer(mUi->webServer->text());
}

void View::NetworkPreferenceDialog::setPreferenceChanged()
{
    mSettingChanged = true;
}

