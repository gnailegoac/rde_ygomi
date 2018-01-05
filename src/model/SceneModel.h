/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    SceneModel.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include <osg/Geometry>
#include <osg/Geode>
#include <map>
#include <memory>
#include <osg/Node>
#include "model/Road.h"

namespace Model
{
    class SceneModel
    {
    public:
        SceneModel();
        const osg::ref_ptr<osg::Group>& GetSceneModelRoot() const;
        void AddRoadToScene(const std::shared_ptr<Model::Road>& aRoad);
        void RemoveRoadFromScene(const std::uint64_t& aRoadId);
        const osg::ref_ptr<osg::Node>& GetLineNodeById(const std::uint64_t& aLineId) const;
    private:
        osg::ref_ptr<osg::Node> buildLineNode(const Model::LinePtr& aLine);
        osg::ref_ptr<osg::Group> buildRoadNode(const std::shared_ptr<Model::Road>& aRoad);

        osg::ref_ptr<osg::Group> mSceneModelRoot;
        std::map<std::uint64_t, osg::ref_ptr<osg::Node>> mRoadNodeMap;
        std::map<std::uint64_t, osg::ref_ptr<osg::Node>> mLineNodeMap;
    };
}
