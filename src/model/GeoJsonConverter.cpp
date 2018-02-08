/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    GeoJsonConverter.cpp
 * @brief
 *******************************************************************************
 */

#include "GeoJsonConverter.h"

#include <QtGlobal>
#include <QVector>

#include <map>

#include "CoordinateTransform/Factory.h"
#include "model/MemoryModel.h"

static const std::map<Model::CurveType, QString> scLineTypeMap =
{
    {Model::CurveType::UnKnown, "unknown"},
    {Model::CurveType::UnDefined, "undefined"},
    {Model::CurveType::Dashed, "dash"},
    {Model::CurveType::Solid, "solid"},
};

Model::GeoJsonConverter::GeoJsonConverter()
{
}

QJsonArray Model::GeoJsonConverter::Convert(const osg::Matrixd &aMatrix)
{
    QJsonArray matrixArray = {aMatrix(0, 0), aMatrix(0, 1), aMatrix(0, 2), aMatrix(0, 3),
                              aMatrix(1, 0), aMatrix(1, 1), aMatrix(1, 2), aMatrix(1, 3),
                              aMatrix(2, 0), aMatrix(2, 1), aMatrix(2, 2), aMatrix(2, 3),
                              aMatrix(3, 0), aMatrix(3, 1), aMatrix(3, 2), aMatrix(3, 3)};
    return matrixArray;
}

QJsonArray Model::GeoJsonConverter::Convert(int aLevel, TileConstPtr& aTile)
{
    QJsonArray roadArray;
    for (const auto& itor : *(aTile->GetRoadMap()))
    {
        QJsonObject roadObj;
        roadObj = Convert(aLevel, itor.second);
        roadArray.push_back(roadObj);
    }

    return roadArray;
}

void Model::GeoJsonConverter::addLine(QJsonObject& aLane, QVector<quint64>& aLineIdVec, QJsonArray& aLines,
                                      const QString& aPos, const Model::LinePtr& aLine, int aLevel)
{
    if (aLine)
    {
        quint64 lineId = aLine->GetLineId();
        aLane[aPos] = static_cast<double>(lineId);
        if (aLineIdVec.count(lineId) == 0)
        {
            QJsonObject line = convert(aLevel, aLine);
            aLineIdVec.append(lineId);
            aLines.push_back(line);
        }
    }
}

QJsonObject Model::GeoJsonConverter::getTileBound(const Model::TilePtr& aTile)
{
    QJsonObject tileObj;
    tileObj["id"] = static_cast<double>(aTile->GetTileId());
    const Point3DPtr& refPt = aTile->GetReferencePoint();
    double ut = 180.0 / (1 << 13);
    QJsonArray leftBottom({refPt->GetX(), refPt->GetY()});
    QJsonArray rightTop({refPt->GetX() + ut, refPt->GetY() + ut});
    tileObj["extent"] = QJsonArray({leftBottom, rightTop});

    return tileObj;
}

QJsonArray Model::GeoJsonConverter::GetTileExtent(const std::shared_ptr<Model::MemoryModel>& aMemoryModel)
{
    QJsonArray tileExtent;
    const TileMapPtr& tileMap = aMemoryModel->GetTileMap();

    for (const auto& tile : (*tileMap))
    {
        tileExtent.append(getTileBound(tile.second));
    }

    return tileExtent;
}

QJsonObject Model::GeoJsonConverter::Convert(int aLevel, const Model::RoadPtr& aRoad)
{
    QJsonObject roadObj;
    QVector<quint64> lineIds;
    QJsonArray lines;
    QJsonObject roadData;
    QJsonArray lanes;

    roadObj["type"] = "road";
    roadData["id"] = static_cast<double>(aRoad->GetRoadId());
    roadData["ndsId"] = static_cast<double>(aRoad->GetTile()->GetTileId());

    for (const auto& it : *(aRoad->GetLaneList()))
    {
        QJsonArray connectionMap;
        QJsonObject lane;

        lane["id"] = static_cast<double>(it->GetLaneId());
        addLine(lane, lineIds, lines, "left", it->GetLeftLine(), aLevel);
        addLine(lane, lineIds, lines, "right", it->GetRightLine(), aLevel);
        addLine(lane, lineIds, lines, "center", it->GetCenterLine(), aLevel);

        for (const auto& con : *(it->GetConnectionMap()))
        {
            connectionMap.push_back(QJsonArray({con.first, static_cast<double>(con.second)}));
        }

        lane["connectionMap"] = connectionMap;
        lanes.push_back(lane);
    }

    roadData["lanes"] = lanes;
    roadObj["data"] = roadData;
    roadObj["lines"] = lines;
    return roadObj;
}

QJsonObject Model::GeoJsonConverter::convert(int aLevel, const Model::LinePtr& aLine)
{
    QJsonObject lineObj;
    if (aLine != nullptr)
    {
        lineObj["id"] = static_cast<double>(aLine->GetLineId());
        lineObj["type"] = scLineTypeMap.at(aLine->GetLineType());
        lineObj["length"] = aLine->GetLength();
        if (aLevel > 0)
        {
            lineObj["points"] = convert(aLine->GetPaintListByLevel(aLevel), true);
        }
        else
        {
            lineObj["points"] = convert(aLine->GetGeodeticPointsList(), false);
        }
    }

    return lineObj;
}

QJsonArray Model::GeoJsonConverter::convert(const Model::PaintListPtr& aPaintList, bool aIsRelative)
{
    // Convert point form ECEF to WGS84
    QJsonArray pointListArray;

    if (aPaintList != nullptr)
    {
        std::shared_ptr<CRS::Factory> factory = std::make_shared<CRS::Factory>();
        std::unique_ptr<CRS::ICoordinateTransform> ecefToWgs84 =
                        factory->CreateEcefProjection(CRS::CoordinateType::Ecef,
                                                      CRS::CoordinateType::Wgs84);

        for (const auto& pointVec : *aPaintList)
        {
            QJsonArray pointList;

            for (const auto& point : *pointVec)
            {
                double x = point->GetX();
                double y = point->GetY();
                double z = point->GetZ();
                if (aIsRelative)
                {
                    ecefToWgs84->Transform(x, y, z);
                }
                pointList.push_back(QJsonArray({x, y, z}));
            }

            pointListArray.push_back(pointList);
        }
    }

    return pointListArray;
}
