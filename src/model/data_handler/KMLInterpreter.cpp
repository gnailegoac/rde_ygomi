/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
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

Model::KMLInterpreter::KMLInterpreter(std::string aKMLFolder, double aOutputInterval):
    mOutputInterval(aOutputInterval),
    mPaintFolder(aKMLFolder + "/" + gcKMLPaint),
    mLaneBoundaryFolder(aKMLFolder + "/" + gcKMLLaneBoundary),
    mTrafficSignFolder(aKMLFolder + "/" + gcKMLTrafficSign),
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
    if (!touchKMLDirectory(mPaintFolder))
    {
        qCritical(("create folder " + mPaintFolder + " failed").c_str());
        return false;
    }

    if (!touchKMLDirectory(mLaneBoundaryFolder))
    {
        qCritical(("create folder " + mLaneBoundaryFolder + " failed").c_str());
        return false;
    }

    if (!touchKMLDirectory(mTrafficSignFolder))
    {
        qCritical(("create folder " + mTrafficSignFolder + " failed").c_str());
        return false;
    }

    return true;
}

bool Model::KMLInterpreter::SaveKMLPaint(const std::shared_ptr<Tile>& aTile)
{
    QDomDocument domDocument;
    writeInstructions(domDocument);

    QDomElement kml = domDocument.createElement("kml");
    writeGoogleEarth(kml);

    QDomElement document = domDocument.createElement("Document");
    writeLaneBoundaryStyle(document, domDocument);

    QDomElement nameFolder = domDocument.createElement("name");
    QDomText paint = domDocument.createTextNode("Paint");
    nameFolder.appendChild(paint);

    QDomElement folder = domDocument.createElement("Folder");
    folder.appendChild(nameFolder);

    const Model::LaneMapPtr& laneMap = aTile->GetLaneMap();

    for (auto& iterLane : *laneMap)
    {
        LanePtr lane = iterLane.second;
        writePaintLine(folder, domDocument, lane->GetLeftLine(), aTile);
        writePaintLine(folder, domDocument, lane->GetRightLine(), aTile);
    }

    document.appendChild(folder);
    kml.appendChild(document);
    domDocument.appendChild(kml);

    const std::string& path = createKMLPath(mPaintFolder, aTile->GetTileId(), mPaintFiles);
    return saveKMLFile(path, domDocument);
}

bool Model::KMLInterpreter::SaveKMLLaneBoundary(const std::shared_ptr<Tile>& aTile)
{
    QDomDocument domDocument;
    writeInstructions(domDocument);

    QDomElement kml = domDocument.createElement("kml");
    writeGoogleEarth(kml);

    QDomElement document = domDocument.createElement("Document");
    writeLaneBoundaryStyle(document, domDocument);

    QDomElement nameFolder = domDocument.createElement("name");
    QDomText paint = domDocument.createTextNode("Road Marks");
    nameFolder.appendChild(paint);

    QDomElement folder = domDocument.createElement("Folder");
    folder.appendChild(nameFolder);

    const Model::LaneMapPtr& laneMap = aTile->GetLaneMap();

    for (auto& iterLane : *laneMap)
    {
        LanePtr lane = iterLane.second;
        writeLaneBoundaryLine(folder, domDocument, lane->GetLeftLine(), aTile);
        writeLaneBoundaryLine(folder, domDocument, lane->GetRightLine(), aTile);
    }

    document.appendChild(folder);
    kml.appendChild(document);
    domDocument.appendChild(kml);

    const std::string& path = createKMLPath(mLaneBoundaryFolder, aTile->GetTileId(), mLaneBoundaryFiles);
    return saveKMLFile(path, domDocument);
}

bool Model::KMLInterpreter::SaveKMLTrafficSign(const std::shared_ptr<Tile>& aTile)
{
    QDomDocument domDocument;
    writeInstructions(domDocument);

    QDomElement kml = domDocument.createElement("kml");
    writeGoogleEarth(kml);

    QDomElement document = domDocument.createElement("Document");
    writeTrafficSignStyle(document, domDocument);

    QDomElement nameFolder = domDocument.createElement("name");
    QDomText trafficSign = domDocument.createTextNode("Traffic Signs");
    nameFolder.appendChild(trafficSign);

    QDomElement folder = domDocument.createElement("Folder");
    folder.appendChild(nameFolder);

    const TrafficSignMapPtr& trafficSignMap = aTile->GetTrafficSignMap();

    for (auto& iterTrafficSign : *trafficSignMap)
    {
        const TrafficSignPtr& trafficSign = iterTrafficSign.second;
        writeTrafficSign(folder, domDocument, trafficSign, aTile);
    }

    document.appendChild(folder);
    kml.appendChild(document);
    domDocument.appendChild(kml);

    const std::string& path = createKMLPath(mTrafficSignFolder, aTile->GetTileId(), mTrafficSignFiles);
    return saveKMLFile(path, domDocument);
}

const std::string& Model::KMLInterpreter::createKMLPath(const std::string& aPrefix,
                                                        const int64_t& aTileId,
                                                        StringListPtr& aFiles)
{
    aFiles->push_back(aPrefix + "/" + std::to_string(aTileId) + ".kml");
    return aFiles->back();
}

bool Model::KMLInterpreter::saveKMLFile(const std::string& aPath, QDomDocument& aDocument) const
{
    QFile file(QString::fromStdString(aPath));

    try
    {
        if(!file.open(QIODevice::ReadWrite))
        {
            return false;
        }

        QTextStream out(&file);
        aDocument.save(out,4);
        file.close();
    }
    catch(...)
    {
        file.close();
        return false;
    }

    return true;
}

bool Model::KMLInterpreter::touchKMLDirectory(const std::string& aPath) const
{
    struct stat sb;

    try
    {
        if (0 != stat(aPath.c_str(), &sb) || !S_ISDIR(sb.st_mode))
        {
            return (0 == mkdir(aPath.c_str(), 0755));
        }
    }
    catch(...)
    {
        return false;
    }

    return true;
}

void Model::KMLInterpreter::writeInstructions(QDomDocument& aDom)
{
    QDomProcessingInstruction instruction;
    instruction = aDom.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    aDom.appendChild(instruction);
}

void Model::KMLInterpreter::writeGoogleEarth(QDomElement& aElement)
{
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
    QDomText colorDescriptor = aDom.createTextNode("#ffff00ff");
    color.appendChild(colorDescriptor);
    lineStyle.appendChild(color);
    style.appendChild(lineStyle);
    aElement.appendChild(style);
}

void Model::KMLInterpreter::writePaint(QDomElement& aElement,
                                       QDomDocument& aDom,
                                       const std::shared_ptr<Curve>& aCurve,
                                       const std::shared_ptr<Tile>& aTile)
{
    PaintListPtr paints = aCurve->CalculatePaintPointCloud(mOutputInterval);

    for (auto& points : *paints)
    {
        QDomElement placemark = aDom.createElement("Placemark");
        QDomElement name = aDom.createElement("name");
        QDomText paintId = aDom.createTextNode(("Paint " + std::to_string(aCurve->GetCurveId())).c_str());
        name.appendChild(paintId);
        QDomElement description = aDom.createElement("description");
        QDomText paintType = aDom.createTextNode(("type=\"" + aCurve->GetCurveTypeDescriptor() = "\"").c_str());
        description.appendChild(paintType);
        QDomElement styleUrl = aDom.createElement("styleUrl");
        QDomText styleUrlValue = aDom.createTextNode("Paint");
        styleUrl.appendChild(styleUrlValue);
        QDomElement altitudeMode = aDom.createElement("altitudeMode");
        QDomText altitudeModeValue = aDom.createTextNode("absolute");
        altitudeMode.appendChild(altitudeModeValue);
        QDomElement lineString = aDom.createElement("LineString");
        QDomElement coordinates = aDom.createElement("coordinates");
        writePaintCoordinates(coordinates, aDom, points, aTile);
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
                                                  QDomDocument& aDom,
                                                  const Point3DListPtr& aPoints,
                                                  const std::shared_ptr<Tile>& aTile)
{
    const Point3DPtr& referencePoint = aTile->GetReferencePoint();
    std::stringstream ss;
    size_t pointSize = aPoints->size();

    std::shared_ptr<CRS::Factory> factory = std::make_shared<CRS::Factory>();
    std::unique_ptr<CRS::ICoordinateTransform> relativeToWgs84 =
                    factory->CreateRelativeTransform(CRS::CoordinateType::Relative,
                                                     CRS::CoordinateType::Wgs84,
                                                     referencePoint->GetX(),
                                                     referencePoint->GetY(),
                                                     referencePoint->GetZ());

    for (size_t i = 0; i < pointSize; i++)
    {
        const auto& point = aPoints->at(i);
        double x = point->GetX();
        double y = point->GetY();
        double z = point->GetZ();
        relativeToWgs84->Transform(x, y, z);
        ss << strings::FormatFloat<double>(x, 10) << ",";
        ss << strings::FormatFloat<double>(y, 10) << ",";
        ss << strings::FormatFloat<double>(z, 10);

        if (pointSize - 1 != i)
        {
            ss << "\n";
        }
    }

    QDomText coordinates = aDom.createTextNode(ss.str().c_str());
    aElement.appendChild(coordinates);
}

void Model::KMLInterpreter::writeLaneBoundaryLine(QDomElement& aElement,
                                                  QDomDocument& aDom,
                                                  const std::shared_ptr<Line>& aLine,
                                                  const std::shared_ptr<Tile>& aTile)
{
    CurveListPtr curves = aLine->GetCurveList();

    for (const auto& curve : *curves)
    {
        writeLaneBoundary(aElement, aDom, curve, aTile);
    }
}

void Model::KMLInterpreter::writeLaneBoundaryStyle(QDomElement& aElement, QDomDocument& aDom)
{
    QDomElement style = aDom.createElement("Style");
    style.setAttribute("id", "RoadMark");
    QDomElement lineStyle = aDom.createElement("LineStyle");
    QDomElement color = aDom.createElement("color");
    QDomText text = aDom.createTextNode("#ffff00ff");
    color.appendChild(text);
    lineStyle.appendChild(color);
    style.appendChild(lineStyle);
    aElement.appendChild(style);
}

void Model::KMLInterpreter::writeLaneBoundary(QDomElement& aElement,
                                              QDomDocument& aDom,
                                              const std::shared_ptr<Curve>& aCurve,
                                              const std::shared_ptr<Tile>& aTile)
{
    QDomElement placemark = aDom.createElement("Placemark");
    QDomElement name = aDom.createElement("name");
    QDomText nameValue = aDom.createTextNode(("Road Mark " + std::to_string(aCurve->GetCurveId())).c_str());
    name.appendChild(nameValue);
    QDomElement description = aDom.createElement("description");
    QDomText descriptionValue = aDom.createTextNode(("type=\"" + aCurve->GetCurveTypeDescriptor() + "\"").c_str());
    description.appendChild(descriptionValue);
    QDomElement styleUrl = aDom.createElement("styleUrl");
    QDomText styleUrlValue = aDom.createTextNode("Paint");
    styleUrl.appendChild(styleUrlValue);
    QDomElement altitudeMode = aDom.createElement("altitudeMode");
    styleUrl.appendChild(styleUrlValue);
    QDomText altitudeModeVale = aDom.createTextNode("absolute");
    altitudeMode.appendChild(altitudeModeVale);
    QDomElement lineString = aDom.createElement("LineString");
    QDomElement coordinates = aDom.createElement("coordinates");
    writeLaneBoundaryCoordinates(coordinates, aDom, aCurve, aTile);
    lineString.appendChild(coordinates);
    placemark.appendChild(name);
    placemark.appendChild(description);
    placemark.appendChild(styleUrl);
    placemark.appendChild(altitudeMode);
    placemark.appendChild(lineString);
    aElement.appendChild(placemark);
}

void Model::KMLInterpreter::writeLaneBoundaryCoordinates(QDomElement& aElement,
                                                         QDomDocument& aDom,
                                                         const std::shared_ptr<Curve>& aCurve,
                                                         const std::shared_ptr<Tile>& aTile)
{
    Point3DListPtr points = aCurve->CalculatePointCloud(mOutputInterval);
    const Point3DPtr& referencePoint = aTile->GetReferencePoint();
    std::stringstream ss;
    size_t pointSize = points->size();

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

        if (pointSize - 1 != i)
        {
            ss << "\n";
        }
    }

    QDomText coordinates = aDom.createTextNode(ss.str().c_str());
    aElement.appendChild(coordinates);
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
    QDomText colorValue = aDom.createTextNode("ff00ffff");
    color.appendChild(colorValue);
    QDomElement scale = aDom.createElement("scale");
    QDomText scaleValue = aDom.createTextNode("0.5");
    scale.appendChild(scaleValue);
    QDomElement icon = aDom.createElement("Icon");
    QDomElement href = aDom.createElement("href");
    QDomText hrefValue = aDom.createTextNode("http://maps.google.com/mapfiles/kml/shapes/shaded_dot.png");
    href.appendChild(hrefValue);
    icon.appendChild(href);
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
    QDomText colorValue = aDom.createTextNode("#ffff0000");
    color.appendChild(colorValue);
    QDomElement scale = aDom.createElement("scale");
    QDomText scaleValue = aDom.createTextNode("0.5");
    scale.appendChild(scaleValue);
    QDomElement icon = aDom.createElement("Icon");
    QDomElement href = aDom.createElement("href");
    QDomText hrefValue = aDom.createTextNode("http://maps.google.com/mapfiles/kml/shapes/shaded_dot.png");
    href.appendChild(hrefValue);
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
       QDomText keyValue = aDom.createTextNode("normal");
       key.appendChild(keyValue);
       QDomElement styleUrl = aDom.createElement("styleUrl");
       QDomText styleUrlValue = aDom.createTextNode("#sn_placemark_circle");
       styleUrl.appendChild(styleUrlValue);
       pair.appendChild(styleUrl);
       pair.appendChild(key);
       styleMap.appendChild(pair);
    }
    {
        QDomElement pair = aDom.createElement("Pair");
        QDomElement key = aDom.createElement("key");
        QDomText keyValue = aDom.createTextNode("highlight");
        key.appendChild(keyValue);
        QDomElement styleUrl = aDom.createElement("styleUrl");
        QDomText styleUrlValue = aDom.createTextNode("#sn_placemark_circle_highlight");
        styleUrl.appendChild(styleUrlValue);
        pair.appendChild(styleUrl);
        pair.appendChild(key);
        styleMap.appendChild(pair);
    }

    aElement.appendChild(styleMap);
}

void Model::KMLInterpreter::writeTrafficSign(QDomElement& aElement,
                                             QDomDocument& aDom,
                                             const std::shared_ptr<TrafficSign>& aTrafficSign,
                                             const std::shared_ptr<Tile>& aTile)
{
    QDomElement placemark = aDom.createElement("Placemark");
    QDomElement name = aDom.createElement("name");
    QDomText nameValue = aDom.createTextNode(
                ("Traffic Sign " + std::to_string(aTrafficSign->GetTrafficSignId())).c_str());
    name.appendChild(nameValue);
    QDomElement description = aDom.createElement("description");
    writeTrafficSignDescription(description, aDom, aTrafficSign);
    QDomElement altitudeMode = aDom.createElement("altitudeMode");
    QDomText altitudeModeValue = aDom.createTextNode("absolute");
    altitudeMode.appendChild(altitudeModeValue);
    QDomElement styleUrl = aDom.createElement("styleUrl");
    QDomText styleUrlValue = aDom.createTextNode("TrafficSign");
    styleUrl.appendChild(styleUrlValue);
    QDomElement point = aDom.createElement("Point");
    QDomElement drawOrder = aDom.createElement("drawOrder");
    QDomText drawOrderlValue = aDom.createTextNode("1");
    drawOrder.appendChild(drawOrderlValue);
    QDomElement coordinates = aDom.createElement("coordinates");
    writeTrafficSignCoordinates(coordinates, aDom, aTrafficSign);
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
                                                        QDomDocument& aDom,
                                                        const std::shared_ptr<TrafficSign> &aTrafficSign)
{
    std::stringstream ss;
    ss << "type=\"" << std::to_string(aTrafficSign->GetTrafficSignType()) << "\",";
    ss << "height=\"" << strings::FormatFloat<double>(aTrafficSign->GetShapeHeight(), 6) << "\",";
    ss << "width=\"" << strings::FormatFloat<double>(aTrafficSign->GetShapeWidth(), 6) << "\",";
    ss << "orientation=\"" << strings::FormatFloat<double>(aTrafficSign->GetOrientation(), 6) << "\",";
    ss << "confidence=\"" << strings::FormatFloat<double>(aTrafficSign->GetConfidence(), 6);
    QDomText description = aDom.createTextNode(ss.str().c_str());
    aElement.appendChild(description);
}

void Model::KMLInterpreter::writeTrafficSignCoordinates(QDomElement& aElement,
                                                        QDomDocument& aDom,
                                                        const std::shared_ptr<TrafficSign> &aTrafficSign)
{
    std::stringstream ss;
    ss  << strings::FormatFloat<double>(aTrafficSign->GetGeodeticPosition()->GetX(), 15) << ",";
    ss  << strings::FormatFloat<double>(aTrafficSign->GetGeodeticPosition()->GetY(), 15) << ",";
    ss  << strings::FormatFloat<double>(aTrafficSign->GetGeodeticPosition()->GetZ(), 15);
    QDomText coordinates = aDom.createTextNode(ss.str().c_str());
    aElement.appendChild(coordinates);
}
