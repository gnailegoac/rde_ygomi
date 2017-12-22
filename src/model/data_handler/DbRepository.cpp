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

}

Model::DbRepository::DbRepository(const std::string& aDbFileName):
    mDbFileName(aDbFileName),
    mTables(std::make_shared<StringList>()),
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
                                    + " WHERE Type in ('0','1','2')";

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

                    std::uint64_t lineId = aMemoryModel->GetLineIntId(query.getColumn(1).getString());
                    LinePtr line = tile->GetMutableLine(lineId);
                    line->GetMutableCurveList()->push_back(nurbsCurve);
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
                int type = std::stoi(query.getColumn(2).getString());

                if (0 == type)
                {
                    setLaneConnection(destinationTile, destinationLaneId, sourceTile, sourceLaneId);
                }

                if (1 == type)
                {
                    setLaneConnection(sourceTile, sourceLaneId, destinationTile, destinationLaneId);
                }
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
        const std::string QUERY_LANE = "SELECT ID,LLineID,RLineID FROM " + laneTable;
        const std::int64_t segmentId = (std::int64_t)std::stoi(strings::Split(laneTable, "_")[1]);
        TilePtr tile = aMemoryModel->GetMutableTile(segmentId);

        try
        {
            SQLite::Statement query(*mDatabase, QUERY_LANE);
            while (query.executeStep())
            {
                std::uint64_t leftLineId = aMemoryModel->GetLineIntId(query.getColumn(1).getString());
                std::uint64_t rightLineId = aMemoryModel->GetLineIntId(query.getColumn(2).getString());
                LinePtr leftLine = tile->GetMutableLine(leftLineId);
                LinePtr rightLine = tile->GetMutableLine(rightLineId);

                std::uint64_t laneId = aMemoryModel->GetLaneIntId(query.getColumn(0).getString());
                LanePtr lane = tile->GetMutableLane(laneId);
                lane->SetLeftLine(leftLine);
                lane->SetRightLine(rightLine);
                leftLine->SetLane(lane);
                rightLine->SetLane(lane);

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

void Model::DbRepository::queryTrafficSigns(std::shared_ptr<MemoryModel>& aMemoryModel) const
{
    for (const std::string& trafficSignTable : *mTrafficSignTables)
    {
        const std::int64_t segmentId = (std::int64_t)std::stoi(strings::Split(trafficSignTable, "_")[1]);
        TilePtr tile = aMemoryModel->GetMutableTile(segmentId);
        const std::string QUERY_TRAFFIC_SIGN = "SELECT ID,Type,Gps FROM "
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
                trafficSign->SetPosition(parsePoint(query.getColumn(7).getString()));
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

void Model::DbRepository::setLaneConnection(const TilePtr& aPredecessorTile,
                                            const std::uint64_t& aPredecessorId,
                                            const TilePtr& aSuccessorTile,
                                            const std::uint64_t& aSuccessorId) const
{
    LanePtr predecessorLane = nullptr;
    LanePtr successorLane = nullptr;

    if (nullptr == aPredecessorTile ||
            nullptr == (predecessorLane = aPredecessorTile->GetLane(aPredecessorId)))
    {
        throw(DbParseException(DbParseException::Type::DbParseError,
                               "Cannot Find the Corresponding Segment with Lane " + aPredecessorId));
    }

    if (nullptr == aSuccessorTile ||
            nullptr == (successorLane = aSuccessorTile->GetLane(aSuccessorId)))
    {
        throw(DbParseException(DbParseException::Type::DbParseError,
                               "Cannot Find the Corresponding Segment with Lane " + aSuccessorId));
    }

    if (aPredecessorId != aSuccessorId)
    {
        predecessorLane->SetSuccessorLaneId(aSuccessorId);
        successorLane->SetPredecessorLaneId(aPredecessorId);
    }
}
