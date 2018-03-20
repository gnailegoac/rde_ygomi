/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file   AlgoPointDataConverter.h
 * @brief  Convert GIS point data to Algorithm point data
 *******************************************************************************
 */

#ifndef ALGOPOINTDATACONVERTER_H
#define ALGOPOINTDATACONVERTER_H

#include "LogicDbLoader.h"
#include "PeqiDataDefine.h"

namespace Service
{

class AlgoPointDataConverter
{
public:
    AlgoPointDataConverter();

    bool convert(const PointCloudLoader& loader, PeqiDataDefine::PointData& pointData);
    bool convert(const LogicDbLoader& loader, PeqiDataDefine::PointData& pointData);

private:
    void initializePointIdx(const std::vector<std::uint64_t>& gisPointIds,
                            PeqiDataDefine::PointLinesIdList& algoPointIdx);
    void initializeLineIds(const std::vector<uint64_t>& gisLineIds,
                           PeqiDataDefine::PointLinesIdList& algoLineIds);
    void
    initializeLanes(const std::vector<std::shared_ptr<DbConverter::Lane>>& gisLanes,
                    std::map<int64_t, PeqiDataDefine::ST_OneLane*>& algoLanes);
    void
    initializeLaneInfos(const std::map<int64_t, PeqiDataDefine::ST_OneLane*>& lanes,
                        std::vector<PeqiDataDefine::LaneInfo>& laneInfos);
    void initializeConnectLaneIds(const std::vector<std::vector<uint64_t>>& gisLaneIds,
                                  PeqiDataDefine::PointLanesIDList& algoLaneIds);

    PeqiDataDefine::ST_Curve*
    getCurves(const std::vector<std::shared_ptr<DbConverter::Curve>>& curves,
              int& curveSize) const;
    void getCurveCoefficients(const std::shared_ptr<DbConverter::Poly2XYZT>& poly2XYZT,
                              double*& coef, int& size) const;
    void getConnection(const std::shared_ptr<DbConverter::LaneConnection>& connection,
                       PeqiDataDefine::ST_ConnectLanes& connectLanes) const;

    void autoCalculateRange(PeqiDataDefine::PointData& pointData);
    PeqiDataDefine::PointLinesIdList
    gisRoadElementIdListToAlgo(const std::vector<std::uint64_t>& gisRoadElementIds);

    PeqiDataDefine::PointIndexList
    gisPointIdxListToAlgo(const std::vector<std::uint64_t>& gisPointIdx);

    static PeqiDataDefine::ClyType
    gisPointClassToAlgo(DbConverter::PointClass pointClassId);
    static std::vector<PeqiDataDefine::ClyType>
    gisPointClassToAlgo(const std::vector<DbConverter::PointClass>& gisPointClassIds);
};

} // namespace QiTools

#endif // ALGOPOINTDATACONVERTER_H
