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

#include <QNetworkProxy>

namespace Service
{

class NetworkPreferenceProvider
{
public:
    NetworkPreferenceProvider();

    const QNetworkProxy& GetNetworkProxy() const;
    void SetNetworkProxy(const QNetworkProxy& aNetworkProxy);

    void SetCurrentApplicationProxy();

private:
    QNetworkProxy readProxySetting();
    void writeProxySetting(const QNetworkProxy& aNetworkProxy);

    QNetworkProxy mNetworkProxy;
};

}
