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

class Junction : public Element
{
public:
    Junction();
    Junction(std::uint64_t aJunctionId);
    ~Junction();

    std::uint64_t GetJunctionId() const;
    void SetJunctionId(std::uint64_t aJunctionId);

    const std::unordered_map<uint8_t, std::vector<Point3D>>& GetPointListMap() const;
    std::unordered_map<uint8_t, std::vector<Point3D>>* GetMutablePointListMap();
    std::vector<Point3D>* GetPointListByLevel(uint8_t aLevel);

    const std::vector<std::shared_ptr<Line>>& GetBorderLineList() const;
    std::vector<std::shared_ptr<Line>>* GetMutableBorderLineList();
    size_t GetBorderLineListSize() const;
    std::shared_ptr<Line> GetBorderLine(size_t aIndex);

    const std::vector<std::shared_ptr<ConnectionRelation>>& GetConnectionRelationList() const;
    std::vector<std::shared_ptr<ConnectionRelation>>* GetMutableConnectionRelationList();
    size_t GetConnectionRelationListSize() const;
    std::shared_ptr<ConnectionRelation> GetConnectionRelation(size_t aIndex);


private:
    std::uint64_t mJunctionId;
    std::unordered_map<uint8_t, std::vector<Point3D>> mPointListMap;
    std::vector<std::shared_ptr<Line>> mBorderLineList;
    std::vector<std::shared_ptr<ConnectionRelation>> mConnectionRelationList;
};
