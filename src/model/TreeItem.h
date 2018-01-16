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

#include "model/MemoryModel.h"

namespace Model
{

static const std::map<CurveType, QString> scLineTypeMap =
{
    {CurveType::Dashed, "Dashed"},
    {CurveType::Solid, "Solid"},
    {CurveType::Imputed, "Imputed"},
    {CurveType::SlamTrace, "SlamTrace"}
};

bool IsValidLineType(const QString& aType, CurveType& aConvertedType);

namespace ItemName
{

static const QString scSegment = "Segment";
static const QString scRoad = "Road";
static const QString scLane = "Lane";
static const QString scPreLaneId = "PreLaneID";
static const QString scSuccLaneId = "SuccLaneID";
static const QString scLeftLaneId = "LeftLaneID";
static const QString scRightLaneId = "RightLaneID";
static const QString scLeftLine = "LeftLine";
static const QString scRightLine = "RightLine";
static const QString scId = "ID";
static const QString scType = "Type";
static const QString scTrafficSign = "TrafficSign";
static const QString scOrientation = "Orientation";
static const QString scShapeWidth = "ShapeWidth";
static const QString scShapeHeight = "ShapeHeight";
static const QString scConfidence = "Confidence";

}

class TreeItem
{

public:
    explicit TreeItem(const QVector<QVariant>& aData, TreeItem* aParent = 0);
    ~TreeItem();

    void AppendChild(TreeItem* aChild);
    TreeItem* Child(int aIndex);
    int ChildCount() const;
    int ColumnCount() const;
    QVariant Data(int aColumn) const;
    TreeItem* Parent();
    int ChildIndex() const;
    bool SetData(int aColumn, const QVariant& aValue, const std::shared_ptr<Model::MemoryModel>& aMemoryModel);

private:
    bool changeLineInfo(const std::shared_ptr<Model::MemoryModel>& aMemoryModel, const QVariant& aValue);
    bool changeTrafficSignInfo(const std::shared_ptr<Model::MemoryModel>& aMemoryModel, const QVariant& aValue);
    bool changeLaneInfo(const std::shared_ptr<Model::MemoryModel>& aMemoryModel, const QVariant& aValue);
    void changeTrafficSignId(const std::shared_ptr<Model::MemoryModel>& aMemoryModel,
                            const std::uint64_t& aOldId, const std::uint64_t& aNewId);
    void changeLineId(const std::shared_ptr<Model::MemoryModel>& aMemoryModel,
                            const std::uint64_t& aOldId, const std::uint64_t& aNewId);
    void changeLaneId(const std::shared_ptr<Model::MemoryModel>& aMemoryModel,
                            const std::uint64_t& aOldId, const std::uint64_t& aNewId);

private:
    QList<TreeItem*> mChildList;
    QVector<QVariant> mData;
    TreeItem* mParent;
};

}
