/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DbRepository.cpp
 * @brief
 *******************************************************************************
 */

#include "DbRepository.h"

#include "SQLiteCpp/Database.h"
#include "SQLiteCpp/Transaction.h"
#include "SQLiteCpp/Statement.h"

#include "DbParseException.h"
#include "../MemoryModel.h"
#include "../Utilities.h"
#include "../NurbsCurve.h"

namespace
{

const std::string gcSegment = "Segment_";
const std::string gcLane = "Lane_";
const std::string gcLaneConnection = "LaneConnection_";
const std::string gcLine = "Line_";
const std::string gcCurve = "Curve_";
const std::string gcTrafficSign = "TrafficSign_";
const std::string gcRoad = "Road_";

}

Model::DbRepository::DbRepository(const std::string& aDbFileName):
    mDbFileName(aDbFileName),
    mTables(std::make_shared<StringList>()),
    mRoadTables(std::make_shared<StringList>()),
    mSegmentTables(std::make_shared<StringList>()),
    mLaneTables(std::make_shared<StringList>()),
    mLaneConnectionTables(std::make_shared<StringList>()),
    mLineTables(std::make_shared<StringList>()),
    mCurveTables(std::make_shared<StringList>()),
    mTrafficSignTables(std::make_shared<StringList>()),
    mDatabase(nullptr)
{

}

Model::DbRepository::~DbRepository()
{

}

bool Model::DbRepository::InitializeDataBase()
{
    try
    {
        mDatabase = std::make_shared<SQLite::Database>(mDbFileName);
    }
    catch(...)
    {
        return false;
    }

    return queryAllTables();
}

bool Model::DbRepository::QueryRoadElement(std::shared_ptr<Model::MemoryModel>& aMemoryModel)
{
    try
    {
        querySegments(aMemoryModel);
        queryLanes(aMemoryModel);
        queryCurves(aMemoryModel);
        queryTrafficSigns(aMemoryModel);
        queryLines(aMemoryModel);
    }
    catch(DbParseException)
    {
        return false;
    }

    return true;
}

bool Model::DbRepository::QueryTopology(std::shared_ptr<Model::MemoryModel>& aMemoryModel)
{
    try
    {
        queryLaneConnection(aMemoryModel);
    }
    catch(DbParseException)
    {
        return false;
    }

    return true;
}

bool Model::DbRepository::GenerateDataBase(const std::shared_ptr<Tile>& aTile)
{
    try
    {
        mDatabase = std::make_shared<SQLite::Database>(mDbFileName, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE);
    }
    catch(...)
    {
        return false;
    }

    return createAllTables(aTile->GetTileId());
}

bool Model::DbRepository::StoreRoadElement(const std::shared_ptr<MemoryModel>& aMemoryModel)
{
    try
    {
        storeSegments(aMemoryModel);
        storeRoads(aMemoryModel);
        storeLines(aMemoryModel);
        storeCurves(aMemoryModel);
        storeLanes(aMemoryModel);
        storeTrafficSigns(aMemoryModel);
    }
    catch(DbParseException& e)
    {
        qCritical(e.what());
        return false;
    }

    return true;
}

bool Model::DbRepository::StoreTopology(const std::shared_ptr<MemoryModel>& aMemoryModel)
{
    try
    {
        storeLaneConnections(aMemoryModel);
    }
    catch(DbParseException& e)
    {
        qCritical(e.what());
        return false;
    }

    return true;
}

void Model::DbRepository::querySegments(std::shared_ptr<MemoryModel>& aMemoryModel) const
{
    for (const std::string& segmentTable : *mSegmentTables)
    {
        const std::string QUERY_SEGMENT = "SELECT ID,ReferencePoint FROM " + segmentTable;

        try
        {
            SQLite::Statement query(*mDatabase, QUERY_SEGMENT);

            while (query.executeStep())
            {
                std::int64_t tileId = query.getColumn(0).getInt64();
                TilePtr tile = aMemoryModel->GetMutableTile(tileId);
                tile->SetReferencePoint(parsePoint(query.getColumn(1).getString()));
            }
        }
        catch (std::exception)
        {
            throw(DbParseException(DbParseException::Type::DbParseError,
                                     "Segment Table Query Error"));
        }
    }
}

void Model::DbRepository::queryCurves(std::shared_ptr<MemoryModel>& aMemoryModel) const
{
    for (const std::string& curveTable : *mCurveTables)
    {
        const std::int64_t segmentId = (std::int64_t)std::stoi(strings::Split(curveTable, "_")[1]);
        TilePtr tile = aMemoryModel->GetMutableTile(segmentId);
        std::string QUERY_CURVE = "SELECT ID,LineID,IndexInLine,Type,Width,"
                                    "EquationDescription,Length,Color FROM " + curveTable
                                    + " WHERE Type in ('0','1','2','3')";

        try
        {
            SQLite::Statement query(*mDatabase, QUERY_CURVE);

            while (query.executeStep())
            {
                double length = query.getColumn(6).getDouble();

                if (length != 0)
                {
                    NurbsCurvePtr nurbsCurve = std::make_shared<NurbsCurve>();

                    if (!nurbsCurve->Convert(query.getColumn(5).getString()))
                    {
                        throw(DbParseException(DbParseException::Type::DbParseError,
                                                 "Parse Curve Equation Failed"));
                    }

                    std::uint64_t curveId = aMemoryModel->GetCurveIntId(query.getColumn(0).getString());

                    nurbsCurve->SetCurveId(curveId);
                    nurbsCurve->SetLength(length);
                    nurbsCurve->SetCurveId(curveId);
                    nurbsCurve->SetIndexInLine(query.getColumn(2).getUInt());
                    nurbsCurve->SetCurveType(CurveType(query.getColumn(3).getUInt()));
                    nurbsCurve->SetWidth(static_cast<std::uint8_t>(query.getColumn(4).getInt()));

                    std::uint64_t lineId = aMemoryModel->GetLineIntId(query.getColumn(1).getString());
                    LinePtr line = tile->GetMutableLine(lineId);
                    line->GetMutableCurveList()->push_back(nurbsCurve);

                    if (CurveType::SlamTrace == nurbsCurve->GetCurveType())
                    {
                        line->SetEditable(false);
                    }
                }
            }
        }
        catch(DbParseException e)
        {
            throw(e);
        }
        catch(...)
        {
            throw(DbParseException(DbParseException::Type::DbParseError,
                                     "Curve Table Query Error"));
        }
    }
}

void Model::DbRepository::queryLaneConnection(std::shared_ptr<MemoryModel>& aMemoryModel) const
{
    for (const std::string& connectionTable : *mLaneConnectionTables)
    {
        const std::string QUERY_LANE_CONNECTION = "SELECT SrcID,DstID,Type FROM "
                                                  + connectionTable;
        try
        {
            SQLite::Statement query(*mDatabase, QUERY_LANE_CONNECTION);

            while (query.executeStep())
            {
                std::uint64_t sourceLaneId = aMemoryModel->GetLaneIntId(query.getColumn(0).getString());
                std::uint64_t destinationLaneId = aMemoryModel->GetLaneIntId(query.getColumn(1).getString());
                TilePtr sourceTile = aMemoryModel->GetTileByLaneId(sourceLaneId);
                TilePtr destinationTile = aMemoryModel->GetTileByLaneId(destinationLaneId);
                LaneConnectionType type = (LaneConnectionType)std::stoi(query.getColumn(2).getString());
                setLaneConnection(sourceTile, sourceLaneId, destinationTile, destinationLaneId, type);
            }
        }
        catch (DbParseException e)
        {
            throw(e);
        }
        catch (std::exception)
        {
            throw(DbParseException(DbParseException::Type::DbParseError,
                                     "Lane Connection Table Query Error"));
        }
    }
}

void Model::DbRepository::queryLanes(std::shared_ptr<MemoryModel>& aMemoryModel) const
{
    for (const std::string& laneTable : *mLaneTables)
    {
        const std::string QUERY_LANE = "SELECT ID,LLineID,RLineID,AvgSlamTrace FROM " + laneTable;
        const std::int64_t segmentId = (std::int64_t)std::stoi(strings::Split(laneTable, "_")[1]);
        TilePtr tile = aMemoryModel->GetMutableTile(segmentId);

        try
        {
            SQLite::Statement query(*mDatabase, QUERY_LANE);
            while (query.executeStep())
            {
                std::uint64_t leftLineId = aMemoryModel->GetLineIntId(query.getColumn(1).getString());
                std::uint64_t rightLineId = aMemoryModel->GetLineIntId(query.getColumn(2).getString());
                std::uint64_t avgSlamTraceId = aMemoryModel->GetLineIntId(query.getColumn(3).getString());
                LinePtr leftLine = tile->GetMutableLine(leftLineId);
                LinePtr rightLine = tile->GetMutableLine(rightLineId);
                LinePtr avgSlamTrace = tile->GetMutableLine(avgSlamTraceId);

                std::uint64_t laneId = aMemoryModel->GetLaneIntId(query.getColumn(0).getString());
                LanePtr lane = tile->GetMutableLane(laneId);
                lane->SetLeftLine(leftLine);
                lane->SetRightLine(rightLine);
                lane->SetAvgSlamTrace(avgSlamTrace);
                leftLine->SetLane(lane);
                rightLine->SetLane(lane);
                avgSlamTrace->SetLane(lane);

                std::uint64_t roadId = (std::uint64_t)query.getColumn(0).getInt64();
                RoadPtr road = tile->GetMutableRoad(roadId);
                lane->SetRoad(road);
                road->GetMutableLaneList()->push_back(lane);
            }
        }
        catch(std::exception)
        {
            throw(DbParseException(DbParseException::Type::DbParseError,
                                     "Lane Table Query Error"));
        }
    }
}

void Model::DbRepository::queryLines(std::shared_ptr<MemoryModel>& aMemoryModel) const
{
    for (const std::string& lineTable : *mLineTables)
    {
        const std::string QUERY_LINE = "SELECT * FROM " + lineTable;
        const std::int64_t segmentId = (std::int64_t)std::stoi(strings::Split(lineTable, "_")[1]);
        TilePtr tile = aMemoryModel->GetMutableTile(segmentId);

        try
        {
            SQLite::Statement query(*mDatabase, QUERY_LINE);
            while (query.executeStep())
            {
                std::uint64_t lineId = aMemoryModel->GetLineIntId(query.getColumn(0).getString());
                LinePtr line = tile->GetMutableLine(lineId);

                line->SetConfidence(query.getColumn(2).getDouble());
                line->SetLength(query.getColumn(3).getDouble());
            }
        }
        catch(std::exception)
        {
            throw(DbParseException(DbParseException::Type::DbParseError,
                                     "Line Table Query Error"));
        }
    }
}

void Model::DbRepository::queryTrafficSigns(std::shared_ptr<MemoryModel>& aMemoryModel) const
{
    for (const std::string& trafficSignTable : *mTrafficSignTables)
    {
        const std::int64_t segmentId = (std::int64_t)std::stoi(strings::Split(trafficSignTable, "_")[1]);
        TilePtr tile = aMemoryModel->GetMutableTile(segmentId);
        const std::string QUERY_TRAFFIC_SIGN = "SELECT * FROM "
                                               + trafficSignTable;

        try
        {
            SQLite::Statement query(*mDatabase, QUERY_TRAFFIC_SIGN);

            while (query.executeStep())
            {
                std::uint64_t trafficSignId = aMemoryModel->GetTrafficSignIntId(query.getColumn(0).getString());
                TrafficSignPtr trafficSign = tile->GetMutableTrafficSign(trafficSignId);

                trafficSign->SetTrafficSignId(trafficSignId);
                trafficSign->SetOrientation(query.getColumn(3).getDouble());
                trafficSign->SetTrafficSignType((std::uint64_t)query.getColumn(2).getInt64());
                trafficSign->SetConfidence(query.getColumn(6).getDouble());
                trafficSign->SetShapeHeight(query.getColumn(5).getDouble());
                trafficSign->SetShapeWidth(query.getColumn(4).getDouble());
                trafficSign->SetGeodeticPosition(parsePoint(query.getColumn(7).getString()));
            }
        }
        catch(std::exception)
        {
            throw(DbParseException(DbParseException::Type::DbParseError,
                                     "Traffic Sign Table Query Error"));
        }
    }
}

bool Model::DbRepository::queryAllTables()
{
    const std::string QUERY_ALL_TABLES
        = "SElECT name FROM sqlite_master WHERE type='table'";

    try
    {
        SQLite::Statement query(*mDatabase, QUERY_ALL_TABLES);

        while (query.executeStep())
        {
            mTables->push_back(query.getColumn(0).getString());
        }
    }
    catch(std::exception)
    {
        return false;
    }

    std::copy_if(mTables->begin(), mTables->end(),
                 std::back_inserter(*mSegmentTables),
                 [](const std::string & aTableName)
    {
        return strings::StartWith(aTableName, gcSegment);
    });

    std::copy_if(mTables->begin(), mTables->end(),
                 std::back_inserter(*mLaneTables),
                 [](const std::string & aTableName)
    {
        return strings::StartWith(aTableName, gcLane);
    });

    std::copy_if(mTables->begin(), mTables->end(),
                 std::back_inserter(*mLaneConnectionTables),
                 [](const std::string & aTableName)
    {
        return strings::StartWith(aTableName, gcLaneConnection);
    });

    std::copy_if(mTables->begin(), mTables->end(),
                 std::back_inserter(*mLineTables),
                 [](const std::string & aTableName)
    {
        return strings::StartWith(aTableName, gcLine);
    });

    std::copy_if(mTables->begin(), mTables->end(),
                 std::back_inserter(*mCurveTables),
                 [](const std::string & aTableName)
    {
        return strings::StartWith(aTableName, gcCurve);
    });

    std::copy_if(mTables->begin(), mTables->end(),
                 std::back_inserter(*mTrafficSignTables),
                 [](const std::string & aTableName)
    {
        return strings::StartWith(aTableName, gcTrafficSign);
    });

    return true;
}

Model::Point3DPtr Model::DbRepository::parsePoint(const std::string& aPointString) const
{
    std::vector<std::string> coords = strings::Split(aPointString, ",");

    if (3 == coords.size())
    {
        std::shared_ptr<Point3D> result = std::make_shared<Point3D>();
        result->SetX(std::stod(coords[0]));
        result->SetY(std::stod(coords[1]));
        result->SetZ(std::stod(coords[2]));
        return result;
    }
    else
    {
        return nullptr;
    }
}

void Model::DbRepository::setLaneConnection(const TilePtr& aSourceTile,
                                            const std::uint64_t& aSourceId,
                                            const TilePtr& aDestinationTile,
                                            const std::uint64_t& aDestinationId,
                                            const LaneConnectionType aType) const
{
    LanePtr sourceLane = nullptr;
    LanePtr destinationLane = nullptr;

    if (nullptr == aSourceTile
            || nullptr == (sourceLane = aSourceTile->GetLane(aSourceId)))
    {
        throw(DbParseException(DbParseException::Type::DbParseError,
                               "Cannot Find the Source Lane " + aSourceId));
    }

    if (nullptr == aDestinationTile
            || nullptr == (destinationLane = aDestinationTile->GetLane(aDestinationId)))
    {
        throw(DbParseException(DbParseException::Type::DbParseError,
                               "Cannot Find the Destination Lane" + aDestinationId));
    }

    if (aSourceId != aDestinationId)
    {
        if (LaneConnectionType::LaneConnectionPredecessor == aType)
        {
            sourceLane->SetPredecessorLaneId(aDestinationId);
            destinationLane->SetSuccessorLaneId(aSourceId);
        }
        else if (LaneConnectionType::LaneConnectionSuccessor == aType)
        {
            sourceLane->SetSuccessorLaneId(aDestinationId);
            destinationLane->SetPredecessorLaneId(aSourceId);
        }
        else if (LaneConnectionType::LaneConnectionLeft == aType)
        {
            sourceLane->SetLeftLaneId(aDestinationId);
            destinationLane->SetRightLaneId(aSourceId);
        }
        else if (LaneConnectionType::LaneConnectionRight == aType)
        {
            sourceLane->SetRightLaneId(aDestinationId);
            destinationLane->SetLeftLaneId(aSourceId);
        }
    }
}

void Model::DbRepository::storeSegments(const std::shared_ptr<MemoryModel>& aMemoryModel) const
{
    try
    {
        for (const std::string& segmentTale : *mSegmentTables)
        {
            const std::int64_t segmentId = (std::int64_t)std::stoi(strings::Split(segmentTale, "_")[1]);
            TilePtr tile = aMemoryModel->GetMutableTile(segmentId);

            mDatabase->exec("INSERT INTO " + segmentTale + " VALUES ("
                             + std::to_string(tile->GetTileId()) + ","
                             + wrapText(formatReferencePoint(tile->GetReferencePoint()), false) + ")"
                            );
        }
    }
    catch(DbParseException e)
    {
        throw(e);
    }
    catch(...)
    {
        throw(DbParseException(DbParseException::Type::DbStoreError,
                                 "Segment Table Store Error"));
    }
}

void Model::DbRepository::storeRoads(const std::shared_ptr<MemoryModel>& aMemoryModel) const
{
    try
    {
        for (const std::string& roadTable : *mRoadTables)
        {
            const std::int64_t segmentId = (std::int64_t)std::stoi(strings::Split(roadTable, "_")[1]);
            TilePtr tile = aMemoryModel->GetMutableTile(segmentId);
            const RoadMapPtr& roadMap = tile->GetRoadMap();

            SQLite::Transaction transaction(*mDatabase);

            for (auto& itorRoad : *roadMap)
            {
                const RoadPtr& road = itorRoad.second;

                mDatabase->exec("INSERT INTO " + roadTable + "(ID,SegID) VALUES ("
                                + std::to_string(road->GetRoadId()) + ","
                                + std::to_string(segmentId) + ")"
                                );
            }

            transaction.commit();
        }

    }
    catch(DbParseException e)
    {
        throw(e);
    }
    catch(...)
    {
        throw(DbParseException(DbParseException::Type::DbStoreError,
                                 "Road Table Store Error"));
    }
}

void Model::DbRepository::storeLines(const std::shared_ptr<MemoryModel>& aMemoryModel) const
{
    std::string sqlText("");

    try
    {
        for (const std::string& lineTable : *mLineTables)
        {
            const std::int64_t segmentId = (std::int64_t)std::stoi(strings::Split(lineTable, "_")[1]);
            TilePtr tile = aMemoryModel->GetMutableTile(segmentId);
            const LineMapPtr& lineMap = tile->GetLineMap();

            SQLite::Transaction transaction(*mDatabase);

            for (auto& itorLine : *lineMap)
            {
                const LinePtr& line = itorLine.second;
                const uint64_t& lineId = line->GetLineId();
                const uint64_t& laneId = line->GetLane()->GetLaneId();

                sqlText = "INSERT INTO " + lineTable
                          + "(ID, LaneID, Confidence, Length) VALUES ("
                          + wrapText(aMemoryModel->GetLineTextId(lineId))
                          + wrapText(aMemoryModel->GetLaneTextId(laneId))
                          + strings::FormatFloat<double>(line->GetConfidence(), 8) + ","
                          + strings::FormatFloat<double>(line->GetLength(), 8) + ")";
                mDatabase->exec(sqlText);
            }

            transaction.commit();
        }

    }
    catch(DbParseException e)
    {
        throw(e);
    }
    catch(...)
    {
        throw(DbParseException(DbParseException::Type::DbStoreError,
                                 "Line Table Store Error with sql:" + sqlText));
    }
}

void Model::DbRepository::storeCurves(const std::shared_ptr<MemoryModel>& aMemoryModel) const
{
    std::string sqlText("");

    try
    {
        for (const std::string& curveTable : *mCurveTables)
        {
            const std::int64_t segmentId = (std::int64_t)std::stoi(strings::Split(curveTable, "_")[1]);
            TilePtr tile = aMemoryModel->GetMutableTile(segmentId);
            const LineMapPtr& lineMap = tile->GetLineMap();

            SQLite::Transaction transaction(*mDatabase);

            for (auto& itorLine : *lineMap)
            {
                const LinePtr& line = itorLine.second;
                const CurveListPtr& curveList = line->GetCurveList();
                const uint64_t& lineId = line->GetLineId();
                const std::string lineTextId = aMemoryModel->GetLineTextId(lineId);

                for (size_t i = 0; i < curveList->size(); i++)
                {
                    CurvePtr& curve = curveList->at(i);
                    std::string errMsg("");

                    if (line->IsEdited())
                    {
                        if (!curve->FitPointsToCurve(errMsg))
                        {
                            throw(DbParseException(DbParseException::Type::DbStoreError,
                                                   "fit curve error:" + errMsg));
                        }
                    }

                    const uint64_t& curveId = curve->GetCurveId();
                    const std::string curveTextId = aMemoryModel->GetCurveTextId(curveId);

                    sqlText = "INSERT INTO " + curveTable
                              + "(ID,LineID,IndexInLine,Type,Width,EquationDescription,Length) VALUES ("
                              + wrapText(curveTextId)
                              + wrapText(lineTextId)
                              + std::to_string(i) + ","
                              + std::to_string((int)curve->GetCurveType()) + ","
                              + std::to_string(curve->GetWidth()) + ","
                              + "'" +  (curve->Parse()) + "',"
                              + strings::FormatFloat<double>(curve->GetLength(), 8) + ")";
                    mDatabase->exec(sqlText);

                }
            }

            transaction.commit();
        }
    }
    catch(DbParseException e)
    {
        throw(e);
    }
    catch(...)
    {
        throw(DbParseException(DbParseException::Type::DbStoreError,
                                 "Curve Table Store Error with sql:" + sqlText));
    }
}

void Model::DbRepository::storeLanes(const std::shared_ptr<MemoryModel>& aMemoryModel) const
{
    std::string sqlText("");

    try
    {
        for (const std::string& laneTable : *mLaneTables)
        {
            const std::int64_t segmentId = (std::int64_t)std::stoi(strings::Split(laneTable, "_")[1]);
            TilePtr tile = aMemoryModel->GetMutableTile(segmentId);
            const LaneMapPtr& laneMap = tile->GetLaneMap();

            SQLite::Transaction transaction(*mDatabase);

            for (auto& itorLane : *laneMap)
            {
                const LanePtr& lane = itorLane.second;
                const uint64_t& laneId = lane->GetLaneId();
                const uint64_t& leftLineId = lane->GetLeftLine()->GetLineId();
                const uint64_t& rightLineId = lane->GetRightLine()->GetLineId();
                const uint64_t& avgSlamTrace = lane->GetAvgSlamTrace()->GetLineId();

                sqlText = "INSERT INTO " + laneTable
                          + "(ID,RoadID,LLineID,RLineID,AvgSlamTrace) VALUES ("
                          + wrapText(aMemoryModel->GetLaneTextId(laneId))
                          + std::to_string(lane->GetRoad()->GetRoadId()) + ","
                          + wrapText(aMemoryModel->GetLineTextId(leftLineId))
                          + wrapText(aMemoryModel->GetLineTextId(rightLineId))
                          + wrapText(aMemoryModel->GetLineTextId(avgSlamTrace), false) + ")";
                mDatabase->exec(sqlText);
            }

            transaction.commit();
        }
    }
    catch(DbParseException e)
    {
        throw(e);
    }
    catch(...)
    {
        throw(DbParseException(DbParseException::Type::DbStoreError,
                                 "Lane Table Store Error with sql:" + sqlText));
    }
}

void Model::DbRepository::storeTrafficSigns(const std::shared_ptr<MemoryModel>& aMemoryModel) const
{
    std::string sqlText("");

    try
    {
        for (const std::string& trafficSignTable : *mTrafficSignTables)
        {
            const std::int64_t segmentId = (std::int64_t)std::stoi(strings::Split(trafficSignTable, "_")[1]);
            TilePtr tile = aMemoryModel->GetMutableTile(segmentId);
            const TrafficSignMapPtr& trafficSignMap = tile->GetTrafficSignMap();

            SQLite::Transaction transaction(*mDatabase);

            for (auto& itorTrafficSign : *trafficSignMap)
            {
                const TrafficSignPtr& trafficSign = itorTrafficSign.second;

                sqlText = "INSERT INTO " + trafficSignTable
                          + "(ID,SegmentID,Type,Orientation,ShapeWidth,ShapeHeight,Confidence,GPS) VALUES ("
                          + std::to_string(trafficSign->GetTrafficSignId()) + ","
                          + std::to_string(segmentId) + ","
                          + std::to_string(trafficSign->GetTrafficSignType()) + ","
                          + strings::FormatFloat<double>(trafficSign->GetOrientation(), 8) + ","
                          + strings::FormatFloat<double>(trafficSign->GetShapeWidth(), 8) + ","
                          + strings::FormatFloat<double>(trafficSign->GetShapeHeight(), 8) + ","
                          + strings::FormatFloat<double>(trafficSign->GetConfidence(), 8) + ","
                          + formatReferencePoint(trafficSign->GetGeodeticPosition()) + ")";
                mDatabase->exec(sqlText);
            }

            transaction.commit();
        }
    }
    catch(DbParseException e)
    {
        throw(e);
    }
    catch(...)
    {
        throw(DbParseException(DbParseException::Type::DbStoreError,
                                 "Trafficsign Table Store Error with sql:" + sqlText));
    }
}

void Model::DbRepository::storeLaneConnections(const std::shared_ptr<MemoryModel>& aMemoryModel) const
{
    try
    {
        for (const std::string& laneConnectionTable : *mLaneConnectionTables)
        {
            const std::int64_t segmentId = (std::int64_t)std::stoi(strings::Split(laneConnectionTable, "_")[1]);
            TilePtr tile = aMemoryModel->GetMutableTile(segmentId);
            const LaneMapPtr& laneMap = tile->GetLaneMap();

            SQLite::Transaction transaction(*mDatabase);

            for (auto& itorLane : *laneMap)
            {
                const LanePtr& lane = itorLane.second;

                recordLaneConnection(laneConnectionTable,
                                     LaneConnectionType::LaneConnectionPredecessor,
                                     aMemoryModel->GetLaneTextId(lane->GetLaneId()),
                                     aMemoryModel->GetLaneTextId(lane->GetPredecessorLaneId()));

                recordLaneConnection(laneConnectionTable,
                                     LaneConnectionType::LaneConnectionSuccessor,
                                     aMemoryModel->GetLaneTextId(lane->GetLaneId()),
                                     aMemoryModel->GetLaneTextId(lane->GetSuccessorLaneId()));

                recordLaneConnection(laneConnectionTable,
                                     LaneConnectionType::LaneConnectionLeft,
                                     aMemoryModel->GetLaneTextId(lane->GetLaneId()),
                                     aMemoryModel->GetLaneTextId(lane->GetLeftLaneId()));

                recordLaneConnection(laneConnectionTable,
                                     LaneConnectionType::LaneConnectionRight,
                                     aMemoryModel->GetLaneTextId(lane->GetLaneId()),
                                     aMemoryModel->GetLaneTextId(lane->GetRightLaneId()));
            }

            transaction.commit();
        }
    }
    catch(DbParseException e)
    {
        throw(e);
    }
    catch(...)
    {
        throw(DbParseException(DbParseException::Type::DbStoreError,
                                 "LaneConnection Table Store Error"));
    }
}

bool Model::DbRepository::createAllTables(const std::int64_t& aTileId)
{
    std::string segmentTable = gcSegment + std::to_string(aTileId);
    mSegmentTables->push_back(segmentTable);
    mTables->push_back(segmentTable);

    try
    {
        mDatabase->exec("CREATE TABLE " + segmentTable +" ("
                        "ID INTEGER primary key,"
                        "ReferencePoint Text)");
    }
    catch(std::exception)
    {
        return false;
    }

    std::string curveTable = gcCurve + std::to_string(aTileId);
    mCurveTables->push_back(curveTable);
    mTables->push_back(curveTable);

    try
    {
        mDatabase->exec("CREATE TABLE " + curveTable +" ("
                        "ID TEXT primary key,"
                        "LineID TEXT,"
                        "IndexInLine BYTE,"
                        "Type INT,"
                        "Width INT,"
                        "EquationDescription TEXT,"
                        "Length FLOAT,"
                        "Color BYTE)");
    }
    catch(std::exception)
    {
        return false;
    }

    std::string trafficSignTable = gcTrafficSign + std::to_string(aTileId);
    mTrafficSignTables->push_back(trafficSignTable);
    mTables->push_back(trafficSignTable);

    try
    {
        mDatabase->exec("CREATE TABLE " + trafficSignTable +" ("
                        "ID UNSIGNED BIG INT primary key,"
                        "SegmentID INTEGER,"
                        "Type INTEGER,"
                        "Orientation float,"
                        "ShapeWidth float,"
                        "ShapeHeight float,"
                        "Confidence float,"
                        "GPS TEXT)");
    }
    catch(std::exception)
    {
        return false;
    }

    std::string lineTable = gcLine + std::to_string(aTileId);
    mLineTables->push_back(lineTable);
    mTables->push_back(lineTable);

    try
    {
        mDatabase->exec("CREATE TABLE " + lineTable +" ("
                        "ID TEXT,"
                        "LaneID TEXT,"
                        "Confidence float,"
                        "Length float,"
                        "primary key(ID,laneID))");
    }
    catch(std::exception)
    {
        return false;
    }

    std::string laneTable = gcLane + std::to_string(aTileId);
    mLaneTables->push_back(laneTable);
    mTables->push_back(laneTable);

    try
    {
        mDatabase->exec("CREATE TABLE " + laneTable +" ("
                        "ID TEXT primary key,"
                        "RoadID UNSIGEND BIG INT,"
                        "LLineID TEXT,"
                        "RLineID TEXT,"
                        "AvgSlamTrace TEXT"
                        "TrafficSignID TEXT"
                        "Attribute TEXT)");
    }
    catch(std::exception)
    {
        return false;
    }

    std::string laneConnectionTable = gcLaneConnection + std::to_string(aTileId);
    mLaneConnectionTables->push_back(laneConnectionTable);
    mTables->push_back(laneConnectionTable);

    try
    {
        mDatabase->exec("CREATE TABLE " + laneConnectionTable +" ("
                        "SrcID TEXT,"
                        "DstID TEXT,"
                        "Type TEXT,"
                        "primary key(SrcID,DstID))");
    }
    catch(std::exception)
    {
        return false;
    }

    std::string roadTable = gcRoad + std::to_string(aTileId);
    mRoadTables->push_back(roadTable);
    mTables->push_back(roadTable);

    try
    {
        mDatabase->exec("CREATE TABLE " + roadTable +" ("
                        "ID UNSIGNED BIG INT primary key,"
                        "SegID INTEGER,"
                        "StartJunctionID BIG INT,"
                        "EndJunction BIG INT)");
    }
    catch(std::exception)
    {
        return false;
    }

    return true;
}

std::string Model::DbRepository::formatReferencePoint(const Point3DPtr& aReferencePoint) const
{
    std::string strPoint("");

    strPoint += (strings::FormatFloat<double>(aReferencePoint->GetX(), 12) + ",");
    strPoint += (strings::FormatFloat<double>(aReferencePoint->GetY(), 12) + ",");
    strPoint += (strings::FormatFloat<double>(aReferencePoint->GetZ(), 4));

    return strPoint;
}

std::string Model::DbRepository::wrapText(const std::string& aText, bool aAppendComma) const
{
    return "\"" + aText + ((aAppendComma) ? ("\",") : ("\""));
}

void Model::DbRepository::recordLaneConnection(const std::string& aTable,
                                               const LaneConnectionType aType,
                                               const std::string& srcId,
                                               const std::string& dstId) const
{
    if (TEXT_NAN != srcId && TEXT_NAN != dstId)
    {
        std::string sqlText("");

        try
        {
            sqlText = "INSERT INTO " + aTable
                      + "(SrcID,DstID,Type) VALUES ("
                      + wrapText(srcId)
                      + wrapText(dstId)
                      + wrapText(std::to_string((std::uint8_t)aType), false) + ")";
            mDatabase->exec(sqlText);
        }
        catch(DbParseException e)
        {
            throw(e);
        }
        catch(...)
        {
            throw(DbParseException(DbParseException::Type::DbStoreError,
                                     "LaneConnection Table Store Error with sql:" + sqlText));
        }
    }
}
