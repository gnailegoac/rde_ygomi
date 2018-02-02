/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DLMInterpreter.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include <mutex>
#include <string>

#include <QDomDocument>
#include <QDomElement>
#include <QVector>

#include "DLMSetting.h"
#include "../MemoryModel.h"
#include "../IdGenerator.h"

namespace Model
{
typedef std::shared_ptr<MemoryModel> MemoryModelPtr;

class DLMInterpreter
{
public:
    DLMInterpreter(const std::string& aFileName, const double& aInterval = 0.5);
    ~DLMInterpreter();

    bool SaveDetailJournal(const MemoryModelPtr& aMemoryModel);

private:
    DLM::Marking convertLineType(const CurveType& aCurveType);
    LanePtr getLeftMostLane(const RoadPtr& aRoad);
    LanePtr getRightMostLane(const RoadPtr& aRoad);
    LanePtr getLeftLane(const LanePtr& aLane);
    LanePtr getRightLane(const LanePtr& aLane);
    std::uint32_t getLaneIndex(const LanePtr& aLane);
    DLM::ConnectionType getLaneConnectionType(const LanePtr& aLane);
    bool saveDLMFile(QDomDocument& aDocument) const;
    void writeInstruction(QDomDocument& aDom);
    void writeMetaInfo(QDomDocument& aDom, QDomElement& aParent);
    void writeLaneGroups(QDomDocument& aDom, QDomElement& aParent, const MemoryModelPtr& aMemoryModel);
    void writeLaneGroup(QDomDocument& aDom, QDomElement& aParent, const RoadPtr& aRoad);
    void writeLane(QDomDocument& aDom, QDomElement& aParent, const LanePtr& aLane);
    void writeLaneSuccessors(QDomDocument& aDom, QDomElement& aParent, const LanePtr& aLane);
    void writeQualityMetadata(QDomDocument& aDom, QDomElement& aParent, const LanePtr& aLane);
    void writeReferenceLaneSeparator(QDomDocument& aDom, QDomElement& aParent, const LanePtr& aLane);
    void writeLaneSeparatorIndex(QDomDocument& aDom, QDomElement& aParent, const QString& aName, const LinePtr& aLine);
    void writeLaneSeparatorGroups(QDomDocument& aDom, QDomElement& aParent, const MemoryModelPtr& aMemoryModel);
    void writeLaneSeparatorGroup(QDomDocument& aDom, QDomElement& aParent, const LinePtr& aLine);
    void addLine(QDomDocument& aDom, QDomElement& aParent, const LinePtr& aLine, QVector<std::uint64_t>& aLineIdVec);
    void writeLaneSeparator(QDomDocument& aDom, QDomElement& aParent, const LinePtr& aLine);
    void writeGlobalObjects(QDomDocument& aDom, QDomElement& aParent, const MemoryModelPtr& aMemoryModel);
    void writeSpeedLimitSign(QDomDocument& aDom, QDomElement& aParent, const TrafficSignPtr& aTrafficSign);
    void writePosition3D(QDomDocument& aDom, QDomElement& aParent, const Point3DPtr& aPosition);
    void writeGeometry(QDomDocument& aDom, QDomElement& aParent, const LinePtr& aLine);
    void writeTextNode(QDomDocument& aDom, QDomElement& aParent, const QString& aName, const QString& aValue);
    void writeNumberNode(QDomDocument& aDom, QDomElement& aParent, const QString& aName, const std::uint32_t& aValue);
    std::uint32_t getUnifiedRoadId(const RoadPtr& aRoad);
    std::uint32_t getUnifiedLaneId(const LanePtr& aLane);
    std::uint32_t getUnifiedLineId(const LinePtr& aLine);
    std::uint32_t getUnifiedTrafficSignId(const TrafficSignPtr& aTrafficSign);

private:
    std::string mFileName;
    double mInterval;
    std::mutex mMutex;
    IdGenerator mIdGenerator;
    std::unordered_map<std::string, std::uint32_t> mUnifiedIdMap;
};

typedef std::shared_ptr<DLMInterpreter> DLMInterpreterPtr;

}
