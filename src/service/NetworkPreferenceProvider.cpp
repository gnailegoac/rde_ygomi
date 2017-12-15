/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    NetworkPreferenceProvider.cpp
 *******************************************************************************
 */

#include "service/NetworkPreferenceProvider.h"

#include <QSettings>

Service::NetworkPreferenceProvider::NetworkPreferenceProvider()
{
    mNetworkProxy = readProxySetting();
}

const QNetworkProxy& Service::NetworkPreferenceProvider::GetNetworkProxy() const
{
    return mNetworkProxy;
}

void Service::NetworkPreferenceProvider::SetNetworkProxy(const QNetworkProxy& aNetworkProxy)
{
    mNetworkProxy = aNetworkProxy;

    SetCurrentApplicationProxy();
    writeProxySetting(mNetworkProxy);
}

void Service::NetworkPreferenceProvider::SetCurrentApplicationProxy()
{
    if (mNetworkProxy.type() == QNetworkProxy::DefaultProxy)
    {
        QNetworkProxyFactory::setUseSystemConfiguration(true);
    }
    else if (mNetworkProxy.type() == QNetworkProxy::HttpProxy)
    {
        QNetworkProxy::setApplicationProxy(mNetworkProxy);
    }

    // Others proxy type, ignored.
}

QNetworkProxy Service::NetworkPreferenceProvider::readProxySetting()
{
    QSettings settings;
    settings.beginGroup("proxy");
    QString proxyType = settings.value("proxyType").toString();
    QString hostName = settings.value("hostName").toString();
    quint16 port = quint16(settings.value("port", 0).toUInt());
    bool useCrendetial = settings.value("useCredential", false).toBool();
    QString username = settings.value("user").toString();
    QString password = settings.value("password").toString();
    settings.endGroup();

    QNetworkProxy networkProxy;
    if (proxyType == "HttpProxy")
    {
        networkProxy.setType(QNetworkProxy::HttpProxy);
        networkProxy.setHostName(hostName);
        networkProxy.setPort(port);
        if (useCrendetial)
        {
            networkProxy.setPassword(password);
            networkProxy.setUser(username);
        }
    }
    else if (proxyType == "DefaultProxy")
    {
        networkProxy.setType(QNetworkProxy::DefaultProxy);
    }
    else
    {
        networkProxy.setType(QNetworkProxy::NoProxy);
    }

    return networkProxy;
}

void Service::NetworkPreferenceProvider::writeProxySetting(const QNetworkProxy& aNetworkProxy)
{
    QString proxyType = "NoProxy";
    switch (aNetworkProxy.type())
    {
        case QNetworkProxy::DefaultProxy:
            proxyType = "DefaultProxy";
            break;
        case QNetworkProxy::HttpProxy:
            proxyType = "HttpProxy";
            break;
        default:
            break;
    }

    QSettings settings;
    settings.beginGroup("proxy");
    settings.setValue("proxyType", proxyType);

    if (aNetworkProxy.type() == QNetworkProxy::HttpProxy)
    {
        settings.setValue("hostName", aNetworkProxy.hostName());
        settings.setValue("port", aNetworkProxy.port());
        bool useCredential = !aNetworkProxy.user().isEmpty() &&
                             !aNetworkProxy.password().isEmpty();

        settings.setValue("useCredential", useCredential);
        if (useCredential)
        {
            settings.setValue("user", aNetworkProxy.user());
            settings.setValue("password", aNetworkProxy.password());
        }
    }

    settings.endGroup();
}
