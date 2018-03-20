/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file   AlgoPointDataConverter.cpp
 *******************************************************************************
 */

#include <QObject>
#include <QtNumeric>

#include "DbConverter/Curve.h"
#include "DbConverter/LaneConnection.h"

#include "AlgoPointDataConverter.h"

namespace Service
{

AlgoPointDataConverter::AlgoPointDataConverter()
{
}

bool AlgoPointDataConverter::convert(const PointCloudLoader& loader,
                                     PeqiDataDefine::PointData& pointData)
{
    pointData.Clear();
    auto pointIdxs = gisPointIdxListToAlgo(loader.getPointIdxs());
    auto lineIds = gisRoadElementIdListToAlgo(loader.getRoadElementIds());
    auto pointCount = lineIds.size();
    if (pointCount <= 0)
    {
        return false;
    }

    const std::size_t dimension(loader.getDimension());
    auto& points = loader.getCoords();
    if (points.size() != pointCount * dimension)
    {
        return false;
    }

    auto pointClasses =
            gisPointClassToAlgo(loader.getPointClasses());
    if (pointClasses.size() != pointCount)
    {
        return false;
    }

    auto& subTypes = loader.getSubTypes();
    if (subTypes.size() != pointCount)
    {
        return false;
    }

    pointData.mPointsIdx = pointIdxs;
    pointData.mPointCount = pointCount;
    pointData.mDimension = dimension;

    pointData.mPoints = points;
    pointData.mLineIds = lineIds;
    pointData.mTypes = pointClasses;
    pointData.mSubTypes = subTypes;

    pointData.mMinPoint = {loader.getMinX(), loader.getMinY(), loader.getMinZ()};
    pointData.mMaxPoint = {loader.getMaxX(), loader.getMaxY(), loader.getMaxZ()};

    return true;
}

bool AlgoPointDataConverter::convert(const LogicDbLoader& loader,
                                     PeqiDataDefine::PointData& pointData)
{
    if (!convert(dynamic_cast<const PointCloudLoader&>(loader), pointData))
    {
        return false;
    }
    initializePointIdx(loader.getPointIdxs(), pointData.mPointsIdx);
    initializeLineIds(loader.getLineIds(), pointData.mLineIds);
    initializeLanes(loader.getLanes(), pointData.mLanes);
    initializeLaneInfos(pointData.mLanes, pointData.mLaneConnect);
    initializeConnectLaneIds(loader.getLaneIds(), pointData.mLaneIds);

    return true;
}

void AlgoPointDataConverter::initializePointIdx(
        const std::vector<uint64_t>& gisPointIds,
        PeqiDataDefine::PointLinesIdList& algoPointIdx)
{
    algoPointIdx.clear();
    for (const auto& pointId : gisPointIds)
    {
        algoPointIdx.push_back(pointId);
    }
}

void AlgoPointDataConverter::initializeLineIds(
        const std::vector<uint64_t>& gisLineIds,
        PeqiDataDefine::PointLinesIdList& algoLineIds)
{
    algoLineIds.clear();
    for (const auto& lineId : gisLineIds)
    {
        algoLineIds.push_back(lineId);
    }
}

void AlgoPointDataConverter::initializeLanes(
        const std::vector<std::shared_ptr<DbConverter::Lane>>& gisLanes,
        std::map<int64_t, PeqiDataDefine::ST_OneLane*>& algoLanes)
{
    for (const std::shared_ptr<DbConverter::Lane>& iterLanes : gisLanes)
    {
        PeqiDataDefine::ST_OneLane* algorithmLane =
                new PeqiDataDefine::ST_OneLane();

        // id
        algorithmLane->mIndex = iterLanes->GetIntId();

        // curves
        algorithmLane->mCurve =
                getCurves(iterLanes->GetLeftCurves(), algorithmLane->mCurveNum);

        algorithmLane->mCurveOppo = getCurves(iterLanes->GetRightCurves(),
                                              algorithmLane->mCurveOppoNum);

        // mType is initialized to 0, because it is useless in algorithm now
        algorithmLane->mType = 0;

        // lane connection
        if (iterLanes->GetPredecessor()->GetLanes().empty()
            && iterLanes->GetSuccessor()->GetLanes().empty())
        {
            algorithmLane->mNumConnect = 0;
            algorithmLane->mIDsConnect = Q_NULLPTR;
        }
        else if (iterLanes->GetPredecessor()->GetLanes().empty()
                 || iterLanes->GetSuccessor()->GetLanes().empty())
        {
            algorithmLane->mNumConnect = 1;
            algorithmLane->mIDsConnect =
                    new PeqiDataDefine::ST_ConnectLanes[algorithmLane->mNumConnect];
            getConnection(iterLanes->GetPredecessor()->GetLanes().size() > 0
                                  ? iterLanes->GetPredecessor()
                                  : iterLanes->GetSuccessor(),
                          algorithmLane->mIDsConnect[0]);
        }
        else
        {
            algorithmLane->mNumConnect = 2;
            algorithmLane->mIDsConnect =
                    new PeqiDataDefine::ST_ConnectLanes[algorithmLane->mNumConnect];
            getConnection(iterLanes->GetPredecessor(),
                          algorithmLane->mIDsConnect[0]);
            getConnection(iterLanes->GetSuccessor(), algorithmLane->mIDsConnect[1]);
        }

        algoLanes[iterLanes->GetIntId()] = algorithmLane;
    }
}

void AlgoPointDataConverter::initializeLaneInfos(
        const std::map<int64_t, PeqiDataDefine::ST_OneLane*>& lanes,
        std::vector<PeqiDataDefine::LaneInfo>& laneInfos)
{
    if (lanes.empty())
    {
        return;
    }

    for (auto& iterLanes : lanes)
    {
        auto oneLane = iterLanes.second;

        PeqiDataDefine::LaneInfo oneLaneInfo;
        oneLaneInfo.mLaneId = oneLane->mIndex;

        for (int i = 0; i < oneLane->mNumConnect; ++i)
        {
            oneLaneInfo.mLaneConnect.push_back(oneLane->mIDsConnect[i]);
        }
        laneInfos.push_back(oneLaneInfo);
    }
}

void AlgoPointDataConverter::initializeConnectLaneIds(
        const std::vector<std::vector<uint64_t>>& gisLaneIds,
        PeqiDataDefine::PointLanesIDList& algoLaneIds)
{
    std::size_t lanesCount = gisLaneIds.size();
    algoLaneIds.resize(lanesCount);
    for (std::size_t i = 0; i < lanesCount; ++i)
    {
        auto& gisLanes = gisLaneIds[i];
        std::size_t laneCount = gisLanes.size();

        auto& algoLanes = algoLaneIds[i];
        algoLanes.resize(laneCount);
        for (std::size_t j = 0; j < laneCount; ++j)
        {
            algoLanes[j] = std::int64_t(gisLanes[j]);
        }
    }
}

PeqiDataDefine::ST_Curve* AlgoPointDataConverter::getCurves(
        const std::vector<std::shared_ptr<DbConverter::Curve>>& curves,
        int& curveSize) const
{

    curveSize = 0;
    PeqiDataDefine::ST_Curve* result = Q_NULLPTR;
    for (auto iterCurve : curves)
    {
        curveSize += iterCurve->GetEquationList().size();
    }

    if (curveSize > 0)
    {
        result = new PeqiDataDefine::ST_Curve[curveSize];
        std::size_t i = 0;
        for (const std::shared_ptr<DbConverter::Curve>& curve : curves)
        {
            for (std::shared_ptr<DbConverter::Poly2XYZT> iterEquation :
                 curve->GetEquationList())
            {
                getCurveCoefficients(iterEquation, result[i].mCoeff,
                                     result[i].mCoeffLen);
                result[i].mPtLen = 3; // 3d point

                result[i].mStartPt = new double[result[i].mPtLen];
                result[i].mStartPt[0] = iterEquation->GetStartPoint()->GetX();
                result[i].mStartPt[1] = iterEquation->GetStartPoint()->GetY();
                result[i].mStartPt[2] = iterEquation->GetStartPoint()->GetZ();

                result[i].mEndPt = new double[result[i].mPtLen];
                result[i].mEndPt[0] = iterEquation->GetEndPoint()->GetX();
                result[i].mEndPt[1] = iterEquation->GetEndPoint()->GetY();
                result[i].mEndPt[2] = iterEquation->GetEndPoint()->GetZ();

                result[i].mStartT = iterEquation->GetStart();
                result[i].mEndT = iterEquation->GetEnd();
                ++i;
            }
        }
    }
    return result;
}

void AlgoPointDataConverter::getCurveCoefficients(
        const std::shared_ptr<DbConverter::Poly2XYZT>& poly2XYZT, double*& coef,
        int& size) const
{
    // The logical part of algorithm suport poly3 ONLY
    // set value 4 is temp solution
    std::uint8_t dimension = 4 /*aCurve->GetXParameters().size()*/;
    size = 3 * dimension;
    coef = new double[size];

    // transfer poly2 to poly3, so the paremeters of x^3, y^3, z^3 is 0
    coef[0] = 0;
    coef[1] = poly2XYZT->GetPoly2X()->GetA();
    coef[2] = poly2XYZT->GetPoly2X()->GetB();
    coef[3] = poly2XYZT->GetPoly2X()->GetC();

    coef[0 + dimension] = 0;
    coef[1 + dimension] = poly2XYZT->GetPoly2Y()->GetA();
    coef[2 + dimension] = poly2XYZT->GetPoly2Y()->GetB();
    coef[3 + dimension] = poly2XYZT->GetPoly2Y()->GetC();

    coef[0 + dimension + dimension] = 0;
    coef[1 + dimension + dimension] = poly2XYZT->GetPoly2Z()->GetA();
    coef[2 + dimension + dimension] = poly2XYZT->GetPoly2Z()->GetB();
    coef[3 + dimension + dimension] = poly2XYZT->GetPoly2Z()->GetC();
}

void AlgoPointDataConverter::getConnection(
        const std::shared_ptr<DbConverter::LaneConnection>& connection,
        PeqiDataDefine::ST_ConnectLanes& connectLanes) const
{
    // id number and id set
    connectLanes.mIDNum = connection->GetLanes().size();
    if (connectLanes.mIDNum != 0)
    {
        connectLanes.mIDSet = new std::int64_t[connectLanes.mIDNum];
        std::size_t i = 0;
        for (const std::weak_ptr<DbConverter::Lane>& connectedLane :
             connection->GetLanes())
        {
            connectLanes.mIDSet[i] = connectedLane.lock()->GetIntId();
            ++i;
        }
    }
    else
    {
        connectLanes.mIDSet = Q_NULLPTR;
    }

    // connnection type
    switch (connection->GetType())
    {
        case DbConverter::LaneConnectionType::TypeNull:
            connectLanes.mConnectType =
                    PeqiDataDefine::LaneConnectionType::TypeNull;
            break;
        case DbConverter::LaneConnectionType::TypePredecessorAndSuccessor:
            connectLanes.mConnectType =
                    PeqiDataDefine::LaneConnectionType::TypePredecessorAndSuccessor;
            break;
        default:
            connectLanes.mConnectType =
                    PeqiDataDefine::LaneConnectionType::TypeOthers;
            break;
    }
}

void AlgoPointDataConverter::autoCalculateRange(PeqiDataDefine::PointData& pointData)
{
    const double* point = pointData.mPoints.data();
    pointData.mMinPoint.resize(pointData.mDimension);
    pointData.mMaxPoint.resize(pointData.mDimension);
    for (std::size_t j = 0; j < pointData.mDimension; ++j)
    {
        pointData.mMinPoint[j] = pointData.mMaxPoint[j] = point[j];
    }
    point += pointData.mDimension;

    for (std::size_t i = 1; i < pointData.mPointCount; ++i)
    {
        for (std::size_t j = 0; j < pointData.mDimension; ++j)
        {
            if (pointData.mMinPoint[j] > point[j])
            {
                pointData.mMinPoint[j] = point[j];
            }
            else if (pointData.mMaxPoint[j] < point[j])
            {
                pointData.mMaxPoint[j] = point[j];
            }
        }

        point += pointData.mDimension;
    }
}

PeqiDataDefine::PointLinesIdList AlgoPointDataConverter::gisRoadElementIdListToAlgo(
        const std::vector<uint64_t>& gisRoadElementIds)
{
    PeqiDataDefine::PointLinesIdList lineIds;
    lineIds.resize(gisRoadElementIds.size());

    std::size_t index = 0;
    for (auto& iter : gisRoadElementIds)
    {
        lineIds[index] = std::int64_t(iter);
        ++index;
    }
    return lineIds;
}

PeqiDataDefine::PointIndexList
AlgoPointDataConverter::gisPointIdxListToAlgo(const std::vector<uint64_t>& gisPointIdx)
{
    PeqiDataDefine::PointIndexList indexList;
    indexList.resize(gisPointIdx.size());
    std::size_t index = 0;
    for (auto& iter : gisPointIdx)
    {
        indexList[index] = std::int64_t(iter);
        ++index;
    }
    return indexList;
}

PeqiDataDefine::ClyType
AlgoPointDataConverter::gisPointClassToAlgo(DbConverter::PointClass pointClassId)
{
    switch (pointClassId)
    {
        case DbConverter::PointClass::PointCurve:
        {
            return PeqiDataDefine::ClyType::LaneLine;
        }
        case DbConverter::PointClass::PointTrafficSign:
        {
            return PeqiDataDefine::ClyType::TrafficSign;
        }
        default:
        {
            return PeqiDataDefine::ClyType::Unknown;
        }
    }
}

std::vector<PeqiDataDefine::ClyType> AlgoPointDataConverter::gisPointClassToAlgo(
        const std::vector<DbConverter::PointClass>& gisPointClassIds)
{
    std::vector<PeqiDataDefine::ClyType> algoPointClassIds;
    algoPointClassIds.resize(gisPointClassIds.size());

    int index = 0;
    for (auto& iter : gisPointClassIds)
    {
        algoPointClassIds[index] = gisPointClassToAlgo(iter);
        ++index;
    }

    return algoPointClassIds;
}

} // namespace QiTools
