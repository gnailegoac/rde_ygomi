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

namespace Model
{
    class SceneModel;
    class MemoryModel;
    class TreeModel;
}

class MainProxy : public PureMVC::Patterns::Proxy
{
public:
    static const std::string NAME;
    MainProxy();
    MainProxy(const std::string& aName);

    const std::shared_ptr<Model::SceneModel>& GetSceneModel() const;
    std::shared_ptr<Model::SceneModel>& GetMutableSceneModel();
    void SetSceneModel(const std::shared_ptr<Model::SceneModel>& aSceneModel);

    const std::shared_ptr<Model::MemoryModel>& GetMemoryModel() const;
    void SetMemoryModel(const std::shared_ptr<Model::MemoryModel>& aMemoryModel);

    const std::shared_ptr<Model::TreeModel>& GetTreeModel() const;
    void SetTreeModel(const std::shared_ptr<Model::TreeModel>& aTreeModel);
private:
    std::shared_ptr<Model::SceneModel> mSceneModel;
    std::shared_ptr<Model::MemoryModel> mMemoryModel;
    std::shared_ptr<Model::TreeModel> mTreeModel;
};
