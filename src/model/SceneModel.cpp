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
    mTrafficSignNodeMap[aTrafficSign->GetTrafficSignId()] = trafficSignGroup;
}

void Model::SceneModel::RemoveTrafficSignFromScene(const std::uint64_t& aTrafficSignId)
{
    if (mTrafficSignNodeMap.find(aTrafficSignId) != mTrafficSignNodeMap.end())
    {
        mSceneModelRoot->removeChild(mTrafficSignNodeMap[aTrafficSignId].release());
        mTrafficSignNodeMap.erase(aTrafficSignId);
    }
}

void Model::SceneModel::RotateTrafficSign(const osg::Matrixd& aMatrix)
{
    auto rotate = [](const osg::Matrixd& aPose, const osg::Vec3& point)->osg::Vec3{
        osg::Vec3 newPoint;
        newPoint[0] = point[0] * aPose(0, 0) + point[1] * aPose(1, 0) + point[2] * aPose(2, 0);
        newPoint[1] = point[0] * aPose(0, 1) + point[1] * aPose(1, 1) + point[2] * aPose(2, 1);
        newPoint[2] = point[0] * aPose(0, 2) + point[1] * aPose(1, 2) + point[2] * aPose(2, 2);
        return newPoint;
    };

    for (const auto& iter : mTrafficSignNodeMap)
    {
        osg::ref_ptr<osg::Group> trafficSignGroup = iter.second->asGroup();
        osg::Geode* geode = trafficSignGroup->getChild(0)->asGeode();
        osg::ref_ptr<osg::Geometry> geometry = geode->getDrawable(0)->asGeometry();
        osg::Vec3Array* v = static_cast<osg::Vec3Array*>(geometry->getVertexArray());
        const osg::Vec3* array = static_cast<const osg::Vec3*>(v->getDataPointer());
        float center_x = (array[0][0] + array[1][0] + array[2][0] + array[3][0]) / 4.;
        float center_y = (array[0][1] + array[1][1] + array[2][1] + array[3][1]) / 4.;
        float center_z = (array[0][2] + array[1][2] + array[2][2] + array[3][2]) / 4.;
        osg::Vec3 vertex0 = rotate(aMatrix, osg::Vec3(VIEW_SIGN_WIDTH, -VIEW_SIGN_LENGTH, 0));
        osg::Vec3 vertex1 = rotate(aMatrix, osg::Vec3(-VIEW_SIGN_WIDTH, -VIEW_SIGN_LENGTH, 0));
        osg::Vec3 vertex2 = rotate(aMatrix, osg::Vec3(-VIEW_SIGN_WIDTH, VIEW_SIGN_LENGTH, 0));
        osg::Vec3 vertex3 = rotate(aMatrix, osg::Vec3(VIEW_SIGN_WIDTH, VIEW_SIGN_LENGTH, 0));
        osg::Vec3 center(center_x, center_y, center_z);
        (*v)[0].set(center + vertex0);
        (*v)[1].set(center + vertex1);
        (*v)[2].set(center + vertex2);
        (*v)[3].set(center + vertex3);
        geometry->dirtyDisplayList();
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

osg::ref_ptr<osg::Group> Model::SceneModel::buildTrafficSignNode(const std::shared_ptr<Model::TrafficSign>& aTrafficSign)
{
    osg::ref_ptr<osg::Group> trafficSignNode(new osg::Group);
    trafficSignNode->setName("Sign:" + std::to_string(aTrafficSign->GetTrafficSignId()));
    osg::Geode* geode = new osg::Geode();
    geode->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
    const Point3DPtr& position = aTrafficSign->GetViewPosition();

    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
    v->push_back(osg::Vec3(position->GetX() + VIEW_SIGN_WIDTH, position->GetY() - VIEW_SIGN_LENGTH, position->GetZ()));
    v->push_back(osg::Vec3(position->GetX() - VIEW_SIGN_WIDTH, position->GetY() - VIEW_SIGN_LENGTH, position->GetZ()));
    v->push_back(osg::Vec3(position->GetX() - VIEW_SIGN_WIDTH, position->GetY() + VIEW_SIGN_LENGTH, position->GetZ()));
    v->push_back(osg::Vec3(position->GetX() + VIEW_SIGN_WIDTH, position->GetY() + VIEW_SIGN_LENGTH, position->GetZ()));
    geometry->setVertexArray(v.get());

    osg::ref_ptr<osg::Vec2Array> tcoords = new osg::Vec2Array();
    tcoords->push_back(osg::Vec2(1.0f, 0.0f));
    tcoords->push_back(osg::Vec2(0.0f, 0.0f));
    tcoords->push_back(osg::Vec2(0.0f, 1.0f));
    tcoords->push_back(osg::Vec2(1.0f, 1.0f));
    geometry->setTexCoordArray(0, tcoords.get());
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
    osg::ref_ptr<osg::Image> image = osgDB::readImageFile(aTrafficSign->GetImagePath());
    texture->setImage(image);
    geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
    geode->addDrawable(geometry);
    trafficSignNode->addChild(geode);

    return trafficSignNode;
}

const osg::ref_ptr<osg::Node>& Model::SceneModel::GetLineNodeById(const std::uint64_t& aLineId) const
{
    if(mLineNodeMap.find(aLineId) != mLineNodeMap.end())
    {
        return mLineNodeMap.at(aLineId);
    }

    return nullptr;
}
