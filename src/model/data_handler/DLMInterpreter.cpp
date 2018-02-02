/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DLMInterpreter.cpp
 * @brief
 *******************************************************************************
 */

#include "DLMInterpreter.h"

#include <QDomProcessingInstruction>
#include <QFile>
#include <QTextStream>

#include <sstream>
#include <string>

#include "CoordinateTransform/Factory.h"
#include "model/Utilities.h"

Model::DLMInterpreter::DLMInterpreter(const std::string& aFileName, const double& aInterval) :
    mFileName(aFileName),
    mInterval(aInterval)
{

}

Model::DLMInterpreter::~DLMInterpreter()
{

}

bool Model::DLMInterpreter::SaveDetailJournal(const Model::MemoryModelPtr& aMemoryModel)
{
    QDomDocument domDocument;
    writeInstruction(domDocument);
    QDomElement journal = domDocument.createElement("journal");
    journal.setAttribute("isoCountryCode", DLMSetting::Instance()->GetIsoCountryCode());
    journal.setAttribute("version", DLMSetting::Instance()->GetJournalVersion());
    journal.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    journal.setAttribute("xsi:noNamespaceSchemaLocation", "had.xsd");
    writeMetaInfo(domDocument, journal);
    QDomElement insertElement = domDocument.createElement("insert");
    writeLaneGroups(domDocument, insertElement, aMemoryModel);
    writeLaneSeparatorGroups(domDocument, insertElement, aMemoryModel);
    writeGlobalObjects(domDocument, insertElement, aMemoryModel);
    journal.appendChild(insertElement);
    domDocument.appendChild(journal);

    return saveDLMFile(domDocument);
}

DLM::Marking Model::DLMInterpreter::convertLineType(const Model::CurveType& aCurveType)
{
    DLM::Marking markingType;

    switch (aCurveType)
    {
    case Model::CurveType::Dashed :
        markingType = DLM::Marking::ShortDashedLine;
        break;
    default :
        markingType = DLM::Marking::SingleSolidLine;
        break;
    }

    return markingType;
}

Model::LanePtr Model::DLMInterpreter::getLeftMostLane(const Model::RoadPtr& aRoad)
{
    for (size_t i = 0; i < aRoad->GetLaneListSize(); ++i)
    {
        LanePtr lanePtr = aRoad->GetLane(i);
        // After data validation, there will be only one lane in a road has no left-lane.
        if (0 == lanePtr->GetLeftLaneId())
        {
            return lanePtr;
        }
    }

    return nullptr;
}

Model::LanePtr Model::DLMInterpreter::getRightMostLane(const Model::RoadPtr& aRoad)
{
    for (size_t i = 0; i < aRoad->GetLaneListSize(); ++i)
    {
        LanePtr lanePtr = aRoad->GetLane(i);
        // After data validation, there will be only one lane in a road has no right-lane.
        if (0 == lanePtr->GetRightLaneId())
        {
            return lanePtr;
        }
    }

    return nullptr;
}

Model::LanePtr Model::DLMInterpreter::getLeftLane(const Model::LanePtr& aLane)
{
    std::uint64_t leftLaneId = aLane->GetLeftLaneId();

    if (0 != leftLaneId)
    {
        return aLane->GetRoad()->GetTile()->GetLane(leftLaneId);
    }

    return nullptr;
}

Model::LanePtr Model::DLMInterpreter::getRightLane(const Model::LanePtr& aLane)
{
    std::uint64_t rightLaneId = aLane->GetRightLaneId();

    if (0 != rightLaneId)
    {
        return aLane->GetRoad()->GetTile()->GetLane(rightLaneId);
    }

    return nullptr;
}

uint32_t Model::DLMInterpreter::getLaneIndex(const Model::LanePtr& aLane)
{
    // Describes the index of the lane, counting from the curb side.
    // First lane has index 1, second index 2 and so on.
    std::uint32_t index = 1;
    RoadPtr road = aLane->GetRoad();

    if (DLMSetting::Instance()->GetTrafficSense() == DLM::TrafficSense::RightHand)
    {
        LanePtr lane = getRightMostLane(road);
        while (nullptr != lane)
        {
            if (lane == aLane)
            {
                break;
            }
            ++index;
            lane = getLeftLane(lane);
        }
    }
    else
    {
        LanePtr lane = getLeftMostLane(road);
        while (nullptr != lane)
        {
            if (lane == aLane)
            {
                break;
            }
            ++index;
            lane = getRightLane(lane);
        }
    }

    return index;
}

DLM::ConnectionType Model::DLMInterpreter::getLaneConnectionType(const Model::LanePtr& aLane)
{
    // Currently a lane has at most one successor/predecessor.
    // TODO: Check if a lane is merge/split when have enough information.
    return DLM::ConnectionType::Continuation;
}

bool Model::DLMInterpreter::saveDLMFile(QDomDocument& aDocument) const
{
    QFile file(QString::fromStdString(mFileName));

    try
    {
        if(!file.open(QIODevice::WriteOnly))
        {
            return false;
        }

        QTextStream out(&file);
        aDocument.save(out, 2);
        file.close();
    }
    catch(...)
    {
        file.close();
        return false;
    }

    return true;
}

void Model::DLMInterpreter::writeInstruction(QDomDocument& aDom)
{
    QDomProcessingInstruction instruction;
    instruction = aDom.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"");
    aDom.appendChild(instruction);
}

void Model::DLMInterpreter::writeMetaInfo(QDomDocument& aDom, QDomElement& aParent)
{
    QDomElement meta = aDom.createElement("meta");
    writeNumberNode(aDom, meta, "sourceVersion", DLMSetting::Instance()->GetSourceVersion());
    writeNumberNode(aDom, meta, "targetVersion", DLMSetting::Instance()->GetTargetVersion());
    writeNumberNode(aDom, meta, "baselineMapId", DLMSetting::Instance()->GetBaselineMapId());
    aParent.appendChild(meta);
}

void Model::DLMInterpreter::writeLaneGroups(QDomDocument& aDom, QDomElement& aParent,
                                            const Model::MemoryModelPtr& aMemoryModel)
{
    for (const auto& tile : *(aMemoryModel->GetTileMap()))
    {
        for (const auto& road : *(tile.second->GetRoadMap()))
        {
            writeLaneGroup(aDom, aParent, road.second);
        }
    }
}

void Model::DLMInterpreter::writeLaneGroup(QDomDocument& aDom, QDomElement& aParent, const Model::RoadPtr& aRoad)
{
    QDomElement laneGroup = aDom.createElement("laneGroup");
    writeNumberNode(aDom, laneGroup, "dlmId", aRoad->GetRoadId());
    // TODO: dont have version for road.
    writeNumberNode(aDom, laneGroup, "version", 1);
    for (const auto& lane : *(aRoad->GetLaneList()))
    {
        writeLane(aDom, laneGroup, lane);
    }
    aParent.appendChild(laneGroup);
}

void Model::DLMInterpreter::writeLane(QDomDocument& aDom, QDomElement& aParent, const Model::LanePtr& aLane)
{
    QDomElement laneElement = aDom.createElement("lane");
    // TODO: currently dont have lane type, just use "NORMAL".
    laneElement.setAttribute("type", "NORMAL");
    DLM::ConnectionType connectionType = getLaneConnectionType(aLane);
    if (DLM::ConnectionType::Continuation != connectionType)
    {
        laneElement.setAttribute("connectionType", DLM::scConnectionTypeMap.at(connectionType));
    }
    writeNumberNode(aDom, laneElement, "dlmId", aLane->GetLaneId());
    writeNumberNode(aDom, laneElement, "laneIndex", getLaneIndex(aLane));
    writeLaneSuccessors(aDom, laneElement, aLane);
    // The minimum width of the lane in centimeters.
    // TODO: dont have lane width, use default value.
    writeNumberNode(aDom, laneElement, "width", 375);
    // Currently use avgSlamTrace as centerline
    // TODO: will use centerline when there is in DB.
    writeGeometry(aDom, laneElement, aLane->GetAvgSlamTrace());
    writeQualityMetadata(aDom, laneElement, aLane);
    writeReferenceLaneSeparator(aDom, laneElement, aLane);
    aParent.appendChild(laneElement);
}

void Model::DLMInterpreter::writeLaneSuccessors(QDomDocument& aDom, QDomElement& aParent, const Model::LanePtr& aLane)
{
    // TODO: a lane has at most one successor currently.
    if (aLane->GetSuccessorLaneId() != 0)
    {
        QDomElement successorsElement = aDom.createElement("laneSuccessors");
        writeNumberNode(aDom, successorsElement, "laneRef", aLane->GetSuccessorLaneId());
        aParent.appendChild(successorsElement);
    }
}

void Model::DLMInterpreter::writeQualityMetadata(QDomDocument& aDom, QDomElement& aParent, const Model::LanePtr& aLane)
{
    QDomElement qualityMetadata = aDom.createElement("qualityMetadata");
    // How the data has been generated: LAND_SURVEYED, FLEET_DATA, NOT_VERIFIED, UNKNOWN.
    // TODO: dont have this field, just use FLEET_DATA.
    writeTextNode(aDom, qualityMetadata, "lineage", "FLEET_DATA");
    // Describes level of confidence the data has been captured, range [0,100].
    // Use left line confidence as the lane confidence.
    writeNumberNode(aDom, qualityMetadata, "confidence",
                    static_cast<std::uint32_t>(100 * aLane->GetLeftLine()->GetConfidence()));
    aParent.appendChild(qualityMetadata);
}

void Model::DLMInterpreter::writeReferenceLaneSeparator(QDomDocument& aDom, QDomElement& aParent,
                                                        const Model::LanePtr& aLane)
{
    if (DLM::TrafficSense::RightHand == DLMSetting::Instance()->GetTrafficSense())
    {
        writeLaneSeparatorIndex(aDom, aParent, "laneSeparatorGroupMiddleSideIndex", aLane->GetLeftLine());
        writeLaneSeparatorIndex(aDom, aParent, "laneSeparatorGroupCurbSideIndex", aLane->GetRightLine());
    }
    else
    {
        writeLaneSeparatorIndex(aDom, aParent, "laneSeparatorGroupMiddleSideIndex", aLane->GetRightLine());
        writeLaneSeparatorIndex(aDom, aParent, "laneSeparatorGroupCurbSideIndex", aLane->GetLeftLine());
    }
}

void Model::DLMInterpreter::writeLaneSeparatorIndex(QDomDocument& aDom, QDomElement& aParent,
                                                    const QString& aName, const Model::LinePtr& aLine)
{
    QDomElement element = aDom.createElement(aName);
    // Includes the dlmId of the lane separator that is referenced
    writeNumberNode(aDom, element, "dlmId", aLine->GetLineId());
    // Indicates whether the geometry of the lane is in in the same direction as
    // the geometry of the lane separator group (true) or in reverse direction (false).
    writeTextNode(aDom, element, "sameDirection", "true");
    aParent.appendChild(element);
}

void Model::DLMInterpreter::writeLaneSeparatorGroups(QDomDocument& aDom, QDomElement& aParent,
                                                     const Model::MemoryModelPtr& aMemoryModel)
{
    QVector<std::uint64_t> lineIds;
    for (const auto& tile : *(aMemoryModel->GetTileMap()))
    {
        for (const auto& road : *(tile.second->GetRoadMap()))
        {
            for (const auto& lane : *(road.second->GetLaneList()))
            {
                addLine(aDom, aParent, lane->GetLeftLine(), lineIds);
                addLine(aDom, aParent, lane->GetRightLine(), lineIds);
            }
        }
    }
}

void Model::DLMInterpreter::writeLaneSeparatorGroup(QDomDocument& aDom, QDomElement& aParent, const Model::LinePtr& aLine)
{
    QDomElement laneSeparatorGroup = aDom.createElement("laneSeparatorGroup");
    writeNumberNode(aDom, laneSeparatorGroup, "dlmId", aLine->GetLineId());
    // TODO: Every element should have version, only a default version currently.
    writeNumberNode(aDom, laneSeparatorGroup, "version", 1);
    writeLaneSeparator(aDom, laneSeparatorGroup, aLine);
    aParent.appendChild(laneSeparatorGroup);
}

void Model::DLMInterpreter::addLine(QDomDocument& aDom, QDomElement& aParent, const Model::LinePtr& aLine,
                                    QVector<std::uint64_t>& aLineIdVec)
{
    if (aLineIdVec.count(aLine->GetLineId()) == 0)
    {
        writeLaneSeparatorGroup(aDom, aParent, aLine);
        aLineIdVec.push_back(aLine->GetLineId());
    }
}

void Model::DLMInterpreter::writeLaneSeparator(QDomDocument& aDom, QDomElement& aParent, const Model::LinePtr& aLine)
{
    QDomElement laneSeparator = aDom.createElement("laneSeparator");
    writeTextNode(aDom, laneSeparator, "marking", DLM::scMarkingTypeMap.at(convertLineType(aLine->GetLineType())));
    writeGeometry(aDom, laneSeparator, aLine);
    // TODO: dont have material type currently, just use default concrete type.
    writeTextNode(aDom, laneSeparator, "material", DLM::scMaterialTypeMap.at(DLM::Material::Concrete));
    aParent.appendChild(laneSeparator);
}

void Model::DLMInterpreter::writeGlobalObjects(QDomDocument& aDom, QDomElement& aParent,
                                               const Model::MemoryModelPtr& aMemoryModel)
{
    for (const auto& tile : *(aMemoryModel->GetTileMap()))
    {
        for (const auto& trafficSign : *(tile.second->GetTrafficSignMap()))
        {
            if (DLM::scSpeedLimitTypeValueMap.count(trafficSign.second->GetTrafficSignType()) > 0)
            {
                writeSpeedLimitSign(aDom, aParent, trafficSign.second);
            }
        }
    }
}

void Model::DLMInterpreter::writeSpeedLimitSign(QDomDocument& aDom, QDomElement& aParent,
                                                const Model::TrafficSignPtr& aTrafficSign)
{
    QDomElement globalObject = aDom.createElement("globalLaneGroupObject");
    writeNumberNode(aDom, globalObject, "dlmId", aTrafficSign->GetTrafficSignId());
    QDomElement speedLimitSign = aDom.createElement("speedLimitSign");
    writePosition3D(aDom, speedLimitSign, aTrafficSign->GetGeodeticPosition());
    // SpeedLimitSign Type: MINIMUM, MAXIMUM, ADVISORY.
    // Currently only have maximum speed limit.
    writeTextNode(aDom, speedLimitSign, "type", "MAXIMUM");
    // The numeric value as posted on the road, in the unit used in the region
    writeNumberNode(aDom, speedLimitSign, "numericValue",
                    DLM::scSpeedLimitTypeValueMap.at(aTrafficSign->GetTrafficSignType()));
    globalObject.appendChild(speedLimitSign);
    aParent.appendChild(globalObject);
}

void Model::DLMInterpreter::writePosition3D(QDomDocument& aDom, QDomElement& aParent,
                                            const Model::Point3DPtr& aPosition)
{
    std::stringstream ss;
    ss << "POINTZ(";
    ss << strings::FormatFloat<double>(aPosition->GetX(), 12) << " ";
    ss << strings::FormatFloat<double>(aPosition->GetY(), 12) << " ";
    ss << strings::FormatFloat<double>(aPosition->GetZ(), 12) << ")";
    writeTextNode(aDom, aParent, "wktPoint3d", ss.str().c_str());
}

void Model::DLMInterpreter::writeGeometry(QDomDocument& aDom, QDomElement& aParent, const Model::LinePtr& aLine)
{
    const Point3DPtr& referencePoint = aLine->GetLane()->GetRoad()->GetTile()->GetReferencePoint();
    std::shared_ptr<CRS::Factory> factory = std::make_shared<CRS::Factory>();
    std::unique_ptr<CRS::ICoordinateTransform> relativeToWgs84 =
                    factory->CreateRelativeTransform(CRS::CoordinateType::Relative,
                                                     CRS::CoordinateType::Wgs84,
                                                     referencePoint->GetX(),
                                                     referencePoint->GetY(),
                                                     referencePoint->GetZ());
    std::stringstream ss;
    ss << "LINESTRINGZ(";
    for (const auto& curve : *(aLine->GetCurveList()))
    {
        Point3DListPtr points = curve->CalculatePointCloud(mInterval);
        for (const auto& point : *points)
        {
            double x = point->GetX();
            double y = point->GetY();
            double z = point->GetZ();
            relativeToWgs84->Transform(x, y, z);
            ss << strings::FormatFloat<double>(x, 12) << " ";
            ss << strings::FormatFloat<double>(y, 12) << " ";
            ss << strings::FormatFloat<double>(z, 12) << ", ";
        }
    }
    std::string lineString = ss.str();
    lineString.replace(lineString.length() - 2, std::string::npos, ")");
    writeTextNode(aDom, aParent, "wktLineString3d", lineString.c_str());
}

void Model::DLMInterpreter::writeTextNode(QDomDocument& aDom, QDomElement& aParent,
                                          const QString& aName, const QString& aValue)
{
    QDomElement newElement = aDom.createElement(aName);
    QDomText textNode = aDom.createTextNode(aValue);
    newElement.appendChild(textNode);
    aParent.appendChild(newElement);
}

void Model::DLMInterpreter::writeNumberNode(QDomDocument& aDom, QDomElement& aParent,
                                            const QString& aName, const uint32_t& aValue)
{
    QDomElement newElement = aDom.createElement(aName);
    QDomText textNode = aDom.createTextNode(QString::number(aValue));
    newElement.appendChild(textNode);
    aParent.appendChild(newElement);
}
