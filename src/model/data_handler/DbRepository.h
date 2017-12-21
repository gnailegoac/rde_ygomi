/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DbRepository.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "../Common.h"

namespace SQLite
{

class Database;

}


namespace Model
{

class MemoryModel;
class Point3D;
class Tile;

typedef std::shared_ptr<Point3D> Point3DPtr;
typedef std::shared_ptr<Tile> TilePtr;

class DbRepository
{
public:
    DbRepository(const std::string& aDbFileName);
    ~DbRepository();

    bool InitializeDataBase();
    bool QueryRoadElement(std::shared_ptr<MemoryModel>& aMemoryModel);
    bool QueryTopology(std::shared_ptr<MemoryModel>& aMemoryModel);

private:
    void querySegments(std::shared_ptr<MemoryModel>& aMemoryModel) const;
    void queryCurves(std::shared_ptr<MemoryModel>& aMemoryModel) const;
    void queryLaneConnection(std::shared_ptr<MemoryModel>& aMemoryModel) const;
    void queryLanes(std::shared_ptr<MemoryModel>& aMemoryModel) const;
    void queryTrafficSigns(std::shared_ptr<MemoryModel>& aMemoryModel) const;

    bool queryAllTables();
    Point3DPtr parsePoint(const std::string& aPointString) const;
    void setLaneConnection(const TilePtr& aPredecessorTile,
                           const std::uint64_t& aPredecessorId,
                           const TilePtr& aSuccessorTile,
                           const std::uint64_t& aSuccessorId) const;

private:
    std::string mDbFileName;
    StringListPtr mTables;
    StringListPtr mSegmentTables;
    StringListPtr mLaneTables;
    StringListPtr mLaneConnectionTables;
    StringListPtr mLineTables;
    StringListPtr mCurveTables;
    StringListPtr mTrafficSignTables;
    std::shared_ptr<SQLite::Database> mDatabase;
};

typedef std::shared_ptr<DbRepository> DbRepositoryPtr;
typedef std::shared_ptr<const DbRepository> DbRepositoryConstPtr;

}

