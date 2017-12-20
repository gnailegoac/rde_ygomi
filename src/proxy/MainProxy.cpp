/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    MainProxy.cpp
 *******************************************************************************
 */

#include "MainProxy.h"
#include "model/MemoryModel.h"
#include "model/SceneModel.h"

MainProxy::MainProxy():
    Proxy("MainProxy")
{

}

MainProxy::MainProxy(const std::string& aName):
    Proxy(aName),
    mSceneModel(nullptr),
    mMemoryModel(nullptr)
{

}

const std::shared_ptr<Model::SceneModel>& MainProxy::GetSceneModel() const
{
    return mSceneModel;
}

void MainProxy::SetSceneModel(const std::shared_ptr<Model::SceneModel>& aSceneModel)
{
    mSceneModel = aSceneModel;
}

const std::shared_ptr<Model::MemoryModel>& MainProxy::GetMemoryModel() const
{
    return mMemoryModel;
}

void MainProxy::SetMemoryModel(const std::shared_ptr<Model::MemoryModel>& aMemoryModel)
{
    mMemoryModel = aMemoryModel;
}
