/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    TreeModel.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include <memory>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

namespace Model {

class MemoryModel;
class TreeItem;

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeModel(const std::shared_ptr<Model::MemoryModel>& aRoadModel,
              QObject* aParent = 0);

    ~TreeModel();

    QVariant data(const QModelIndex& aIndex, int aRole) const override;
    Qt::ItemFlags flags(const QModelIndex &aIndex) const override;
    QVariant headerData(int aSection, Qt::Orientation aOrientation,
                        int aRole = Qt::DisplayRole) const override;
    QModelIndex index(int aRow, int aColumn,
                      const QModelIndex& aParent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& aIndex) const override;
    int rowCount(const QModelIndex& aParent = QModelIndex()) const override;
    int columnCount(const QModelIndex& aParent = QModelIndex()) const override;

private:
    void setupModelData(const std::shared_ptr<Model::MemoryModel>& aRoadModel, TreeItem* aParent);

    TreeItem* mRoot;
};

}
