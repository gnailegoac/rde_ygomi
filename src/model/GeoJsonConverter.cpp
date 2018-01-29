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

#include "CoordinateTransform/Factory.h"
#include "model/MemoryModel.h"

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
        QVector<quint64> lineIds;
        QJsonObject road;
        QJsonArray lines;
        RoadPtr roadPtr = itor.second;
        road["type"] = "road";
        QJsonObject roadData;
        roadData["id"] = static_cast<double>(roadPtr->GetRoadId());
        roadData["ndsId"] = static_cast<double>(aTile->GetTileId());
        QJsonArray lanes;

        for (const auto& it : *(roadPtr->GetLaneList()))
        {
            QJsonObject lane;
            lane["id"] = static_cast<double>(it->GetLaneId());
            LinePtr leftLine = it->GetLeftLine();
            if (leftLine != nullptr)
            {
                quint64 leftLineId = leftLine->GetLineId();
                lane["left"] = static_cast<double>(leftLineId);
                if (lineIds.count(leftLineId) == 0)
                {
                    QJsonObject line = convert(aLevel, leftLine);
                    lineIds.push_back(leftLineId);
                    lines.push_back(line);
                }
            }

            LinePtr rightLine = it->GetRightLine();
            if (rightLine != nullptr)
            {
                quint64 rightLineId = rightLine->GetLineId();
                lane["right"] = static_cast<double>(rightLineId);
                if (lineIds.count(rightLineId) == 0)
                {
                    QJsonObject line = convert(aLevel, rightLine);
                    lineIds.push_back(rightLineId);
                    lines.push_back(line);
                }
            }

            LinePtr centerLine = it->GetCenterLine();
            if (centerLine != nullptr)
            {
                quint64 centerLineId = centerLine->GetLineId();
                lane["center"] = static_cast<double>(centerLineId);
                if (lineIds.count(centerLineId) == 0)
                {
                    QJsonObject line = convert(aLevel, centerLine);
                    lineIds.append(centerLineId);
                    lines.push_back(line);
                }
            }

            QJsonArray connectionMap;
            for (const auto& con : *(it->GetConnectionMap()))
            {
                connectionMap.push_back(QJsonArray({con.first, static_cast<double>(con.second)}));
            }
            lane["connectionMap"] = connectionMap;
            lanes.push_back(lane);
        }
        roadData["lanes"] = lanes;
        road["data"] = roadData;
        road["lines"] = lines;
        roadArray.push_back(road);
    }
    return roadArray;
}

QJsonObject Model::GeoJsonConverter::getTileBound(const Model::TilePtr& aTile)
{
    QJsonObject tileObj;
    tileObj["id"] = static_cast<double>(aTile->GetTileId());
    const Point3DPtr& refPt = aTile->GetReferencePoint();
    double ut = 180.0 / std::pow(2, 13);
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

QJsonObject Model::GeoJsonConverter::convert(int aLevel, const Model::LinePtr& aLine)
{
    QJsonObject lineObj;
    if (aLine != nullptr)
    {
        lineObj["id"] = qint64(aLine->GetLineId());
        switch (aLine->GetCurve(0)->GetCurveType())
        {
        case CurveType::Solid:
//            lineObj["type"] = "solid";
//            break;
        case CurveType::Dashed:
            lineObj["type"] = "dash";
            break;
        case CurveType::Imputed:
        case CurveType::SlamTrace:
        case CurveType::UnDefined:
        case CurveType::UnKnown:
        default:
            break;
        }
        lineObj["length"] = aLine->GetLength();
        lineObj["points"] = convert(aLine->GetPaintListByLevel(aLevel));
    }
    return lineObj;
}

QJsonArray Model::GeoJsonConverter::convert(const Model::PaintListPtr& aPaintList)
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
                ecefToWgs84->Transform(x, y, z);
                pointList.push_back(QJsonArray({x, y, z}));
            }
//            if (aPaintList->size() == 1)
//            {
//                // for solid line, the JSON content should be an array of points.
//                return pointList;
//            }
            pointListArray.push_back(pointList);
        }
    }
    return pointListArray;
}
