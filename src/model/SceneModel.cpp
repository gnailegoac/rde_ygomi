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
    if(aRoad->GetRoadId()!=146159476544634888)
    {
        //return;
    }
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

//    osg::Point* point=new osg::Point;
//    point->setSize(10);
//    geode->getOrCreateStateSet()->setAttributeAndModes(point, osg::StateAttribute::ON);

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

void Model::SceneModel::AddRoadModelToScene(const std::shared_ptr<Model::Road> &aRoad)
{
    if(aRoad->GetRoadId()!=146157664068435975)
    {
        //return;
    }

    osg::ref_ptr<osg::Group> roadGroup = buildRoadModelNode(aRoad);
    mSceneModelRoot->addChild(roadGroup);
    mRoadModelNodeMap[aRoad->GetRoadId()] = roadGroup;

    //osg::ref_ptr<osg::Group> roadMarksGroup = buildRoadMarksModelNode(aRoad);
    //mSceneModelRoot->addChild(roadMarksGroup);
    //mRoadModelNodeMap[aRoad->GetRoadId()] = roadGroup;
}

void Model::SceneModel::RemoveRoadModelFromScene()
{
    std::map<std::uint64_t, osg::ref_ptr<osg::Node>>::iterator iter;
    for(iter = mRoadModelNodeMap.begin(); iter != mRoadModelNodeMap.end(); iter++)
    {
        mSceneModelRoot->removeChild((iter->second).release());
    }
    mRoadNodeMap.clear();
}

osg::ref_ptr<osg::Group> Model::SceneModel::buildRoadModelNode(const std::shared_ptr<Model::Road> &aRoad)
{
    osg::ref_ptr<osg::Group> roadNode(new osg::Group);
    roadNode->setName("RoadModel:" + std::to_string(aRoad->GetRoadId()));
    Model::LaneListPtr laneListptr = aRoad->GetLaneList();
    for(const auto& lane : *laneListptr.get())
    {
        if(lane->GetLaneId()!=2)
        {
            //continue;
        }

        Model::LinePtr rightLine = lane->GetRightLine();
        Model::LinePtr leftLine = lane->GetLeftLine();

        Model::PaintListPtr rightLinePointListPtr = rightLine->GetPaintListByLevel(2);
        Model::PaintListPtr leftLinePointListPtr = leftLine->GetPaintListByLevel(2);
        if(rightLinePointListPtr->size() == 0||leftLinePointListPtr->size() == 0)
        {
            continue;
        }

        osg::ref_ptr<osg::Vec3Array> vertexArray = new osg::Vec3Array;
        osg::ref_ptr<osg::Vec3Array> normalArray = new osg::Vec3Array;
        osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
        osg::ref_ptr<osg::Geode> geode = new osg::Geode;
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
                if(vertexArray->size()>0&&distance((*vertexArray)[vertexArray->size()-1],vec)<0.1)
                {
                    continue;
                }
                vertexArray->push_back(vec);
                textureCoords->push_back(osg::Vec2(1,1));
            }
        }
        int rightLinePointsNum=vertexArray->size();
        for (Point3DListPtr& points : *leftLinePointListPtr)
        {

            if (0 == points->size())
            {
                continue;
            }
            for (auto& point : *points)
            {
                osg::Vec3d vec(osg::Vec3d(point->GetX(), point->GetY(), point->GetZ()));
                if(vertexArray->size()>0&&distance((*vertexArray)[vertexArray->size()-1],vec)<0.1)
                {
                    continue;
                }
                vertexArray->push_back(vec);
                textureCoords->push_back(osg::Vec2(0,0));
            }
        }

        osg::ref_ptr<osg::DrawElementsUInt> tris = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
        if(!createRoadTriangles(vertexArray,rightLinePointsNum,tris))
        {
            continue;
        }

        normalArray->push_back(osg::Vec3d(0.0,0.0,1.0));
        colorArray->push_back(osg::Vec4d(0.9,0.9,0.9,0.5));
        geometry->setVertexArray(vertexArray);
        geometry->setNormalArray(normalArray);
        geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
        geometry->setColorArray(colorArray);
        geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
        geometry->addPrimitiveSet(tris);
        geometry->setTexCoordArray(0,textureCoords.get());
        std::string path="/home/test/work/road_database_explorer/src/resource/RoadSurface.png";//LaneSection//Lane
        createRoadTexture(path,geometry);
        geode->addDrawable(geometry.get());

        //osg::ref_ptr<osg::PolygonMode> polygonMode = new osg::PolygonMode();
        //polygonMode->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
        //geode->getOrCreateStateSet()->setAttribute( polygonMode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
        roadNode->addChild(geode.get());
    }
    return roadNode;
}

double Model::SceneModel::distance(osg::Vec3 aP1, osg::Vec3 aP2)
{
    double x=aP1[0]-aP2[0];
    double y=aP1[1]-aP2[1];
    double z=aP1[2]-aP2[2];
    return sqrt(x*x+y*y+z*z);
}

bool Model::SceneModel::createRoadTriangles_(const osg::ref_ptr<osg::Vec3Array> &aVertexArray,
                                              int aRightLinePointsNum, osg::ref_ptr<osg::DrawElementsUInt> &aTris)
{
    int num=aVertexArray->size();
    if(aRightLinePointsNum==1||num==aRightLinePointsNum+1)
    {
        return false;
    }
    int j=0;
    int i=j+1;
    int s=aRightLinePointsNum;
    int t=s+1;
    float ld=distance((*aVertexArray)[s],(*aVertexArray)[t]);

    for(i;i<aRightLinePointsNum;++i)
    {
        float rd=distance((*aVertexArray)[j],(*aVertexArray)[i]);
        if(rd<ld)
        {
            aTris->push_back(i-1);
            aTris->push_back(i);
            aTris->push_back(s);
            continue;
        }
        else
        {
            j=i-1;
            for(t;t<num;++t)
            {
               ld=distance((*aVertexArray)[s],(*aVertexArray)[t]);
               if(rd<ld)
               {
                   ld=distance((*aVertexArray)[t-1],(*aVertexArray)[t]);
                   s=t-1;
                   j++;
                   aTris->push_back(i-1);
                   aTris->push_back(i);
                   aTris->push_back(t-1);
                   break;
               }
               aTris->push_back(j);
               aTris->push_back(t);
               aTris->push_back(t-1);
            }
        }
    }
    for(s;s<num;++s)
    {
        aTris->push_back(i-1);
        aTris->push_back(s);
        aTris->push_back(s-1);
    }
    return true;
}

bool Model::SceneModel::createRoadTriangles(const osg::ref_ptr<osg::Vec3Array> &aVertexArray,
                                              int aRightLinePointsNum, osg::ref_ptr<osg::DrawElementsUInt> &aTris)
{
    int num=aVertexArray->size();
    if(num<3)
    {
        return false;
    }
    int i=0;
    int s=aRightLinePointsNum+1;
    double preD=0.0;
    double currD=0.0;
    for(i;i<aRightLinePointsNum;++i)
    {
        preD=distance((*aVertexArray)[i],(*aVertexArray)[s-1]);
        for(s;s<num;++s)
        {
            currD=distance((*aVertexArray)[i],(*aVertexArray)[s]);
            if(preD>currD)
            {
                aTris->push_back(s-1);
                aTris->push_back(i);
                aTris->push_back(s);
                preD=currD;
            }
            else
            {
                break;
            }
        }
        if(i+1<aRightLinePointsNum)
        {
            aTris->push_back(i);
            aTris->push_back(i+1);
            aTris->push_back(s-1);
        }
    }
    return true;
}

void Model::SceneModel::createRoadTexture(const std::string aRoadTextureFile, osg::ref_ptr<osg::Geometry> &aRoadGeometry)
{
    // create a simple material
    osg::ref_ptr<osg::Material> roadMaterial = new osg::Material();
    roadMaterial->setEmission(osg::Material::FRONT, osg::Vec4(1.0, 1.0, 1.0, 1.0));
    roadMaterial->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
    roadMaterial->setTransparency(osg::Material::FRONT_AND_BACK,0.1);
    // load image for texture
    osg::Image *roadImage = osgDB::readImageFile(aRoadTextureFile);
    if (!roadImage)
    {
        roadImage = osgDB::readImageFile("../Resources/roadTexture.jpg");

        osg::notify(osg::WARN) << "Couldn't load texture."  << std::endl;

        if (!roadImage)
        {
            return;
        }
    }

     // create a texture
    osg::ref_ptr<osg::Texture2D> roadTexture = new osg::Texture2D;
    roadTexture->setDataVariance(osg::Object::DYNAMIC);
    roadTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    roadTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    roadTexture->setImage(roadImage);

    // assign the material and texture
    osg::StateSet* roadStateSet=aRoadGeometry->getOrCreateStateSet();
    roadStateSet->setAttribute(roadMaterial);
    roadStateSet->setTextureAttributeAndModes(0, roadTexture, osg::StateAttribute::ON);
    roadStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);
    roadStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
    blendFunc->setFunction(GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA );
    roadStateSet->setAttributeAndModes(blendFunc);
}

osg::ref_ptr<osg::Group> Model::SceneModel::buildRoadMarksModelNode(const std::shared_ptr<Model::Road> &aRoad)
{
    osg::ref_ptr<osg::Group> roadMarksNode(new osg::Group);
    roadMarksNode->setName("RoadMarksModel:" + std::to_string(aRoad->GetRoadId()));
    Model::LaneListPtr laneListptr = aRoad->GetLaneList();
    for(const auto& lane : *laneListptr.get())
    {
        Model::LinePtr rightLine = lane->GetRightLine();
        Model::LinePtr leftLine = lane->GetLeftLine();

        osg::Group* masksNode = nullptr;

        if(mMarksNodeMap.find(leftLine->GetLineId()) == mMarksNodeMap.end())
        {
            if(masksNode == nullptr)
            {
                masksNode = new osg::Group;
            }

            osg::ref_ptr<osg::Node> leftLineMasksNode = createRoadMarksNode(leftLine);
            masksNode->addChild(leftLineMasksNode);
            mMarksNodeMap[leftLine->GetLineId()] = leftLineMasksNode;
        }

        if(mMarksNodeMap.find(rightLine->GetLineId()) == mMarksNodeMap.end())
        {
            if(masksNode == nullptr)
            {
                masksNode = new osg::Group;
            }

            osg::ref_ptr<osg::Node> rightLineMasksNode = createRoadMarksNode(rightLine);
            masksNode->addChild(rightLineMasksNode);
            mMarksNodeMap[rightLine->GetLineId()] = rightLineMasksNode;
        }

        if(masksNode != nullptr)
        {
            roadMarksNode->addChild(masksNode);
        }
    }
    return roadMarksNode;
}


osg::ref_ptr<osg::Node> Model::SceneModel::createRoadMarksNode(const Model::LinePtr &aLine)
{
    osg::ref_ptr<osg::Vec3Array> vertexArray = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec3Array> normalArray = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
    osg::ref_ptr<osg::Vec2Array> textureCoords = new osg::Vec2Array;
    osg::ref_ptr<osg::DrawElementsUInt> tris = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);

    Model::PaintListPtr viewPointListPtr = aLine->GetPaintListByLevel(2);
    if(viewPointListPtr->size() == 0)
    {
        return nullptr;
    }
    for (Point3DListPtr& points : *viewPointListPtr)
    {
        if (0 == points->size())
        {
            continue;
        }
        for (auto& point : *points)
        {
            osg::Vec3 vec=osg::Vec3(point->GetX(), point->GetY(), point->GetZ());
            createMarksTriangles(vec,osg::Vec3(0.0,0.0,1.0),vertexArray,tris);
        }
    }
    normalArray->push_back(osg::Vec3d(0.0,0.0,1.0));
    colorArray->push_back(osg::Vec4d(1.0,1.0,0.0,1.0));
    geometry->setVertexArray(vertexArray);
    geometry->setNormalArray(normalArray);
    geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
    geometry->setColorArray(colorArray);
    geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
    geometry->addPrimitiveSet(tris);

    geode->addDrawable(geometry.get());
    return geode;
}

void Model::SceneModel::createMarksTriangles(const osg::Vec3 &aCenter, const osg::Vec3 &aNormal,
                                             osg::ref_ptr<osg::Vec3Array> &aVertexArray,
                                             osg::ref_ptr<osg::DrawElementsUInt> &aTris)
{
    osg::Vec3 a;
    osg::Vec3 b;
    a = cross(aNormal, osg::Vec3(1.0, 0.0, 0.0));
    if (!any(a))
    {
        a = cross(aNormal, osg::Vec3(0.0, 1.0, 0.0));
    }
    b = cross(aNormal, a);
    if (!normalize(a))
        return;
    if (!normalize(b))
        return;

    const int edgeNum=12;
    float radius=1;
    double deltaAngle = 2 * 3.1415926535 / edgeNum;
    float COSINE[edgeNum];
    float SINE[edgeNum];
    for (int i = 0; i < edgeNum; ++i)
    {
        COSINE[i] = cos(i*deltaAngle);
        SINE[i] = sin(i*deltaAngle);
    }
    for (int i = 0; i < edgeNum; ++i)
    {
        float cs = COSINE[i];
        float sn = SINE[i];
        float x = aCenter[0] + radius*a[0] * cs + radius*b[0] * sn;
        float y = aCenter[1] + radius*a[1] * cs + radius*b[1] * sn;
        float z = aCenter[2] + radius*a[2] * cs + radius*b[2] * sn+0.1;
        aVertexArray->push_back(osg::Vec3(x,y,z));
    }
    int num=aVertexArray->size()-edgeNum;
    for(int i=1; i<edgeNum-1; ++i)
    {

        aTris->push_back(num);
        aTris->push_back(num+i);
        aTris->push_back(num+i+1);
    }
}

osg::Vec3 Model::SceneModel::cross(osg::Vec3 aV1, osg::Vec3 aV2)
{
    osg::Vec3 v;
    v[0] = aV1[1] * aV2[2] - aV1[2] * aV2[1];
    v[1] = aV1[2] * aV2[0] - aV1[0] * aV2[2];
    v[2] = aV1[0] * aV2[1] - aV1[1] * aV2[0];
    return v;
}

bool Model::SceneModel::any(osg::Vec3 aVec3)
{
    double eps = 1e-6;
    if (abs(aVec3[0]) > eps || abs(aVec3[1]) > eps || abs(aVec3[2]) > eps)
        return true;
    return false;
}

bool Model::SceneModel::normalize(osg::Vec3 &aVec3)
{
    if (!any(aVec3))
    {
        return false;
    }
    double sum = 0.0;
    for (int i = 0; i < 3; i++)
    {
        sum += aVec3[i] * aVec3[i];
    }
    sum = sqrt(sum);
    for (int i = 0; i < 3; i++)
    {
        aVec3[i] /= sum;
    }
    return true;
}

osgText::Text* Model::SceneModel::createContent(osg::Vec3 aPosition, int aNum)
{
    wchar_t* wText = new wchar_t[10];
    char teethNum[10];
    sprintf(teethNum, "%d", aNum);
    mbstowcs(wText, teethNum, 10);

    osg::ref_ptr<osgText::Text> txtContent = new osgText::Text;
    txtContent->setText(wText);
    txtContent->setPosition(aPosition);
    txtContent->setColor(osg::Vec4(1,0,0,1));
    txtContent->setCharacterSize(2);

    txtContent->setAlignment(osgText::Text::CENTER_BOTTOM);
    txtContent->setAutoRotateToScreen(true);
    txtContent->setName("TxtContent");
    txtContent->setFontResolution(50, 50);

    delete[] wText;
    return txtContent.release();
}

void Model::SceneModel::RedrawRoadMarks(const double &aDistance)
{
    double width=0.0;
    double s=20100;
    double t=21000;
    double widthMin=1.0;
    double widthMax=10.0;
    if(aDistance<=s)
    {
        width=widthMin;
    }
    else if(aDistance>t)
    {
        width=widthMax;
    }
    else
    {
        width=(aDistance-s)/(t-s)*widthMax;
    }

    osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth;
    lineWidth->setWidth(width);
    std::map<std::uint64_t, osg::ref_ptr<osg::Node>>::iterator iter;
    for(iter = mLineNodeMap.begin(); iter != mLineNodeMap.end(); iter++)
    {
        osg::Geode* geode=dynamic_cast<osg::Geode*>((iter->second).get());
        geode->getOrCreateStateSet()->setAttributeAndModes(lineWidth, osg::StateAttribute::ON);
    }
    //qDebug("distance=%.12f",aDistance);
    //qDebug("width=%.12f",width);
}
