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
        return QVariant();

    if (aRole != Qt::DisplayRole/* && aRole != Qt::EditRole*/)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(aIndex.internalPointer());

    return item->Data(aIndex.column());
}

Qt::ItemFlags Model::TreeModel::flags(const QModelIndex& aIndex) const
{
    if (!aIndex.isValid())
        return 0;

//    return Qt::ItemIsEditable | QAbstractItemModel::flags(aIndex);
    return QAbstractItemModel::flags(aIndex);
}

QVariant Model::TreeModel::headerData(int aSection, Qt::Orientation aOrientation,
                               int aRole) const
{
    if (aOrientation == Qt::Horizontal && aRole == Qt::DisplayRole)
        return mRoot->Data(aSection);

    return QVariant();
}

QModelIndex Model::TreeModel::index(int aRow, int aColumn, const QModelIndex& aParent) const
{
    if (!hasIndex(aRow, aColumn, aParent))
        return QModelIndex();

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

void Model::TreeModel::setupModelData(const std::shared_ptr<Model::MemoryModel>& aRoadModel, Model::TreeItem *aParent)
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
        QVector<QVariant> columnData;
//        qDebug() << QString("SegID(%1)").arg(tile.first);
        columnData << qlonglong(tile.first);
//        columnData << QString("SegID(%1)").arg(tile.first);
//        columnData << "SegID(12344)" << qlonglong(tile.first);
        const RoadMapPtr& roadMap = tile.second->GetRoadMap();
        for (const auto& road : *roadMap)
        {

        }
        parents.last()->AppendChild(new Model::TreeItem(columnData, parents.last()));
    }
//    while (number < lines.count()) {

//        QString lineData = lines[number].mid(position).trimmed();

//        if (!lineData.isEmpty()) {
//            // Read the column data from the rest of the line.
//            QList<QVariant> columnData;
//            for (int column = 0; column < columnStrings.count(); ++column)
//                columnData << columnStrings[column];

//            if (position > indentations.last()) {
//                // The last child of the current parent is now the new parent
//                // unless the current parent has no children.

//                if (parents.last()->childCount() > 0) {
//                    parents << parents.last()->child(parents.last()->childCount()-1);
//                    indentations << position;
//                }
//            } else {
//                while (position < indentations.last() && parents.count() > 0) {
//                    parents.pop_back();
//                    indentations.pop_back();
//                }
//            }

//            // Append a new item to the current parent's list of children.
//            parents.last()->appendChild(new TreeItem(columnData, parents.last()));
//        }

//    }
}
