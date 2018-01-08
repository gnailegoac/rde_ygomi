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

#include "model/Curve.h"
namespace Model {

class MemoryModel;
class TreeItem;
class Tile;
class Road;
class Lane;
class Line;
//enum CurveType;

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeModel(const std::shared_ptr<MemoryModel>& aRoadModel,
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
    void setupModelData(const std::shared_ptr<MemoryModel>& aRoadModel, TreeItem* aParent);

    void createSegmentNode(const std::shared_ptr<Tile>& aTile, TreeItem* aParent);
    void createRoadNode(const std::shared_ptr<Road>& aRoad, TreeItem* aParent);
    void createLaneNode(const std::shared_ptr<Lane>& aLane, TreeItem* aParent);
    void createPredecessorLaneNode(const std::uint64_t& aLaneId, TreeItem* aParent);
    void createSuccessorLaneNode(const std::uint64_t& aLaneId, TreeItem* aParent);
    void createLeftLaneNode(const std::uint64_t& aLaneId, TreeItem* aParent);
    void createRightLaneNode(const std::uint64_t& aLaneId, TreeItem* aParent);
    void createLeftLineNode(const std::shared_ptr<Line>& aLine, TreeItem* aParent);
    void createRightLineNode(const std::shared_ptr<Line>& aLine, TreeItem* aParent);
    void createLineIdNode(const std::shared_ptr<Line>& aLine, TreeItem* aParent);
    void createLineTypeNode(const std::shared_ptr<Line>& aLine, TreeItem* aParent);

private:
    TreeItem* mRoot;
};

}
