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

    osg::ref_ptr<osg::Group> roadMarksGroup = buildRoadMarksModelNode(aRoad);
    mSceneModelRoot->addChild(roadMarksGroup);
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
    qDebug()<<"CLOSE_ROAD_RENDERING";
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
                textureCoords->push_back(osg::Vec2(0.66,1));
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
                textureCoords->push_back(osg::Vec2(0.3,0.0));
            }
        }
//        osg::Vec3Array::iterator it=vertexArray->begin();
//        vertexArray->erase(it+243,it+245);
        for(int i=0;i<vertexArray->size();i++)
        {
            geode->addDrawable(createContent((*vertexArray)[i],i+1));
//            if(vertexArray->size()==243||vertexArray->size()==244||vertexArray->size()==242)
//            {
//                qDebug("%.12f // %.12f // %.12f", point->GetX(), point->GetY(), point->GetZ());
//            }
        }


        osg::ref_ptr<osg::DrawElementsUInt> tris = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
        if(!createRoadTriangles_(vertexArray,rightLinePointsNum,tris))
        {
            continue;
        }

        normalArray->push_back(osg::Vec3d(0.0,0.0,1.0));
        colorArray->push_back(osg::Vec4d(1.0,1.0,1.0,1.0));
        geometry->setVertexArray(vertexArray);
        geometry->setNormalArray(normalArray);
        geometry->setNormalBinding(osg::Geometry::BIND_OVERALL);
        geometry->setColorArray(colorArray);
        geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
        geometry->addPrimitiveSet(tris);

        geometry->setTexCoordArray(0,textureCoords.get());

        std::string path="/home/test/work/road_database_explorer/src/resource/Lane.png";//LaneSection//Lane
        createRoadTexture(path,geometry);

        geode->addDrawable(geometry.get());


        osg::ref_ptr<osg::PolygonMode> polygonMode = new osg::PolygonMode();
        polygonMode->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
        //geode->getOrCreateStateSet()->setAttribute( polygonMode.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
        roadNode->addChild(geode.get());
    }
    return roadNode;
}

double Model::SceneModel::distance(osg::Vec3 p1, osg::Vec3 p2)
{
    double x=p1[0]-p2[0];
    double y=p1[1]-p2[1];
    double z=p1[2]-p2[2];
    return sqrt(x*x+y*y+z*z);
}

bool Model::SceneModel::createRoadTriangles(const osg::ref_ptr<osg::Vec3Array> &vertexArray,
                                              int rightLinePointsNum, osg::ref_ptr<osg::DrawElementsUInt> &tris)
{
    int num=vertexArray->size();
    if(rightLinePointsNum==1||num==rightLinePointsNum+1)
    {
        return false;
    }
    int j=0;
    int i=j+1;
    int s=rightLinePointsNum;
    int t=s+1;
    float ld=distance((*vertexArray)[s],(*vertexArray)[t]);

    for(i;i<rightLinePointsNum;++i)
    {
        float rd=distance((*vertexArray)[j],(*vertexArray)[i]);
        if(rd<ld)
        {
            tris->push_back(i-1);
            tris->push_back(i);
            tris->push_back(s);
            continue;
        }
        else
        {
            j=i-1;
            for(t;t<num;++t)
            {
               ld=distance((*vertexArray)[s],(*vertexArray)[t]);
               if(rd<ld)
               {
                   ld=distance((*vertexArray)[t-1],(*vertexArray)[t]);
                   s=t-1;
                   j++;
                   tris->push_back(i-1);
                   tris->push_back(i);
                   tris->push_back(t-1);
                   break;
               }
               tris->push_back(j);
               tris->push_back(t);
               tris->push_back(t-1);
            }
        }
    }
    for(s;s<num;++s)
    {
        tris->push_back(i-1);
        tris->push_back(s);
        tris->push_back(s-1);
    }
//    qDebug()<<"num="<<num;
//    qDebug()<<"i="<<i;
//    qDebug()<<"j="<<j;
//    qDebug()<<"s="<<s;
//    qDebug()<<"t="<<t;
    return true;
}

bool Model::SceneModel::createRoadTriangles_(const osg::ref_ptr<osg::Vec3Array> &vertexArray,
                                              int rightLinePointsNum, osg::ref_ptr<osg::DrawElementsUInt> &tris)
{
    int num=vertexArray->size();
    if(num<3)
    {
        return false;
    }

    int i=0;
    int s=rightLinePointsNum+1;
    double preD=0.0;
    double currD=0.0;
    for(i;i<rightLinePointsNum;++i)
    {
        preD=distance((*vertexArray)[i],(*vertexArray)[s-1]);
        for(s;s<num;++s)
        {
            currD=distance((*vertexArray)[i],(*vertexArray)[s]);
            if(preD>currD)
            {
                tris->push_back(s-1);
                tris->push_back(i);
                tris->push_back(s);
                preD=currD;
            }
            else
            {
                break;
            }
        }
        if(i+1<rightLinePointsNum)
        {
            tris->push_back(i);
            tris->push_back(i+1);
            tris->push_back(s-1);
        }
    }
    return true;
}


void Model::SceneModel::createRoadTexture(const std::string roadTextureFile, osg::ref_ptr<osg::Geometry> &roadGeometry)
{
    // create a simple material
    osg::ref_ptr<osg::Material> roadMaterial = new osg::Material();
    roadMaterial->setEmission(osg::Material::FRONT, osg::Vec4(0.0, 0.0, 0.0, 1.0));
    roadMaterial->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);

    // load image for texture
    osg::Image *roadImage = osgDB::readImageFile(roadTextureFile);
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
    osg::StateSet* roadStateSet=roadGeometry->getOrCreateStateSet();
    roadStateSet->setAttribute(roadMaterial);
    roadStateSet->setTextureAttributeAndModes(0, roadTexture, osg::StateAttribute::ON);
    roadStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON);
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

//    geometry->setTexCoordArray(0,textureCoords.get());
//    std::string path="/home/test/work/road_database_explorer/src/resource/Lane.png";
//    createRoadTexture(path,geometry);

    geode->addDrawable(geometry.get());
    return geode;
}

void Model::SceneModel::createMarksTriangles(const osg::Vec3 &center, const osg::Vec3 &normal,
                                             osg::ref_ptr<osg::Vec3Array> &vertexArray,
                                             osg::ref_ptr<osg::DrawElementsUInt> &tris)
{
    osg::Vec3 a;
    osg::Vec3 b;
    a = cross(normal, osg::Vec3(1.0, 0.0, 0.0));
    if (!any(a))
    {
        a = cross(normal, osg::Vec3(0.0, 1.0, 0.0));
    }
    b = cross(normal, a);
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
        float x = center[0] + radius*a[0] * cs + radius*b[0] * sn;
        float y = center[1] + radius*a[1] * cs + radius*b[1] * sn;
        float z = center[2] + radius*a[2] * cs + radius*b[2] * sn+0.1;
        vertexArray->push_back(osg::Vec3(x,y,z));
    }
    int num=vertexArray->size()-edgeNum;
    for(int i=1; i<edgeNum-1; ++i)
    {

        tris->push_back(num);
        tris->push_back(num+i);
        tris->push_back(num+i+1);
    }
}

osg::Vec3 Model::SceneModel::cross(osg::Vec3 v1, osg::Vec3 v2)
{
    osg::Vec3 v;
    v[0] = v1[1] * v2[2] - v1[2] * v2[1];
    v[1] = v1[2] * v2[0] - v1[0] * v2[2];
    v[2] = v1[0] * v2[1] - v1[1] * v2[0];
    return v;
}

bool Model::SceneModel::any(osg::Vec3 vec3)
{
    double eps = 1e-8;
    if (abs(vec3[0]) > eps || abs(vec3[1]) > eps || abs(vec3[2]) > eps)
        return true;
    return false;
}

bool Model::SceneModel::normalize(osg::Vec3 &vec3)
{
    double eps = 1e-8;
    if (!any(vec3))
    {
        return false;
    }
    double sum = 0.0;
    for (int i = 0; i < 3; i++)
    {
        sum += vec3[i] * vec3[i];
    }
    sum = sqrt(sum);
    for (int i = 0; i < 3; i++)
    {
        vec3[i] /= sum;
    }
    return true;
}

osgText::Text* Model::SceneModel::createContent(osg::Vec3 position, int num)
{
    wchar_t* wText = new wchar_t[10];
    char teethNum[10];
    sprintf(teethNum, "%d", num);
    mbstowcs(wText, teethNum, 10);

    osg::ref_ptr<osgText::Text> txtContent = new osgText::Text;
    txtContent->setText(wText);
    txtContent->setPosition(position);
    txtContent->setColor(osg::Vec4(1,0,0,1));
    txtContent->setCharacterSize(2);

    txtContent->setAlignment(osgText::Text::CENTER_BOTTOM);
    txtContent->setAutoRotateToScreen(true);
    txtContent->setName("TxtContent");
    txtContent->setFontResolution(50, 50);

    delete[] wText;
    return txtContent.release();
}
