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
#include "model/QIModel.h"

const std::string MainProxy::NAME = "MainProxy";

MainProxy::MainProxy():
    Proxy("MainProxy"),
    mSceneModel(nullptr),
    mMemoryModel(nullptr),
    mTreeModel(nullptr)
{

}

MainProxy::MainProxy(const std::string& aName):
    Proxy(aName),
    mSceneModel(nullptr),
    mMemoryModel(nullptr),
    mTreeModel(nullptr)
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

const std::shared_ptr<Model::TreeModel>& MainProxy::GetTreeModel() const
{
    return mTreeModel;
}

void MainProxy::SetTreeModel(const std::shared_ptr<Model::TreeModel>& aTreeModel)
{
    mTreeModel = aTreeModel;
}

const std::shared_ptr<Model::QIModel>& MainProxy::GetQIModel() const
{
    static std::shared_ptr<Model::QIModel> qiModel(new Model::QIModel());
    return qiModel;
}
