/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    SceneModel.cpp
 * @brief
 *******************************************************************************
 */

#include "SceneModel.h"
#include "model/Lane.h"
#include "model/Line.h"

#include <osg/Material>
#include <osg/LineWidth>

Model::SceneModel::SceneModel() :
    mSceneModelRoot(new osg::Group)
{

}

const osg::ref_ptr<osg::Group>& Model::SceneModel::GetSceneModelRoot() const
{
    return mSceneModelRoot;
}

void Model::SceneModel::AddRoadToScene(const std::shared_ptr<Model::Road>& aRoad)
{
    osg::ref_ptr<osg::Group> roadGroup = buildRoadNode(aRoad);
    mSceneModelRoot->addChild(roadGroup);
    mRoadNodeMap[aRoad->GetRoadId()] = roadGroup;
}

void Model::SceneModel::RemoveRoadFromScene(const std::uint64_t& aRoadId)
{
    if (mRoadNodeMap.find(aRoadId) != mRoadNodeMap.end())
    {
        mSceneModelRoot->removeChild(mRoadNodeMap[aRoadId].release());
        mRoadNodeMap.erase(aRoadId);
    }
}

osg::ref_ptr<osg::Node> Model::SceneModel::buildLineNode(const Model::LinePtr& aLine)
{
    Model::PaintListPtr viewPointListPtr = aLine->GetPaintListByLevel(2);
    if(viewPointListPtr->size() == 0)
    {
        return nullptr;
    }

    osg::Geode* geode = new osg::Geode;
    osg::ref_ptr<osg::LineWidth> width = new osg::LineWidth;
    width->setWidth(1.0f);
    geode->getOrCreateStateSet()->setAttributeAndModes(width, osg::StateAttribute::ON);

    osg::ref_ptr<osg::Material> material = new osg::Material;
    material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    material->setTransparency(osg::Material::FRONT_AND_BACK, 0.9);
    geode->getOrCreateStateSet()->setAttributeAndModes(material,
                                                       osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    for (Point3DListPtr& points : *viewPointListPtr)
    {
        if (0 == points->size())
        {
            continue;
        }

        osg::ref_ptr<osg::Vec3dArray> vertexArray = new osg::Vec3dArray;
        for (auto& point : *points)
        {
            vertexArray->push_back(osg::Vec3d(point->GetX(), point->GetY(), point->GetZ()));
        }

        osg::Geometry* geometry = new osg::Geometry;
        geometry->addPrimitiveSet(new osg::DrawArrays(osg::DrawArrays::LINE_STRIP, 0, vertexArray->size()));
        geometry->setVertexArray(vertexArray);

        geode->addDrawable(geometry);
    }

    geode->setName("Line:" +std::to_string(aLine->GetLineId()));
    return geode;
}

osg::ref_ptr<osg::Group> Model::SceneModel::buildRoadNode(const std::shared_ptr<Model::Road>& aRoad)
{
    osg::ref_ptr<osg::Group> roadNode(new osg::Group);
    roadNode->setName("Road:" + std::to_string(aRoad->GetRoadId()));
    Model::LaneListPtr laneListptr = aRoad->GetLaneList();
    for(const auto& lane : *laneListptr.get())
    {
        Model::LinePtr rightLine = lane->GetRightLine();
        Model::LinePtr leftLine = lane->GetLeftLine();
        osg::Group* laneNode = nullptr;

        if(mLineNodeMap.find(leftLine->GetLineId()) == mLineNodeMap.end())
        {
            if(laneNode == nullptr)
            {
                laneNode = new osg::Group;
            }

            osg::ref_ptr<osg::Node> leftLineNode = buildLineNode(leftLine);
            laneNode->addChild(leftLineNode);
            mLineNodeMap[leftLine->GetLineId()] = leftLineNode;
        }

        if(mLineNodeMap.find(rightLine->GetLineId()) == mLineNodeMap.end())
        {
            if(laneNode == nullptr)
            {
                laneNode = new osg::Group;
            }

            osg::ref_ptr<osg::Node> rightLineNode = buildLineNode(rightLine);
            laneNode->addChild(rightLineNode);
            mLineNodeMap[rightLine->GetLineId()] = rightLineNode;
        }

        if(laneNode != nullptr)
        {
            laneNode->setName("Lane:" + std::to_string(lane->GetLaneId()));
            roadNode->addChild(laneNode);
        }
    }

    return roadNode;
}

const osg::ref_ptr<osg::Node>& Model::SceneModel::GetLineNodeById(const std::uint64_t& aLineId) const
{
    if(mLineNodeMap.find(aLineId) != mLineNodeMap.end())
    {
        return mLineNodeMap.at(aLineId);
    }

    return nullptr;
}
