/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    KMLInterpreter.cpp
 * @brief
 *******************************************************************************
 */

#include "KMLInterpreter.h"

#include <QtXml>
#include <QDomDocument>
#include <CoordinateTransform/Factory.h>
#include <sys/stat.h>

#include "../Tile.h"
#include "../Line.h"
#include "../Utilities.h"

namespace
{

const std::string gcKMLPaint = "Paint_";
const std::string gcKMLLaneBoundary = "Lane_Boundaries_";
const std::string gcKMLTrafficSign = "Traffic_Signs_";

}

Model::KMLInterpreter::KMLInterpreter(std::string aKMLFolder):
    mKMLFolder(aKMLFolder),
    mPaintFiles(std::make_shared<StringList>()),
    mLaneBoundaryFiles(std::make_shared<StringList>()),
    mTrafficSignFiles(std::make_shared<StringList>())
{

}

Model::KMLInterpreter::~KMLInterpreter()
{

}

bool Model::KMLInterpreter::TouchKMLDirectories()
{
    if (!touchKMLDirectory(mKMLFolder + "/" + gcKMLPaint))
    {
        return false;
    }

    if (!touchKMLDirectory(mKMLFolder + "/" + gcKMLLaneBoundary))
    {
        return false;
    }

    if (!touchKMLDirectory(mKMLFolder + "/" + gcKMLTrafficSign))
    {
        return false;
    }

    return true;
}

bool Model::KMLInterpreter::StorePaint(const std::shared_ptr<Tile>& aTile)
{
    const std::string& path = createKMLPath(mKMLFolder + "/" + gcKMLPaint, aTile->GetTileId(), mPaintFiles);
    QDomDocument domDocument;

    if (!createKMLFile(path, domDocument))
    {
        return false;
    }

    QDomElement root = domDocument.documentElement();
    writeGoogleEarth(root);
    QDomElement document = domDocument.createElement("Document");
    writeLaneBoundaryStyle(document, domDocument);

    QDomElement folder = domDocument.createElement("Folder");
    QDomElement nameFolder = domDocument.createElement("name");
    nameFolder.setNodeValue("Paint");
    folder.appendChild(nameFolder);

    const Model::LaneMapPtr& laneMap = aTile->GetLaneMap();

    for (auto& iterLane : *laneMap)
    {
        LanePtr lane = iterLane.second;
        writeLaneBoundaryLine(folder, domDocument, lane->GetLeftLine(), aTile);
        writeLaneBoundaryLine(folder, domDocument, lane->GetRightLine(), aTile);
    }

    document.appendChild(folder);
    root.appendChild(document);
}

bool Model::KMLInterpreter::StoreLaneBoundary(const std::shared_ptr<Tile>& aTile)
{
    const std::string& path = createKMLPath(mKMLFolder + "/" + gcKMLLaneBoundary, aTile->GetTileId(), mLaneBoundaryFiles);
    QDomDocument domDocument;

    if (!createKMLFile(path, domDocument))
    {
        return false;
    }

    QDomElement root = domDocument.documentElement();
    writeGoogleEarth(root);
    QDomElement document = domDocument.createElement("Document");
    writeLaneBoundaryStyle(document, domDocument);

    QDomElement folder = domDocument.createElement("Folder");
    QDomElement nameFolder = domDocument.createElement("name");
    nameFolder.setNodeValue("Paint");
    folder.appendChild(nameFolder);

    const Model::LaneMapPtr& laneMap = aTile->GetLaneMap();

    for (auto& iterLane : *laneMap)
    {
        LanePtr lane = iterLane.second;
        writeLaneBoundaryLine(folder, domDocument, lane->GetLeftLine(), aTile);
        writeLaneBoundaryLine(folder, domDocument, lane->GetRightLine(), aTile);
    }

    document.appendChild(folder);
    root.appendChild(document);
}

bool Model::KMLInterpreter::StoreTrafficSign(const std::shared_ptr<Tile>& aTile)
{
    const std::string& path = createKMLPath(mKMLFolder + "/" + gcKMLTrafficSign, aTile->GetTileId(), mLaneBoundaryFiles);
    QDomDocument domDocument;

    if (!createKMLFile(path, domDocument))
    {
        return false;
    }

    QDomElement root = domDocument.documentElement();
    writeGoogleEarth(root);
    writeTrafficSignStyle(root, domDocument);

    QDomElement folder = domDocument.createElement("Folder");
    QDomElement nameFolder = domDocument.createElement("name");
    nameFolder.setNodeValue("Traffic Signs");
    folder.appendChild(nameFolder);

    const TrafficSignMapPtr& trafficSignMap = aTile->GetTrafficSignMap();

    for (auto& iterTrafficSign : *trafficSignMap)
    {
        const TrafficSignPtr trafficSign = iterTrafficSign.second;
        writeTrafficSign(folder, domDocument, trafficSign, aTile);
    }
}

const std::string& Model::KMLInterpreter::createKMLPath(const std::string& aPrefix,
                                                        const int64_t& aTileId,
                                                        StringListPtr& aFiles)
{
    aFiles->push_back(mKMLFolder + "/" + aPrefix + std::to_string(aTileId));
    return aFiles->back();
}

bool Model::KMLInterpreter::createKMLFile(const std::string& aPath, QDomDocument& aDocument) const
{
    QFile file(aPath.c_str());

    if (!file.open(QFile::ReadWrite | QFile::Text))
    {
        return false;
    }

    if (!aDocument.setContent(&file, true)) {
        file.close();
        return false;
    }

    return true;
}

bool Model::KMLInterpreter::touchKMLDirectory(const std::string& aPath) const
{
    struct stat sb;

    if (0 != stat(aPath.c_str(), &sb) || !S_ISDIR(sb.st_mode))
    {
        return mkdir(aPath.c_str(), 0755);
    }

    return true;
}

void Model::KMLInterpreter::writeGoogleEarth(QDomElement& aElement)
{
    aElement.setTagName("kml");
    aElement.setAttribute("xmlns", "http://earth.google.com/kml/2.2");
}

void Model::KMLInterpreter::writePaintLine(QDomElement& aElement,
                                           QDomDocument& aDom,
                                           const std::shared_ptr<Line>& aLine,
                                           const std::shared_ptr<Tile>& aTile)
{
    CurveListPtr curves = aLine->GetCurveList();

    for (const auto& curve : *curves)
    {
        writePaint(aElement, aDom, curve, aTile);
    }
}

void Model::KMLInterpreter::writePaintStyle(QDomElement& aElement, QDomDocument& aDom)
{
    QDomElement style = aDom.createElement("Style");
    style.setAttribute("id", "RoadMark");
    QDomElement lineStyle = aDom.createElement("LineStyle");
    QDomElement color = aDom.createElement("color");
    color.setNodeValue("#ffff00ff");
    lineStyle.appendChild(color);
    style.appendChild(lineStyle);
    aElement.appendChild(style);
}

void Model::KMLInterpreter::writePaint(QDomElement& aElement,
                                       QDomDocument& aDom,
                                       const std::shared_ptr<Curve>& aCurve,
                                       const std::shared_ptr<Tile>& aTile)
{
    const static double PAINT_INTERVAL = 0.1;
    PaintListPtr paints = aCurve->CalculatePaintPointCloud(PAINT_INTERVAL);

    for (auto& points : *paints)
    {
        QDomElement placemark = aDom.createElement("Placemark");
        QDomElement name = aDom.createElement("name");
        name.setNodeValue(("Paint " + std::to_string(aCurve->GetCurveId())).c_str());
        QDomElement description = aDom.createElement("description");
        description.setNodeValue(("type=\"" + aCurve->GetCurveTypeDescriptor() = "\"").c_str());
        QDomElement styleUrl = aDom.createElement("styleUrl");
        styleUrl.setNodeValue("Paint");
        QDomElement altitudeMode = aDom.createElement("altitudeMode");
        altitudeMode.setNodeValue("absolute");
        QDomElement lineString = aDom.createElement("LineString");
        QDomElement coordinates = aDom.createElement("coordinates");

        writePaintCoordinates(coordinates, points, aTile);

        lineString.appendChild(coordinates);
        placemark.appendChild(name);
        placemark.appendChild(description);
        placemark.appendChild(styleUrl);
        placemark.appendChild(altitudeMode);
        placemark.appendChild(lineString);
        aElement.appendChild(placemark);
    }
}

void Model::KMLInterpreter::writePaintCoordinates(QDomElement& aElement,
                                                  const Point3DListPtr& aPoints,
                                                  const std::shared_ptr<Tile>& aTile)
{
    const Point3DPtr& referencePoint = aTile->GetReferencePoint();
    std::stringstream ss;

    std::shared_ptr<CRS::Factory> factory = std::make_shared<CRS::Factory>();
    std::unique_ptr<CRS::ICoordinateTransform> relativeToWgs84 =
                    factory->CreateRelativeTransform(CRS::CoordinateType::Relative,
                                                     CRS::CoordinateType::Wgs84,
                                                     referencePoint->GetX(),
                                                     referencePoint->GetY(),
                                                     referencePoint->GetZ());

    for (size_t i = 0; i < aPoints->size(); i++)
    {
        const auto& point = aPoints->at(i);
        double x = point->GetX();
        double y = point->GetY();
        double z = point->GetZ();
        relativeToWgs84->Transform(x, y, z);
        ss << strings::FormatFloat<double>(x, 10) << ",";
        ss << strings::FormatFloat<double>(y, 10) << ",";
        ss << strings::FormatFloat<double>(z, 10);

        if (aPoints->size() - 1 != i)
        {
            ss << "\n";
        }
    }

    aElement.setNodeValue(ss.str().c_str());
}

void Model::KMLInterpreter::writeLaneBoundaryLine(QDomElement& aElement,
                                                  QDomDocument& aDom,
                                                  const std::shared_ptr<Line>& aLine,
                                                  const std::shared_ptr<Tile>& aTile)
{
    CurveListPtr curves = aLine->GetCurveList();

    for (const auto& curve : *curves)
    {
        writeLaneBoundary(aElement, aDom, aLine, curve, aTile);
    }
}

void Model::KMLInterpreter::writeLaneBoundaryStyle(QDomElement& aElement, QDomDocument& aDom)
{
    QDomElement style = aDom.createElement("Style");
    style.setAttribute("id", "RoadMark");
    QDomElement lineStyle = aDom.createElement("LineStyle");
    QDomElement color = aDom.createElement("color");
    color.setNodeValue("#ffff00ff");
    lineStyle.appendChild(color);
    style.appendChild(lineStyle);
    aElement.appendChild(style);
}

void Model::KMLInterpreter::writeLaneBoundary(QDomElement& aElement,
                                              QDomDocument& aDom,
                                              const std::shared_ptr<Line>& aLine,
                                              const std::shared_ptr<Curve>& aCurve,
                                              const std::shared_ptr<Tile>& aTile)
{
    QDomElement placemark = aDom.createElement("Placemark");
    QDomElement name = aDom.createElement("name");
    name.setNodeValue(("Road Mark " + std::to_string(aCurve->GetCurveId())).c_str());
    QDomElement description = aDom.createElement("description");
    description.setNodeValue(("type=\"" + aCurve->GetCurveTypeDescriptor() + "\"," +
                              " confidence=\"" +
                              strings::FormatFloat<double>(aLine->GetConfidence(), 8) + "\"").c_str());
    QDomElement styleUrl = aDom.createElement("styleUrl");
    styleUrl.setNodeValue("RoadMark");
    QDomElement altitudeMode = aDom.createElement("altitudeMode");
    altitudeMode.setNodeValue("absolute");
    QDomElement lineString = aDom.createElement("LineString");
    QDomElement coordinates = aDom.createElement("coordinates");

    writeLaneBoundaryCoordinates(coordinates, aCurve, aTile);
    lineString.appendChild(coordinates);
    placemark.appendChild(name);
    placemark.appendChild(description);
    placemark.appendChild(styleUrl);
    placemark.appendChild(altitudeMode);
    placemark.appendChild(lineString);
    aElement.appendChild(placemark);
}

void Model::KMLInterpreter::writeLaneBoundaryCoordinates(QDomElement& aElement,
                                                         const std::shared_ptr<Curve>& aCurve,
                                                         const std::shared_ptr<Tile>& aTile)
{
    const static double LANE_BOUNDARY_INTERVAL = 0.1;
    Point3DListPtr points = aCurve->CalculatePointCloud(LANE_BOUNDARY_INTERVAL);
    const Point3DPtr& referencePoint = aTile->GetReferencePoint();
    std::stringstream ss;

    std::shared_ptr<CRS::Factory> factory = std::make_shared<CRS::Factory>();
    std::unique_ptr<CRS::ICoordinateTransform> relativeToWgs84 =
                    factory->CreateRelativeTransform(CRS::CoordinateType::Relative,
                                                     CRS::CoordinateType::Wgs84,
                                                     referencePoint->GetX(),
                                                     referencePoint->GetY(),
                                                     referencePoint->GetZ());

    for (size_t i = 0; i < points->size(); i++)
    {
        const auto& point = points->at(i);
        double x = point->GetX();
        double y = point->GetY();
        double z = point->GetZ();
        relativeToWgs84->Transform(x, y, z);
        ss << strings::FormatFloat<double>(x, 10) << ",";
        ss << strings::FormatFloat<double>(y, 10) << ",";
        ss << strings::FormatFloat<double>(z, 10);

        if (points->size() - 1 != i)
        {
            ss << "\n";
        }
    }

    aElement.setNodeValue(ss.str().c_str());
}

void Model::KMLInterpreter::writeTrafficSignStyle(QDomElement& aElement, QDomDocument& aDom)
{
    writeTrafficSignStyleCircle(aElement, aDom);
    writeTrafficSignStyleCircleHighlight(aElement, aDom);
    writeTrafficSignStyleMap(aElement, aDom);
}

void Model::KMLInterpreter::writeTrafficSignStyleCircle(QDomElement& aElement, QDomDocument& aDom)
{
    QDomElement style = aDom.createElement("Style");
    style.setAttribute("id", "sn_placemark_circle");
    QDomElement iconStyle = aDom.createElement("IconStyle");
    QDomElement color = aDom.createElement("color");
    color.setNodeValue("ff00ffff");
    QDomElement scale = aDom.createElement("scale");
    scale.setNodeValue("0.5");
    QDomElement icon = aDom.createElement("Icon");
    QDomElement href = aDom.createElement("href");
    href.setNodeValue("http://maps.google.com/mapfiles/kml/shapes/shaded_dot.png");
    iconStyle.appendChild(href);
    iconStyle.appendChild(icon);
    iconStyle.appendChild(scale);
    iconStyle.appendChild(color);
    style.appendChild(iconStyle);
    aElement.appendChild(style);
}

void Model::KMLInterpreter::writeTrafficSignStyleCircleHighlight(QDomElement& aElement, QDomDocument& aDom)
{
    QDomElement style = aDom.createElement("Style");
    style.setAttribute("id", "sn_placemark_circle_highlight");
    QDomElement iconStyle = aDom.createElement("IconStyle");
    QDomElement color = aDom.createElement("color");
    color.setNodeValue("ffff0000");
    QDomElement scale = aDom.createElement("scale");
    scale.setNodeValue("0.5");
    QDomElement icon = aDom.createElement("Icon");
    QDomElement href = aDom.createElement("href");
    href.setNodeValue("http://maps.google.com/mapfiles/kml/shapes/shaded_dot.png");
    iconStyle.appendChild(href);
    iconStyle.appendChild(icon);
    iconStyle.appendChild(scale);
    iconStyle.appendChild(color);
    style.appendChild(iconStyle);
    aElement.appendChild(style);
}

void Model::KMLInterpreter::writeTrafficSignStyleMap(QDomElement& aElement, QDomDocument& aDom)
{
    QDomElement styleMap = aDom.createElement("StyleMap");
    styleMap.setAttribute("id", "TrafficSign");

    {
       QDomElement pair = aDom.createElement("Pair");
       QDomElement key = aDom.createElement("key");
       key.setNodeValue("normal");
       QDomElement styleUrl = aDom.createElement("styleUrl");
       key.setNodeValue("#sn_placemark_circle");
       pair.appendChild(styleUrl);
       pair.appendChild(key);
       styleMap.appendChild(pair);
    }
    {
        QDomElement pair = aDom.createElement("Pair");
        QDomElement key = aDom.createElement("key");
        key.setNodeValue("highlight");
        QDomElement styleUrl = aDom.createElement("styleUrl");
        key.setNodeValue("#sn_placemark_circle_highlight");
        pair.appendChild(styleUrl);
        pair.appendChild(key);
        styleMap.appendChild(pair);
    }

    aElement.appendChild(styleMap);
}

void Model::KMLInterpreter::writeTrafficSign(QDomElement& aElement,
                                             QDomDocument& aDom,
                                             const std::shared_ptr<TrafficSign> &aTrafficSign,
                                             const std::shared_ptr<Tile>& aTile)
{
    QDomElement placemark = aDom.createElement("Placemark");
    QDomElement name = aDom.createElement("name");
    name.setNodeValue(("Traffic Sign " + std::to_string(aTrafficSign->GetTrafficSignId())).c_str());
    QDomElement description = aDom.createElement("description");
    writeTrafficSignDescription(description, aTrafficSign);
    QDomElement altitudeMode = aDom.createElement("altitudeMode");
    altitudeMode.setNodeValue("absolute");
    QDomElement styleUrl = aDom.createElement("styleUrl");
    styleUrl.setNodeValue("TrafficSign");
    QDomElement point = aDom.createElement("Point");
    QDomElement drawOrder = aDom.createElement("drawOrder");
    drawOrder.setNodeValue("1");
    QDomElement coordinates = aDom.createElement("coordinates");
    writeTrafficSignCoordinates(coordinates, aTrafficSign);
    point.appendChild(coordinates);
    point.appendChild(drawOrder);
    placemark.appendChild(point);
    placemark.appendChild(styleUrl);
    placemark.appendChild(altitudeMode);
    placemark.appendChild(description);
    placemark.appendChild(name);
    aElement.appendChild(placemark);
}

void Model::KMLInterpreter::writeTrafficSignDescription(QDomElement& aElement,
                                                        const std::shared_ptr<TrafficSign> &aTrafficSign)
{
    std::stringstream ss;
    ss << "type=\"" << std::to_string(aTrafficSign->GetTrafficSignType()) << "\",";
    ss << "height=\"" << strings::FormatFloat<double>(aTrafficSign->GetShapeHeight(), 6) << "\",";
    ss << "width=\"" << strings::FormatFloat<double>(aTrafficSign->GetShapeWidth(), 6) << "\",";
    ss << "orientation=\"" << strings::FormatFloat<double>(aTrafficSign->GetOrientation(), 6) << "\",";
    ss << "confidence=\"" << strings::FormatFloat<double>(aTrafficSign->GetConfidence(), 6) << "\",";
    aElement.setNodeValue(ss.str().c_str());
}

void Model::KMLInterpreter::writeTrafficSignCoordinates(QDomElement& aElement,
                                                        const std::shared_ptr<TrafficSign> &aTrafficSign)
{
    std::stringstream ss;
    ss  << strings::FormatFloat<double>(aTrafficSign->GetPosition()->GetX(), 15) << ",";
    ss  << strings::FormatFloat<double>(aTrafficSign->GetPosition()->GetY(), 15) << ",";
    ss  << strings::FormatFloat<double>(aTrafficSign->GetPosition()->GetZ(), 15);
    aElement.setNodeValue(ss.str().c_str());
}
