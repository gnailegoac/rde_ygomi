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
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>

namespace  Model
{
    const static float VIEW_SIGN_WIDTH = 2.0;
    const static float VIEW_SIGN_LENGTH = 2.0;
}

Model::SceneModel::SceneModel() :
    mSceneModelRoot(new osg::Group)
{
    mSceneModelRoot->getOrCreateStateSet()->setMode(GL_LIGHTING,
                                                    osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
    mSceneModelRoot->setName("root");
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

void Model::SceneModel::AddTrafficSignToScene(const std::shared_ptr<Model::TrafficSign>& aTrafficSign)
{
    osg::ref_ptr<osg::Group> trafficSignGroup = buildTrafficSignNode(aTrafficSign);
    mSceneModelRoot->addChild(trafficSignGroup);
    mTrafficSignNodeMap[aTrafficSign->GetTrafficSignId()] = trafficSignGroup->getChild(0);
}

void Model::SceneModel::RemoveTrafficSignFromScene(const std::uint64_t& aTrafficSignId)
{
    if (mTrafficSignNodeMap.find(aTrafficSignId) != mTrafficSignNodeMap.end())
    {
        mSceneModelRoot->removeChild(mTrafficSignNodeMap[aTrafficSignId].release());
        mTrafficSignNodeMap.erase(aTrafficSignId);
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

    geode->setName("Line:" + std::to_string(aLine->GetLineId()));
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

osg::ref_ptr<osg::Geode> Model::SceneModel::createBox(const osg::Vec3d& aCenter, const std::string& aTexturePath)
{
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    const double boxSize = 30.0;
    osg::ref_ptr<osg::Box> box = new osg::Box(aCenter, boxSize, boxSize, boxSize);
    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable(box.get());
    osg::ref_ptr<osg::Texture2D> texture2D = new osg::Texture2D;
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile(aTexturePath);
    if (image.valid())
    {
        texture2D->setImage(image.get());
    }

    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
    geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
    geode->getOrCreateStateSet()->setTextureAttributeAndModes(0,
                                                              texture2D.get(),
                                                              osg::StateAttribute::ON);
    geode->addDrawable(shape.get());
    return geode;
}

osg::ref_ptr<osg::Group> Model::SceneModel::buildTrafficSignNode(const std::shared_ptr<Model::TrafficSign>& aTrafficSign)
{
    const Point3DPtr& position = aTrafficSign->GetViewPosition();
    osg::ref_ptr<osg::Geode> geode = createBox(osg::Vec3d(position->GetX(), position->GetY(), position->GetZ()),
                                               aTrafficSign->GetImagePath());
    geode->setName("Sign:" + std::to_string(aTrafficSign->GetTrafficSignId()));
    osg::ref_ptr<osg::Group> group = new osg::Group;
    group->addChild(geode);
    return group;
}

const osg::ref_ptr<osg::Node>& Model::SceneModel::GetLineNodeById(const std::uint64_t& aLineId) const
{
    if(mLineNodeMap.find(aLineId) != mLineNodeMap.end())
    {
        return mLineNodeMap.at(aLineId);
    }

    return nullptr;
}

const osg::ref_ptr<osg::Node>& Model::SceneModel::GetTrafficSignNodeById(const std::uint64_t& aLineId) const
{
    if(mTrafficSignNodeMap.find(aLineId) != mTrafficSignNodeMap.end())
    {
        return mTrafficSignNodeMap.at(aLineId);
    }

    return nullptr;
}
