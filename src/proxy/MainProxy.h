/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    MainProxy.h
 *******************************************************************************
 */

#pragma once

#include <PureMVC/PureMVC.hpp>

class MainProxy : public PureMVC::Patterns::Proxy
{
public:
    MainProxy();
    MainProxy(const std::string& aName);
};
