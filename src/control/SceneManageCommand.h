/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    SceneManageCommand.h
 *******************************************************************************
 */

#pragma once

#include "BasicCommand.h"
#include <osg/Polytope>

#include "model/BoundingBox.h"

namespace Controller
{
class SceneManageCommand : public BasicCommand
{
public:
    void execute(PureMVC::Interfaces::INotification const& aNotification) override;
    static std::string GetCommandName();
private:
    void constructScene(osg::Polytope &aPolytope);
    void convertBoundingbox(const Model::BoundingBoxPtr& aBoundingBox, osg::BoundingBox& aResult);
};
}

