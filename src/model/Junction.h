/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Junction.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Common.h"
#include "Element.h"
#include "Point3D.h"
#include "ConnectionRelation.h"
#include "Line.h"

namespace Model
{

class Junction : public Element
{
public:
    Junction();
    Junction(const std::uint64_t& aJunctionId);
    ~Junction();

    const std::uint64_t& GetJunctionId() const;
    void SetJunctionId(const std::uint64_t& aJunctionId);

    const Point3DListPtr& GetGeodeticPoints() const;
    Point3DListPtr GetMutableGeodeticPoints();
    void SetGeodeticPoints(const Point3DListPtr& aGeodeticPoints);
    void CreateGeodeticPoints(const Point3DPtr&, const double& );

    const ViewPointMapPtr& GetPointListMap() const;
    ViewPointMapPtr GetMutablePointListMap();
    Point3DListPtr GetPointListByLevel(uint8_t aLevel);
    Point3DListPtr GetMutablePointListByLevel(uint8_t aLevel);

    const LineListPtr& GetBorderLineList() const;
    LineListPtr GetMutableBorderLineList();
    size_t GetBorderLineListSize() const;
    LinePtr GetBorderLine(const size_t& aIndex);

    const ConnectionRelationListPtr& GetConnectionRelationList() const;
    ConnectionRelationListPtr GetMutableConnectionRelationList();
    size_t GetConnectionRelationListSize() const;
    ConnectionRelationPtr GetConnectionRelation(const size_t& aIndex);

private:
    std::uint64_t mJunctionId;
    Point3DListPtr mGeodeticPoints;
    ViewPointMapPtr mPointListMap;
    LineListPtr mBorderLineList;
    ConnectionRelationListPtr mConnectionRelationList;
};

typedef std::shared_ptr<Junction> JunctionPtr;
typedef std::shared_ptr<const Junction> JunctionConstPtr;
typedef std::vector<JunctionPtr> JunctionList;
typedef std::shared_ptr<JunctionList> JunctionListPtr;
typedef std::shared_ptr<const JunctionList> JunctionListConstPtr;

typedef std::unordered_map<std::uint64_t, JunctionPtr> JunctionMap;
typedef std::shared_ptr<JunctionMap> JunctionMapPtr;
typedef std::shared_ptr<const JunctionMap> JunctionMapConstPtr;

}
