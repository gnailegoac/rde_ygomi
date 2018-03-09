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
#include "model/Utilities.h"
#include "model/MemoryModel.h"

#include <osg/Material>
#include <osg/LineWidth>
#include <osg/Point>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osgUtil/Tessellator>
#include <osgUtil/DelaunayTriangulator>
#include <osg/PolygonMode>
#include <osg/BlendFunc>
#include <stdlib.h>
#include <QDebug>
#include <QCoreApplication>
#include <QGLWidget>

namespace  Model
{
const static float VIEW_SIGN_WIDTH = 2.0;
const static float VIEW_SIGN_LENGTH = 2.0;
}

Model::SceneModel::SceneModel() :
    mSceneModelRoot(new osg::Group),
    mLevel(1)
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
        // Should remove line from mLineNodeMap frist.
        std::vector<osg::Node*> lineNodeList = GetLineNodesByRoadNode(mRoadNodeMap[aRoadId]);
        for (const auto& lineNode : lineNodeList)
        {
            uint64_t lineId = GetIdByNodeName(lineNode->getName());
            mLineNodeMap.erase(lineId);
        }
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
    Model::PaintListPtr viewPointListPtr = aLine->GetPaintListByLevel(mLevel);
    if(viewPointListPtr->size() == 0)
    {
        return nullptr;
    }

    osg::Geode* geode = new osg::Geode;
    osg::ref_ptr<osg::LineWidth> width = new osg::LineWidth;
    width->setWidth(1.0f);

    geode->getOrCreateStateSet()->setAttributeAndModes(width, osg::StateAttribute::ON);

    osg::ref_ptr<osg::Material> material = new osg::Material;
    material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
    material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
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
            laneNode->setName("LaneModel:" + std::to_string(lane->GetLaneId()));
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

void Model::SceneModel::AddRoadModelToScene(const std::shared_ptr<Model::Road>& aRoad)
{
    osg::ref_ptr<osg::Group> roadGroup = buildRoadModelNode(aRoad);
    mSceneModelRoot->addChild(roadGroup);
    mRoadModelNodeMap[aRoad->GetRoadId()] = roadGroup;
}

void Model::SceneModel::RemoveRoadModelFromScene()
{
    for(auto& roadNode : mRoadModelNodeMap)
    {
        mSceneModelRoot->removeChild((roadNode.second).release());
    }
    mRoadNodeMap.clear();
    // TODO: shall we clear lineNodeMap?
}

osg::ref_ptr<osg::Geometry> Model::SceneModel::createLaneGeometry(const std::shared_ptr<Model::Lane>& aLane, const int& aLevel)
{
    Model::LinePtr rightLine = aLane->GetRightLine();
    Model::LinePtr leftLine = aLane->GetLeftLine();
    Model::PaintListPtr rightLinePointListPtr = rightLine->GetPaintListByLevel(aLevel);
    Model::PaintListPtr leftLinePointListPtr = leftLine->GetPaintListByLevel(aLevel);
    if(rightLinePointListPtr->size() == 0 || leftLinePointListPtr->size() == 0)
    {
        return NULL;
    }

    osg::ref_ptr<osg::Vec3Array> vertexArray = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec3Array> normalArray = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
    osg::ref_ptr<osg::Vec2Array> textureCoords = new osg::Vec2Array;
    for (Point3DListPtr& points : *rightLinePointListPtr)
    {
        if (0 == points->size())
        {
            continue;
        }
        for (auto& point : *points)
        {
            osg::Vec3d vec(osg::Vec3d(point->GetX(), point->GetY(), point->GetZ()));
            if(vertexArray->size() > 0 && distance((*vertexArray)[vertexArray->size() - 1], vec) < 0.1)
            {
                continue;
            }
            vertexArray->push_back(vec);
            textureCoords->push_back(osg::Vec2(1, 1));
        }
    }
    int rightLinePointsNum = vertexArray->size();
    for (Point3DListPtr& points : *leftLinePointListPtr)
    {
        if (0 == points->size())
        {
            continue;
        }
        for (auto& point : *points)
        {
            osg::Vec3d vec(osg::Vec3d(point->GetX(), point->GetY(), point->GetZ()));
            if(vertexArray->size() > 0 && distance((*vertexArray)[vertexArray->size() - 1], vec) < 0.1)
            {
                continue;
            }
            vertexArray->push_back(vec);
            textureCoords->push_back(osg::Vec2(0, 0));
        }
    }

    osg::ref_ptr<osg::DrawElementsUInt> tris = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
    if(!createRoadTriangles(vertexArray, rightLinePointsNum, tris))
    {
        return NULL;
    }

    normalArray->push_back(osg::Vec3d(0.0, 0.0, 1.0));
    colorArray->push_back(osg::Vec4d(0.9, 0.9, 0.9, 0.5));
    geometry->setVertexArray(vertexArray);
    geometry->setNormalArray(normalArray);
    geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
    geometry->setColorArray(colorArray);
    geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
    geometry->addPrimitiveSet(tris);
    geometry->setTexCoordArray(0, textureCoords.get());
    std::string path = ":/resource/image/RoadSurface.png";
    createRoadTexture(path, geometry);
    return geometry;
}

osg::ref_ptr<osg::Node> Model::SceneModel::buildLaneNode(const std::shared_ptr<Model::Lane>& aLane, const int& aLevel)
{
    osg::ref_ptr<osg::Geometry> geometry = createLaneGeometry(aLane, aLevel);
    if(!geometry)
    {
        return NULL;
    }
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(geometry.get());
    return geode;
}

void Model::SceneModel::updateLaneNode(osg::ref_ptr<osg::Geode>& geode, const std::shared_ptr<Model::Lane>& aLane, const int& aLevel)
{
    osg::ref_ptr<osg::Geometry> geometry = createLaneGeometry(aLane, aLevel);
    if(!geometry)
    {
        return;
    }
    geode->addDrawable(geometry.get());
}

osg::ref_ptr<osg::Group> Model::SceneModel::buildRoadModelNode(const std::shared_ptr<Model::Road>& aRoad)
{
    osg::ref_ptr<osg::Group> roadNode(new osg::Group);
    roadNode->setName("RoadModel:" + std::to_string(aRoad->GetRoadId()));
    Model::LaneListPtr laneListptr = aRoad->GetLaneList();
    for(const auto& lane : *laneListptr.get())
    {
        osg::ref_ptr<osg::Node> laneNode = buildLaneNode(lane, mLevel);
        if(laneNode)
        {
            roadNode->addChild(laneNode.get());
            std::uint64_t laneId = lane-> GetLaneId();
            mLaneNodeMap[laneId] = laneNode;
        }
    }
    return roadNode;
}

double Model::SceneModel::distance(const osg::Vec3& aP1, const osg::Vec3& aP2)
{
    double x = aP1.x() - aP2.x();
    double y = aP1.y() - aP2.y();
    double z = aP1.z() - aP2.z();
    return sqrt(x * x + y * y + z * z);
}

bool Model::SceneModel::createRoadTriangles(const osg::ref_ptr<osg::Vec3Array>& aVertexArray,
                                            int aRightLinePointsNum, osg::ref_ptr<osg::DrawElementsUInt>& aTris)
{
    int num = aVertexArray->size();
    if(num < 3)
    {
        return false;
    }

    int s = aRightLinePointsNum + 1;
    double preD = 0.0;
    double currD = 0.0;
    for(int i = 0; i < aRightLinePointsNum; ++i)
    {
        preD = distance((*aVertexArray)[i], (*aVertexArray)[s - 1]);
        for(s; s < num; ++s)
        {
            currD = distance((*aVertexArray)[i], (*aVertexArray)[s]);
            if(preD > currD)
            {
                aTris->push_back(s - 1);
                aTris->push_back(i);
                aTris->push_back(s);
                preD = currD;
            }
            else
            {
                break;
            }
        }
        if(i + 1 < aRightLinePointsNum)
        {
            aTris->push_back(i);
            aTris->push_back(i + 1);
            aTris->push_back(s - 1);
        }
    }
    return true;
}

void Model::SceneModel::createRoadTexture(const std::string& aRoadTextureFile, osg::ref_ptr<osg::Geometry>& aRoadGeometry)
{
    osg::ref_ptr<osg::Material> roadMaterial = new osg::Material();
    roadMaterial->setEmission(osg::Material::FRONT, osg::Vec4(1.0, 1.0, 1.0, 1.0));
    roadMaterial->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
    roadMaterial->setTransparency(osg::Material::FRONT_AND_BACK, 0.1);
    osg::Image* roadImage = osgDB::readImageFile(aRoadTextureFile);
    if (!roadImage)
    {
        roadImage = osgDB::readImageFile("../src/resource/image/NoTexture.png");
        osg::notify(osg::WARN) << "Couldn't load texture."  << std::endl;
        if (!roadImage)
        {
            return;
        }
    }

    osg::ref_ptr<osg::Texture2D> roadTexture = new osg::Texture2D;
    roadTexture->setDataVariance(osg::Object::DYNAMIC);
    roadTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    roadTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    roadTexture->setImage(roadImage);

    osg::StateSet* roadStateSet = aRoadGeometry->getOrCreateStateSet();
    roadStateSet->setAttribute(roadMaterial);
    roadStateSet->setTextureAttributeAndModes(0, roadTexture, osg::StateAttribute::ON);
    roadStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);
    osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
    blendFunc->setFunction(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA);
    roadStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    roadStateSet->setAttributeAndModes(blendFunc);
}

uint64_t Model::SceneModel::GetIdByNodeName(const std::string& aNodeName)
{
    std::vector<std::string> results;
    results = strings::Split(aNodeName, ":");
    if(results.size() != 2)
    {
        return 0;
    }

    return QString::fromStdString(results[1]).toULong();
}

std::vector<osg::Node*> Model::SceneModel::GetLineNodesByRoadNode(osg::Node* aNode)
{
    std::vector<osg::Node*> lineNodeList;
    osg::Group* roadNode = dynamic_cast<osg::Group*>(aNode);
    for(unsigned int i = 0; i < roadNode->getNumChildren(); ++i)
    {
        osg::Group* laneNode = dynamic_cast<osg::Group*>(roadNode->getChild(i));
        for (unsigned int j = 0; j < laneNode->getNumChildren(); ++j)
        {
            osg::Node* lineNode = laneNode->getChild(j);
            lineNodeList.push_back(lineNode);
        }
    }

    return lineNodeList;
}

void Model::SceneModel::RedrawRoadMarks(const uint8_t& aLevel)
{
    float width = aLevel;
    osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth;
    lineWidth->setWidth(width);
    for(const auto& node : mLineNodeMap)
    {
        osg::Geode* geode = dynamic_cast<osg::Geode*>((node.second).get());
        geode->getOrCreateStateSet()->setAttributeAndModes(lineWidth, osg::StateAttribute::ON);
    }
}

void Model::SceneModel::RedrawSceneByLOD(const std::shared_ptr<Model::MemoryModel>& aMemoryModel, const uint8_t& aLevel)
{
    if(mLevel == aLevel)
    {
        return;
    }
    mLevel = aLevel;

    RedrawRoadMarks(aLevel);

    for(auto& node : mLineNodeMap)
    {
        osg::Geode* geode = dynamic_cast<osg::Geode*>((node.second).get());
        geode->removeDrawables(0, geode->getNumDrawables());

        std::uint64_t lineId = node.first;
        std::shared_ptr<Model::Line> line = aMemoryModel->GetLineById(lineId);
        Model::PaintListPtr pointListPtr = line->GetPaintListByLevel(aLevel);
        if(pointListPtr->size() == 0)
        {
            return;
        }
        for (Point3DListPtr& points : *pointListPtr)
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
        geode->dirtyBound();
    }
    for(auto& node : mLaneNodeMap)
    {
        osg::ref_ptr<osg::Geode> geode = dynamic_cast<osg::Geode*>((node.second).get());
        geode->removeDrawables(0, geode->getNumDrawables());

        std::uint64_t laneId = node.first;
        std::shared_ptr<Model::Lane> lane = aMemoryModel->GetLaneById(laneId);
        updateLaneNode(geode, lane, aLevel);
    }
}
