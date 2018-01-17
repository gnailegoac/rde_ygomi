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

#include "model/LookUpTable.h"

bool Model::IsValidLineType(const QString& aType, Model::CurveType& aConvertedType)
{
    for (const auto& it : Model::scLineTypeMap)
    {
        if (aType == it.second)
        {
            aConvertedType = it.first;
            return true;
        }
    }
    return false;
}

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
    {
        return mParent->mChildList.indexOf(const_cast<TreeItem*>(this));
    }

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

bool Model::TreeItem::SetData(int aColumn, const QVariant& aValue,
                              const std::shared_ptr<Model::MemoryModel>& aMemoryModel)
{
    // only column 1 can be modified.
    if (aColumn != 1)
    {
        return false;
    }

    bool dataChanged = false;

    if (aValue != mData[1])
    {
        dataChanged = changeTrafficSignInfo(aMemoryModel, aValue)
                      | changeLineInfo(aMemoryModel, aValue)
                      | changeLaneInfo(aMemoryModel, aValue);

        if (dataChanged)
        {
            mData[aColumn] = aValue;
        }
    }

    return dataChanged;
}

bool Model::TreeItem::changeLineInfo(const std::shared_ptr<Model::MemoryModel>& aMemoryModel,
                                     const QVariant& aValue)
{
    bool dataChanged = false;
    if (mParent
        && (ItemName::scLeftLine == mParent->Data(0).toString()
            || ItemName::scRightLine == mParent->Data(0)))
    {
        std::uint64_t lineId = mParent->Child(0)->Data(1).toULongLong();
        if (ItemName::scType == mData[0].toString())
        {
            // To change line type.
            CurveType curveType;
            if (IsValidLineType(aValue.toString(), curveType))
            {
                LinePtr linePtr = aMemoryModel->GetLineById(lineId);
                if (linePtr)
                {
                    for (auto& curve: *(linePtr->GetCurveList()))
                    {
                        curve->SetCurveType(curveType);
                    }
                    dataChanged = true;
                }
            }
        }
        else if (ItemName::scId == mData[0].toString())
        {
            // To change line id.
            if (aValue.canConvert(QMetaType::ULongLong))
            {
                std::uint64_t newLineId = aValue.toULongLong();
                if (lineId != newLineId)
                {
                    dataChanged = changeLineId(aMemoryModel, lineId, newLineId);
                }
            }
        }
    }
    return dataChanged;
}

bool Model::TreeItem::changeTrafficSignInfo(const std::shared_ptr<Model::MemoryModel>& aMemoryModel,
                                            const QVariant& aValue)
{
    bool dataChanged = false;

    if (mParent && ItemName::scTrafficSign == mParent->Data(0).toString())
    {
        std::uint64_t trafficSignId = mParent->Data(1).toULongLong();
        TrafficSignPtr trafficSignPtr = aMemoryModel->GetTrafficSignById(trafficSignId);
        // Actually, the trafficSignPtr should always be valid.
        if (trafficSignPtr)
        {
            if (ItemName::scType == mData[0].toString()
                && aValue.canConvert(QMetaType::ULongLong)
                && LookUpTable::mTrafficSignImageMap.count(aValue.toULongLong()))
            {
                // To change traffic sign type.
                // Notify 3D viewer the type change and repaint
                trafficSignPtr->SetTrafficSignType(aValue.toULongLong());
                dataChanged = true;
            }
            else if (ItemName::scOrientation == mData[0].toString())
            {
                // To change traffic sign orientation.
                if (aValue.canConvert(QMetaType::Double))
                {
                    trafficSignPtr->SetOrientation(aValue.toDouble());
                    dataChanged = true;
                }
            }
            else if (ItemName::scShapeWidth == mData[0].toString())
            {
                // To change traffic sign shape width.
                if (aValue.canConvert(QMetaType::Double))
                {
                    trafficSignPtr->SetShapeWidth(aValue.toDouble());
                    dataChanged = true;
                }
            }
            else if (ItemName::scShapeHeight == mData[0].toString())
            {
                // To change traffic sign shape height.
                if (aValue.canConvert(QMetaType::Double))
                {
                    trafficSignPtr->SetShapeHeight(aValue.toDouble());
                    dataChanged = true;
                }
            }
            else if (ItemName::scConfidence == mData[0].toString())
            {
                // To change traffic sign confidence.
                if (aValue.canConvert(QMetaType::Float))
                {
                    trafficSignPtr->SetConfidence(aValue.toFloat());
                    dataChanged = true;
                }
            }
        }
    }

    return dataChanged;
}

bool Model::TreeItem::changeLaneInfo(const std::shared_ptr<Model::MemoryModel>& aMemoryModel,
                                     const QVariant& aValue)
{
    bool dataChanged = false;

    if (ItemName::scLane == mData[0].toString())
    {
        // To change lane Id.
        if (aValue.canConvert(QMetaType::ULongLong))
        {
            std::uint64_t laneId = mData[1].toULongLong();
            std::uint64_t newLaneId = aValue.toULongLong();
            dataChanged = changeLaneId(aMemoryModel, laneId, newLaneId);
        }
    }
    else if (mParent && ItemName::scLane == mParent->Data(0).toString())
    {
        std::uint64_t laneId = mParent->Data(1).toULongLong();
        LanePtr lanePtr = aMemoryModel->GetLaneById(laneId);
        if (lanePtr && aValue.canConvert(QMetaType::ULongLong))
        {
            std::uint64_t newLaneId = aValue.toULongLong();
            LanePtr newLanePtr = aMemoryModel->GetLaneById(newLaneId);
            if (newLanePtr)
            {
                if (ItemName::scPreLaneId == mData[0].toString())
                {
                    // To change predecessor lane id.
                    lanePtr->SetPredecessorLaneId(newLaneId);
                    newLanePtr->SetSuccessorLaneId(laneId);
                    dataChanged = true;
                }
                else if (ItemName::scSuccLaneId == mData[0].toString())
                {
                    // To change successor lane id.
                    lanePtr->SetSuccessorLaneId(newLaneId);
                    newLanePtr->SetPredecessorLaneId(laneId);
                    dataChanged = true;
                }
                else if (ItemName::scLeftLaneId == mData[0].toString())
                {
                    // To change left lane id.
                    lanePtr->SetLeftLaneId(newLaneId);
                    newLanePtr->SetRightLaneId(laneId);
                    dataChanged = true;
                }
                else if (ItemName::scRightLaneId == mData[0].toString())
                {
                    // To change right lane id.
                    lanePtr->SetRightLaneId(newLaneId);
                    newLanePtr->SetLeftLaneId(laneId);
                    dataChanged = true;
                }
            }
        }
    }

    return dataChanged;
}

bool Model::TreeItem::changeLineId(const std::shared_ptr<Model::MemoryModel>& aMemoryModel,
                                   const uint64_t& aOldId, const uint64_t& aNewId)
{
    // Remove old line from Tile's mLineMap and add new pair.
    bool dataChanged = false;

    LinePtr linePtr = aMemoryModel->GetLineById(aOldId);
    if (linePtr)
    {
        LanePtr lanePtr = linePtr->GetLane();
        if (lanePtr)
        {
            RoadPtr roadPtr = lanePtr->GetRoad();
            if (roadPtr)
            {
                TilePtr tilePtr = roadPtr->GetTile();
                if (tilePtr)
                {
                    LineMapPtr lineMapPtr = tilePtr->GetMutableLineMap();
                    lineMapPtr->erase(aOldId);
                    linePtr->SetLineId(aNewId);
                    lineMapPtr->insert(std::make_pair(aNewId, linePtr));
                    dataChanged = true;
                }
            }
        }
    }

    return dataChanged;
}

bool Model::TreeItem::changeLaneId(const std::shared_ptr<Model::MemoryModel>& aMemoryModel,
                                   const uint64_t& aOldId, const uint64_t& aNewId)
{
    // Remove old lane from Tile's mLaneMap and add new pair.
    bool dataChanged = false;
    LanePtr lanePtr = aMemoryModel->GetLaneById(aOldId);
    if (lanePtr)
    {
        RoadPtr roadPtr = lanePtr->GetRoad();
        if (roadPtr)
        {
            TilePtr tilePtr = roadPtr->GetTile();
            if (tilePtr)
            {
                LaneMapPtr laneMapPtr = tilePtr->GetMutableLaneMap();
                laneMapPtr->erase(aOldId);
                lanePtr->SetLaneId(aNewId);
                laneMapPtr->insert(std::make_pair(aNewId, lanePtr));
                dataChanged = true;
            }
        }
    }
    return dataChanged;
}
