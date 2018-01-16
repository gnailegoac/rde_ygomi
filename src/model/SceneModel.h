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
#include "model/TrafficSign.h"

namespace Model
{
    class SceneModel
    {
    public:
        SceneModel();
        const osg::ref_ptr<osg::Group>& GetSceneModelRoot() const;
        void AddRoadToScene(const std::shared_ptr<Model::Road>& aRoad);
        void RemoveRoadFromScene(const std::uint64_t& aRoadId);
        void AddTrafficSignToScene(const std::shared_ptr<Model::TrafficSign>& aTrafficSign);
        void RemoveTrafficSignFromScene(const std::uint64_t& aTrafficSignId);
        void RotateTrafficSign(const osg::Matrixd& aMatrix);
        const osg::ref_ptr<osg::Node>& GetLineNodeById(const std::uint64_t& aLineId) const;
    private:
        osg::ref_ptr<osg::Node>  buildLineNode(const Model::LinePtr& aLine);
        osg::ref_ptr<osg::Group> buildRoadNode(const std::shared_ptr<Model::Road>& aRoad);
        osg::ref_ptr<osg::Group> buildTrafficSignNode(const std::shared_ptr<Model::TrafficSign>& aTrafficSign);

        osg::ref_ptr<osg::Group> mSceneModelRoot;
        std::map<std::uint64_t, osg::ref_ptr<osg::Node>> mRoadNodeMap;
        std::map<std::uint64_t, osg::ref_ptr<osg::Node>> mLineNodeMap;
        std::map<std::uint64_t, osg::ref_ptr<osg::Node>> mTrafficSignNodeMap;
    };
}
