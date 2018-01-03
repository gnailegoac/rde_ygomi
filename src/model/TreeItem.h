/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    TreeItem.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include <QList>
#include <QVariant>
#include <QVector>

namespace Model
{

class TreeItem
{
public:
    explicit TreeItem(const QVector<QVariant> &aData, TreeItem* aParent = 0);
    ~TreeItem();

    void AppendChild(TreeItem *aChild);
    TreeItem* Child(int aIndex);
    int ChildCount() const;
    int ColumnCount() const;
    const QVariant& Data(int aColumn) const;
    TreeItem* Parent();
    int ChildIndex() const;
    bool SetData(int aColumn, const QVariant& aValue);

private:
    QList<TreeItem*> mChildList;
    QVector<QVariant> mData;
    TreeItem* mParent;
};

}
