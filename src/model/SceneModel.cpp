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

osg::ref_ptr<osg::Node> Model::SceneModel::buildLineNode(const Model::LinePtr& aLine) const
{
    Model::ViewPointMapPtr viewPointMapPtr = aLine->GetPointListMap();
    if(viewPointMapPtr->size() == 0)
    {
        return nullptr;
    }

    osg::ref_ptr<osg::Vec3dArray> vertexArray = new osg::Vec3dArray;
    for (auto point : *viewPointMapPtr->begin()->second)
    {
        vertexArray->push_back(osg::Vec3d(point->GetX(), point->GetY(), point->GetZ()));
    }

    osg::Geometry* geometry = new osg::Geometry;
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::DrawArrays::LINE_STRIP, 0, vertexArray->size()));
    geometry->setVertexArray(vertexArray);

    osg::Geode* geode = new osg::Geode;
    geode->addDrawable(geometry);
    return geode;
}

osg::ref_ptr<osg::Group> Model::SceneModel::buildRoadNode(const std::shared_ptr<Model::Road>& aRoad) const
{
    osg::ref_ptr<osg::Group> roadNode(new osg::Group);
    Model::LaneListPtr laneListptr = aRoad->GetLaneList();
    std::set<std::uint64_t> lineIdSet;
    for(const auto& lane : *laneListptr.get())
    {
        Model::LinePtr rightLine = lane->GetRightLine();
        Model::LinePtr leftLine = lane->GetLeftLine();
        osg::Group* laneNode = nullptr;

        if(lineIdSet.find(leftLine->GetLineId()) == lineIdSet.end())
        {
            if(laneNode == nullptr)
            {
                laneNode = new osg::Group;
            }

            laneNode->addChild(buildLineNode(leftLine));
            lineIdSet.insert(leftLine->GetLineId());
        }

        if(lineIdSet.find(rightLine->GetLineId()) == lineIdSet.end())
        {
            if(laneNode == nullptr)
            {
                laneNode = new osg::Group;
            }

            laneNode->addChild(buildLineNode(rightLine));
            lineIdSet.insert(rightLine->GetLineId());
        }

        if(laneNode != nullptr)
        {
            roadNode->addChild(laneNode);
        }
    }

    return roadNode;
}
