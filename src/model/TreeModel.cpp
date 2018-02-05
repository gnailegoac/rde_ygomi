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

#include "model/Road.h"
#include "model/Tile.h"

Model::TreeModel::TreeModel(const std::shared_ptr<Model::MemoryModel>& aRoadModel, QObject* aParent)
    : QAbstractItemModel(aParent)
{
    QVector<QVariant> rootData;
    rootData << "Feature" << "Value";

    mRoot = new TreeItem(rootData);
    mMemoryModel = aRoadModel;
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
        return getItem(aParent)->ColumnCount();
    }
    else
    {
        return mRoot->ColumnCount();
    }
}

bool Model::TreeModel::setData(const QModelIndex& aIndex, const QVariant& aValue, int aRole)
{
    if (aRole != Qt::EditRole)
    {
        return false;
    }

    TreeItem *item = getItem(aIndex);
    bool result = item->SetData(aIndex.column(), aValue, mMemoryModel);

    if (result)
    {
        emit dataChanged(aIndex, aIndex);
    }

    return result;
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

    TreeItem *item = getItem(aIndex);
    return item->Data(aIndex.column());
}

Qt::ItemFlags Model::TreeModel::flags(const QModelIndex& aIndex) const
{
    if (!aIndex.isValid())
    {
        return 0;
    }

    if (aIndex.column() == 1)
    {
        return Qt::ItemIsEditable | QAbstractItemModel::flags(aIndex);
    }
    else
    {
        return QAbstractItemModel::flags(aIndex);
    }
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
        parentItem = getItem(aParent);
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

    TreeItem *childItem = getItem(aIndex);
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
        parentItem = getItem(aParent);
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
    columnData << ItemName::scSegment << qlonglong(aTile->GetTileId());
    Model::TreeItem* tileNode = new Model::TreeItem(columnData, aParent);
    aParent->AppendChild(tileNode);
    const Model::RoadMapPtr& roadMap = aTile->GetRoadMap();
    for(const auto& road : *roadMap)
    {
        createRoadNode(road.second, tileNode);
    }

    const Model::TrafficSignMapPtr& trafficSignMap = aTile->GetTrafficSignMap();
    for(const auto& trafficSign : *trafficSignMap)
    {
        createTrafficSignNode(trafficSign.second, tileNode);
    }
}

void Model::TreeModel::createTrafficSignNode(const std::shared_ptr<Model::TrafficSign>& aTrafficSign,
                                             Model::TreeItem* aParent)
{
    QVector<QVariant> columnData;
    columnData << ItemName::scTrafficSign << qulonglong(aTrafficSign->GetTrafficSignId());
    Model::TreeItem* trafficSignNode = new Model::TreeItem(columnData, aParent);
    aParent->AppendChild(trafficSignNode);
    createUint64Node(ItemName::scType, aTrafficSign->GetTrafficSignType(), trafficSignNode);
    createDoubleNode(ItemName::scOrientation, aTrafficSign->GetOrientation(), trafficSignNode);
    createDoubleNode(ItemName::scShapeWidth, aTrafficSign->GetShapeWidth(), trafficSignNode);
    createDoubleNode(ItemName::scShapeHeight, aTrafficSign->GetShapeHeight(), trafficSignNode);
    createDoubleNode(ItemName::scConfidence, aTrafficSign->GetConfidence(), trafficSignNode);
}

void Model::TreeModel::createUint64Node(const QString& aName, const uint64_t& aValue, Model::TreeItem* aParent)
{
    QVector<QVariant> columnData;
    columnData << aName;
    if (aValue > 0)
    {
        columnData << qulonglong(aValue);
    }
    else
    {
        columnData << "";
    }

    aParent->AppendChild(new Model::TreeItem(columnData, aParent));
}

void Model::TreeModel::createDoubleNode(const QString& aName, const double& aValue, Model::TreeItem* aParent)
{
    QVector<QVariant> columnData;
    columnData << aName;
    columnData << aValue;
    aParent->AppendChild(new Model::TreeItem(columnData, aParent));
}

Model::TreeItem* Model::TreeModel::getItem(const QModelIndex& aIndex) const
{
    if (aIndex.isValid())
    {
        TreeItem *item = static_cast<TreeItem*>(aIndex.internalPointer());
        if (item)
        {
            return item;
        }
    }
    return mRoot;
}

void Model::TreeModel::createRoadNode(const std::shared_ptr<Model::Road>& aRoad, Model::TreeItem* aParent)
{
    QVector<QVariant> columnData;
    columnData << ItemName::scRoad << qulonglong(aRoad->GetRoadId());
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
    columnData << ItemName::scLane << qulonglong(aLane->GetLaneId());
    Model::TreeItem* laneNode = new Model::TreeItem(columnData, aParent);
    aParent->AppendChild(laneNode);
    createUint64Node(ItemName::scPreLaneId, aLane->GetPredecessorLaneId(), laneNode);
    createUint64Node(ItemName::scSuccLaneId, aLane->GetSuccessorLaneId(), laneNode);
    createUint64Node(ItemName::scLeftLaneId, aLane->GetLeftLaneId(), laneNode);
    createUint64Node(ItemName::scRightLaneId, aLane->GetRightLaneId(), laneNode);
    createLineNode(ItemName::scLeftLine, aLane->GetLeftLine(), laneNode);
    createLineNode(ItemName::scRightLine, aLane->GetRightLine(), laneNode);
}

void Model::TreeModel::createLineNode(const QString& aName, const std::shared_ptr<Model::Line>& aLine, Model::TreeItem* aParent)
{
    QVector<QVariant> columnData;
    columnData << aName << "";
    Model::TreeItem* lineNode = new Model::TreeItem(columnData, aParent);
    aParent->AppendChild(lineNode);
    createUint64Node(ItemName::scId, aLine->GetLineId(), lineNode);
    createLineTypeNode(aLine, lineNode);
}

void Model::TreeModel::createLineTypeNode(const std::shared_ptr<Model::Line>& aLine, Model::TreeItem* aParent)
{
    QVector<QVariant> columnData;
    columnData << ItemName::scType << scLineTypeMap.at(aLine->GetLineType());
    aParent->AppendChild(new Model::TreeItem(columnData, aParent));
}

bool Model::TreeModel::isNodeNameMatch(const QString& aName, const QString& aNodeName) const
{
    if(aName == "Line")
    {
        if(aNodeName == ItemName::scLeftLine || aNodeName == ItemName::scRightLine)
        {
            return true;
        }
    }
    else if(aName == aNodeName)
    {
        return true;
    }

    return false;
}

QModelIndex Model::TreeModel::GetItemIndex(
        const QString& aName,
        const uint64_t& aId,
        const QModelIndex& aParentIndex) const
{
    QModelIndex searchedModelIndex;
    for(int rowIndex = 0; rowIndex < rowCount(aParentIndex); ++rowIndex)
    {
        QModelIndex rowNameIndex = index(rowIndex, 0, aParentIndex);
        QModelIndex rowValueIndex = index(rowIndex, 1, aParentIndex);
        QString rowName = rowNameIndex.data().toString();
        uint64_t id;
        if(rowName == ItemName::scLeftLine || rowName == ItemName::scRightLine)
        {
            id = rowNameIndex.child(0, 1).data().toULongLong();
        }
        else
        {
            id = rowValueIndex.data().toULongLong();
        }

        if(isNodeNameMatch(aName, rowName) && id == aId)
        {
            searchedModelIndex = rowNameIndex;
            break;
        }
    }

    return searchedModelIndex;
}

