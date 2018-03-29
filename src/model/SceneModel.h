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
class MemoryModel;

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
    void RedrawRoadMarks(const std::uint8_t& aLevel);
    void RedrawSceneByLOD(const std::shared_ptr<MemoryModel>& aMemoryModel, const std::uint8_t& aLevel);
    static std::vector<osg::Node*> GetLineNodesByRoadNode(osg::Node* aNode);
    static uint64_t GetIdByNodeName(const std::string& aNodeName);

    void addErrPointsToScene(int id, const std::vector<Point3D>& pointList, const Point3D& refPoint);
    void showErrPoints(int id);

private:
    osg::ref_ptr<osg::Node> buildLineNode(const Model::LinePtr& aLine);
    osg::ref_ptr<osg::Group> buildRoadNode(const std::shared_ptr<Model::Road>& aRoad);
    osg::ref_ptr<osg::Group> buildTrafficSignNode(const std::shared_ptr<Model::TrafficSign>& aTrafficSign);
    osg::ref_ptr<osg::Geode> createBox(const osg::Vec3d& aCenter, const std::string& aTexturePath);
    osg::ref_ptr<osg::Geometry> createLaneGeometry(const std::shared_ptr<Model::Lane>& aLane, const int& aLevel);
    osg::ref_ptr<osg::Node> buildLaneNode(const std::shared_ptr<Model::Lane>& aLane, const int& aLevel);
    void updateLaneNode(osg::ref_ptr<osg::Geode>& geode, const std::shared_ptr<Model::Lane>& aLane, const int& aLevel);
    osg::ref_ptr<osg::Group> buildRoadModelNode(const std::shared_ptr<Model::Road>& aRoad);
    osg::ref_ptr<osg::Group> buildRoadMarksModelNode(const std::shared_ptr<Model::Road>& aRoad);
    double distance(const osg::Vec3& aP1, const osg::Vec3& aP2);
    bool createRoadTriangles(const osg::ref_ptr<osg::Vec3Array>& aVertexArray,
                             int aRightLinePointsNum, osg::ref_ptr<osg::DrawElementsUInt>& aTris);
    void createRoadTexture(const std::string& aRoadTextureFile, osg::ref_ptr<osg::Geometry>& aRoadGeometry);

    osg::ref_ptr<osg::Node> buildErrPointsNode(int id, const std::vector<Point3D>& pointList, const Point3D& refPoint);

private:
    osg::ref_ptr<osg::Group> mSceneModelRoot;
    std::map<std::uint64_t, osg::ref_ptr<osg::Node>> mRoadNodeMap;
    std::map<std::uint64_t, osg::ref_ptr<osg::Node>> mLineNodeMap;
    std::map<std::uint64_t, osg::ref_ptr<osg::Node>> mTrafficSignNodeMap;
    std::map<std::uint64_t, osg::ref_ptr<osg::Node>> mRoadModelNodeMap;
    std::map<std::uint64_t, osg::ref_ptr<osg::Node>> mLaneNodeMap;
    std::map<std::uint64_t, osg::ref_ptr<osg::Node>> mMarksNodeMap;
    std::map<int, osg::ref_ptr<osg::Node>> mErrNodeMap;
    std::uint8_t mLevel;
};
}
