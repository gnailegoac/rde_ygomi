/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    NetworkPreferenceProvider.h
 *******************************************************************************
 */

#pragma once
#include <memory>
#include <QNetworkProxy>

namespace Service
{

class NetworkPreferenceProvider
{
public:
    static const std::shared_ptr<NetworkPreferenceProvider>& Instance();
    const QNetworkProxy& GetNetworkProxy() const;
    const QString& GetWebServer() const;
    void SetNetworkProxy(const QNetworkProxy& aNetworkProxy);
    void SetWebServer(const QString& aWebServer);
    void SetCurrentApplicationProxy();

private:
    NetworkPreferenceProvider();
    QNetworkProxy readProxySetting();
    QString readWebServerSetting();
    void writeProxySetting(const QNetworkProxy& aNetworkProxy);
    void writeWebServerSetting(const QString& aWebServer);

    QString mWebServer;
    QNetworkProxy mNetworkProxy;
};

}
