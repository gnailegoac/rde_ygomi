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
#include <osgText/Text>
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
        const osg::ref_ptr<osg::Node>& GetLineNodeById(const std::uint64_t& aLineId) const;
        const osg::ref_ptr<osg::Node>& GetTrafficSignNodeById(const std::uint64_t& aLineId) const;
        void AddRoadModelToScene(const std::shared_ptr<Model::Road>& aRoad);
        void RemoveRoadModelFromScene();

    private:
        osg::ref_ptr<osg::Node>  buildLineNode(const Model::LinePtr& aLine);
        osg::ref_ptr<osg::Group> buildRoadNode(const std::shared_ptr<Model::Road>& aRoad);
        osg::ref_ptr<osg::Group> buildTrafficSignNode(const std::shared_ptr<Model::TrafficSign>& aTrafficSign);
        osg::ref_ptr<osg::Geode> createBox(const osg::Vec3d& aCenter, const std::string& aTexturePath);
        osg::ref_ptr<osg::Group> buildRoadModelNode(const std::shared_ptr<Model::Road>& aRoad);
        osg::ref_ptr<osg::Group> buildRoadMarksModelNode(const std::shared_ptr<Model::Road>& aRoad);
        double distance(osg::Vec3 p1,osg::Vec3 p2);
        bool createRoadTriangles(const osg::ref_ptr<osg::Vec3Array> &vertexArray,
                                  int rightLinePointsNum,osg::ref_ptr<osg::DrawElementsUInt> &tris);
        bool createRoadTriangles_(const osg::ref_ptr<osg::Vec3Array> &vertexArray,
                                  int rightLinePointsNum,osg::ref_ptr<osg::DrawElementsUInt> &tris);
        void createRoadTexture(const std::string roadTextureFile, osg::ref_ptr<osg::Geometry> &roadGeometry);
        osg::ref_ptr<osg::Node> createRoadMarksNode(const Model::LinePtr& aLine);
        void createMarksTriangles(const osg::Vec3& center, const osg::Vec3& normal,
                                  osg::ref_ptr<osg::Vec3Array> &vertexArray,
                                  osg::ref_ptr<osg::DrawElementsUInt> &tris);
        osg::Vec3 cross(osg::Vec3 v1, osg::Vec3 v2);
        bool any(osg::Vec3 vec3);
        bool normalize(osg::Vec3 &vec3);
        osgText::Text* createContent(osg::Vec3 position, int num);

    private:
        osg::ref_ptr<osg::Group> mSceneModelRoot;
        std::map<std::uint64_t, osg::ref_ptr<osg::Node>> mRoadNodeMap;
        std::map<std::uint64_t, osg::ref_ptr<osg::Node>> mLineNodeMap;
        std::map<std::uint64_t, osg::ref_ptr<osg::Node>> mTrafficSignNodeMap;
        std::map<std::uint64_t, osg::ref_ptr<osg::Node>> mRoadModelNodeMap;
        std::map<std::uint64_t, osg::ref_ptr<osg::Node>> mMarksNodeMap;
    };
}
