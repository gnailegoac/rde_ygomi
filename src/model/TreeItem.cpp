/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    TreeItem.cpp
 * @brief
 *******************************************************************************
 */

#include "TreeItem.h"

#include <QStringList>

Model::TreeItem::TreeItem(const QVector<QVariant>& aData, TreeItem* aParent)
{
    mParent = aParent;
    mData = aData;
}

Model::TreeItem::~TreeItem()
{
    qDeleteAll(mChildList);
}

void Model::TreeItem::AppendChild(Model::TreeItem* aChild)
{
    mChildList.append(aChild);
}

Model::TreeItem *Model::TreeItem::Child(int aIndex)
{
    return mChildList.value(aIndex);
}

int Model::TreeItem::ChildCount() const
{
    return mChildList.count();
}

int Model::TreeItem::ChildIndex() const
{
    if (mParent)
        return mParent->mChildList.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

int Model::TreeItem::ColumnCount() const
{
    return mData.count();
}

QVariant Model::TreeItem::Data(int aColumn) const
{
    return mData.value(aColumn);
}

Model::TreeItem* Model::TreeItem::Parent()
{
    return mParent;
}

bool Model::TreeItem::SetData(int aColumn, const QVariant& aValue)
{
    if (aColumn < 0 || aColumn >= mData.size())
        return false;

    mData[aColumn] = aValue;
    return true;
}
