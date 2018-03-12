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
    mWebServer = readWebServerSetting();
}

const std::shared_ptr<Service::NetworkPreferenceProvider>& Service::NetworkPreferenceProvider::Instance()
{
    static std::shared_ptr<Service::NetworkPreferenceProvider> networkPreferenceProvider(new Service::NetworkPreferenceProvider());
    return networkPreferenceProvider;
}

const QNetworkProxy& Service::NetworkPreferenceProvider::GetNetworkProxy() const
{
    return mNetworkProxy;
}

const QString& Service::NetworkPreferenceProvider::GetWebServer() const
{
    return mWebServer;
}

void Service::NetworkPreferenceProvider::SetNetworkProxy(const QNetworkProxy& aNetworkProxy)
{
    mNetworkProxy = aNetworkProxy;

    SetCurrentApplicationProxy();
    writeProxySetting(mNetworkProxy);
}

void Service::NetworkPreferenceProvider::SetWebServer(const QString& aWebServer)
{
    mWebServer = aWebServer;
    writeWebServerSetting(mWebServer);
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
    QString hostName = settings.value("proxyServer").toString();
    quint16 port = quint16(settings.value("proxyPort", 0).toUInt());
    bool useCrendetial = settings.value("useCredential", false).toBool();
    QString username = settings.value("username").toString();
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

QString Service::NetworkPreferenceProvider::readWebServerSetting()
{
    QSettings settings;
    settings.beginGroup("WebServer");
    mWebServer = settings.value("webServer").toString();
    if (mWebServer.isEmpty())
    {
        SetWebServer(scDefaultWebServer);
    }
    return mWebServer;
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
        settings.setValue("proxyServer", aNetworkProxy.hostName());
        settings.setValue("proxyPort", aNetworkProxy.port());
        bool useCredential = !aNetworkProxy.user().isEmpty() &&
                             !aNetworkProxy.password().isEmpty();

        settings.setValue("useCredential", useCredential);
        if (useCredential)
        {
            settings.setValue("username", aNetworkProxy.user());
            settings.setValue("password", aNetworkProxy.password());
        }
    }

    settings.endGroup();
}

void Service::NetworkPreferenceProvider::writeWebServerSetting(const QString& aWebServer)
{
    QSettings settings;
    settings.beginGroup("WebServer");
    settings.setValue("webServer", aWebServer);
    settings.endGroup();
}
