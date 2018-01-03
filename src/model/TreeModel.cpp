/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    TreeModel.cpp
 * @brief   A container for items of data supplied by Road Memory Model
 *******************************************************************************
 */

#include "TreeItem.h"
#include "TreeModel.h"

#include <QtWidgets>
#include <memory>

#include "model/MemoryModel.h"
#include "model/Road.h"
#include "model/Tile.h"

Model::TreeModel::TreeModel(const std::shared_ptr<Model::MemoryModel>& aRoadModel, QObject* aParent)
    : QAbstractItemModel(aParent)
{
    QVector<QVariant> rootData;
    rootData << "Feature" << "Value";

    mRoot = new TreeItem(rootData);
    setupModelData(aRoadModel, mRoot);
}

Model::TreeModel::~TreeModel()
{
    delete mRoot;
}

int Model::TreeModel::columnCount(const QModelIndex& aParent) const
{
    if (aParent.isValid())
    {
        return static_cast<TreeItem*>(aParent.internalPointer())->ColumnCount();
    }
    else
    {
        return mRoot->ColumnCount();
    }
}

QVariant Model::TreeModel::data(const QModelIndex& aIndex, int aRole) const
{
    if (!aIndex.isValid())
    {
        return QVariant();
    }

    if (aRole != Qt::DisplayRole && aRole != Qt::EditRole)
    {
        return QVariant();
    }

    TreeItem *item = static_cast<TreeItem*>(aIndex.internalPointer());
    return item->Data(aIndex.column());
}

Qt::ItemFlags Model::TreeModel::flags(const QModelIndex& aIndex) const
{
    if (!aIndex.isValid())
    {
        return 0;
    }

    return QAbstractItemModel::flags(aIndex);
}

QVariant Model::TreeModel::headerData(int aSection, Qt::Orientation aOrientation,
                               int aRole) const
{
    if (aOrientation == Qt::Horizontal && aRole == Qt::DisplayRole)
    {
        return mRoot->Data(aSection);
    }

    return QVariant();
}

QModelIndex Model::TreeModel::index(int aRow, int aColumn, const QModelIndex& aParent) const
{
    if (!hasIndex(aRow, aColumn, aParent))
    {
        return QModelIndex();
    }

    TreeItem *parentItem;

    if (!aParent.isValid())
    {
        parentItem = mRoot;
    }
    else
    {
        parentItem = static_cast<TreeItem*>(aParent.internalPointer());
    }

    TreeItem *childItem = parentItem->Child(aRow);
    if (childItem)
    {
        return createIndex(aRow, aColumn, childItem);
    }
    else
    {
        return QModelIndex();
    }
}

QModelIndex Model::TreeModel::parent(const QModelIndex &aIndex) const
{
    if (!aIndex.isValid())
    {
        return QModelIndex();
    }

    TreeItem *childItem = static_cast<TreeItem*>(aIndex.internalPointer());
    TreeItem *parentItem = childItem->Parent();

    if (parentItem == mRoot)
    {
        return QModelIndex();
    }

    return createIndex(parentItem->ChildIndex(), 0, parentItem);
}

int Model::TreeModel::rowCount(const QModelIndex& aParent) const
{
    Model::TreeItem *parentItem;
    if (aParent.column() > 0)
    {
        return 0;
    }

    if (!aParent.isValid())
    {
        parentItem = mRoot;
    }
    else
    {
        parentItem = static_cast<Model::TreeItem*>(aParent.internalPointer());
    }

    return parentItem->ChildCount();
}

void Model::TreeModel::setupModelData(const std::shared_ptr<Model::MemoryModel>& aRoadModel, Model::TreeItem* aParent)
{
    if (!aRoadModel)
    {
        return;
    }

    QList<Model::TreeItem*> parents;
    parents << aParent;

    const Model::TileMapPtr& tileMap = aRoadModel->GetTileMap();
    for (const auto& tile : *tileMap)
    {
        createSegmentNode(tile.second, aParent);
    }
}

void Model::TreeModel::createSegmentNode(const std::shared_ptr<Model::Tile>& aTile, Model::TreeItem* aParent)
{
    QVector<QVariant> columnData;
    columnData << tr("Segment") << qlonglong(aTile->GetTileId());
    Model::TreeItem* tileNode = new Model::TreeItem(columnData, aParent);
    aParent->AppendChild(tileNode);
    const Model::RoadMapPtr& roadMap = aTile->GetRoadMap();
    for (const auto& road : *roadMap)
    {
        createRoadNode(road.second, tileNode);
    }
}

void Model::TreeModel::createRoadNode(const std::shared_ptr<Model::Road>& aRoad, Model::TreeItem* aParent)
{
    QVector<QVariant> columnData;
    columnData << tr("Road") << qlonglong(aRoad->GetRoadId());
    Model::TreeItem* roadNode = new Model::TreeItem(columnData, aParent);
    aParent->AppendChild(roadNode);
    const Model::LaneListPtr& laneListPtr = aRoad->GetLaneList();
    for (const auto& lane : *laneListPtr)
    {
        createLaneNode(lane, roadNode);
    }
}

void Model::TreeModel::createLaneNode(const std::shared_ptr<Model::Lane>& aLane, Model::TreeItem* aParent)
{
    QVector<QVariant> columnData;
    columnData << tr("Lane") << qlonglong(aLane->GetLaneId());
    Model::TreeItem* laneNode = new Model::TreeItem(columnData, aParent);
    aParent->AppendChild(laneNode);
    createPredecessorLaneNode(aLane->GetPredecessorLaneId(), laneNode);
    createSuccessorLaneNode(aLane->GetSuccessorLaneId(), laneNode);
    createLeftLaneNode(aLane->GetLeftLaneId(), laneNode);
    createRightLaneNode(aLane->GetRightLaneId(), laneNode);

    const std::shared_ptr<Line>& leftLinePtr = aLane->GetLeftLine();
    const std::shared_ptr<Line>& rightLinePtr = aLane->GetRightLine();
    createLeftLineNode(leftLinePtr, laneNode);
    createRightLineNode(rightLinePtr, laneNode);
}

void Model::TreeModel::createPredecessorLaneNode(const uint64_t& aLaneId, Model::TreeItem* aParent)
{
    QVector<QVariant> columnData;
    columnData << tr("PreLaneID");
    if (aLaneId > 0)
    {
        columnData << qlonglong(aLaneId);
    }
    else
    {
        columnData << "";
    }
    Model::TreeItem* preLaneNode = new Model::TreeItem(columnData, aParent);
    aParent->AppendChild(preLaneNode);
}

void Model::TreeModel::createSuccessorLaneNode(const uint64_t& aLaneId, Model::TreeItem* aParent)
{
    QVector<QVariant> columnData;
    columnData << tr("SuccLaneID");
    if (aLaneId > 0)
    {
        columnData << qlonglong(aLaneId);
    }
    else
    {
        columnData << "";
    }
    Model::TreeItem* succLaneNode = new Model::TreeItem(columnData, aParent);
    aParent->AppendChild(succLaneNode);
}

void Model::TreeModel::createLeftLaneNode(const uint64_t& aLaneId, Model::TreeItem* aParent)
{
    QVector<QVariant> columnData;
    columnData << tr("LeftLaneID");
    if (aLaneId > 0)
    {
        columnData << qlonglong(aLaneId);
    }
    else
    {
        columnData << "";
    }
    Model::TreeItem* leftLaneNode = new Model::TreeItem(columnData, aParent);
    aParent->AppendChild(leftLaneNode);
}

void Model::TreeModel::createRightLaneNode(const uint64_t& aLaneId, Model::TreeItem* aParent)
{
    QVector<QVariant> columnData;
    columnData << tr("RightLaneID");
    if (aLaneId > 0)
    {
        columnData << qlonglong(aLaneId);
    }
    else
    {
        columnData << "";
    }
    Model::TreeItem* rightLaneNode = new Model::TreeItem(columnData, aParent);
    aParent->AppendChild(rightLaneNode);
}

void Model::TreeModel::createLeftLineNode(const std::shared_ptr<Model::Line>& aLine, Model::TreeItem* aParent)
{
    QVector<QVariant> columnData;
    columnData << tr("LeftLine") << tr("");
    Model::TreeItem* lineNode = new Model::TreeItem(columnData, aParent);
    aParent->AppendChild(lineNode);
    createLineIdNode(aLine, lineNode);
}

void Model::TreeModel::createRightLineNode(const std::shared_ptr<Model::Line>& aLine, Model::TreeItem* aParent)
{
    QVector<QVariant> columnData;
    columnData << tr("RightLine") << tr("");
    Model::TreeItem* lineNode = new Model::TreeItem(columnData, aParent);
    aParent->AppendChild(lineNode);
    createLineIdNode(aLine, lineNode);
}

void Model::TreeModel::createLineIdNode(const std::shared_ptr<Model::Line>& aLine, Model::TreeItem* aParent)
{
    QVector<QVariant> columnData;
    columnData << tr("ID") << qlonglong(aLine->GetLineId());
    Model::TreeItem* lineIdNode = new Model::TreeItem(columnData, aParent);
    aParent->AppendChild(lineIdNode);
}
