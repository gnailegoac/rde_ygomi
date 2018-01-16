/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    KMLInterpreter.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "../Common.h"
#include "../Point3D.h"

class QDomDocument;
class QDomElement;

namespace  Model
{

class MemoryModel;
class Tile;
class Curve;
class Line;
class TrafficSign;

class KMLInterpreter
{
public:
    KMLInterpreter(std::string aKMLFolder, double aOutputInterval = 0.1);
    ~KMLInterpreter();

    bool TouchKMLDirectories();
    bool SaveKMLPaint(const std::shared_ptr<Tile>& aTile);
    bool SaveKMLLaneBoundary(const std::shared_ptr<Tile>& aTile);
    bool SaveKMLTrafficSign(const std::shared_ptr<Tile>& aTile);

private:
    const std::string& createKMLPath(const std::string& aPrefix,
                                     const std::int64_t& aTileId,
                                     StringListPtr& aFiles);
    std::string getLaneTypeStr(const std::shared_ptr<Line>& aline);
    bool saveKMLFile(const std::string& aPath, QDomDocument& aDocument) const;
    bool touchKMLDirectory(const std::string& aPath) const;

    //===============================================================================
    void writeInstructions(QDomDocument& aDom);
    void writeGoogleEarth(QDomElement& aElement);
    //===============================================================================
    void writePaintLine(QDomElement& aElement,
                        QDomDocument& aDom,
                        const std::shared_ptr<Line>& aLine,
                        const std::shared_ptr<Tile>& aTile);
    void writePaintStyle(QDomElement& aElement, QDomDocument& aDom);
    void writePaint(QDomElement& aElement,
                    QDomDocument& aDom,
                    const std::shared_ptr<Curve>& aCurve,
                    const std::shared_ptr<Tile>& aTile);
    void writePaintCoordinates(QDomElement& aElement,
                               QDomDocument& aDom,
                               const Point3DListPtr& aPoints,
                               const std::shared_ptr<Tile>& aTile);
    //===============================================================================
    void writeLaneBoundaryLine(QDomElement& aElement,
                               QDomDocument& aDom,
                               const std::shared_ptr<Line>& aLine,
                               const std::shared_ptr<Tile>& aTile);
    void writeLaneBoundaryStyle(QDomElement& aElement, QDomDocument& aDom);
    void writeLaneBoundary(QDomElement& aElement,
                           QDomDocument& aDom,
                           const std::shared_ptr<Curve>& aCurve,
                           const std::shared_ptr<Tile>& aTile);
    void writeLaneBoundaryCoordinates(QDomElement& aElement,
                                      QDomDocument& aDom,
                                      const std::shared_ptr<Curve>& aCurve,
                                      const std::shared_ptr<Tile>& aTile);
    //===============================================================================
    void writeTrafficSignStyle(QDomElement& aElement, QDomDocument& aDom);
    void writeTrafficSignStyleCircle(QDomElement& aElement, QDomDocument& aDom);
    void writeTrafficSignStyleCircleHighlight(QDomElement& aElement, QDomDocument& aDom);
    void writeTrafficSignStyleMap(QDomElement& aElement, QDomDocument& aDom);
    void writeTrafficSign(QDomElement& aElement,
                          QDomDocument& aDom,
                          const std::shared_ptr<TrafficSign> &aTrafficSign,
                          const std::shared_ptr<Tile>& aTile);
    void writeTrafficSignDescription(QDomElement& aElement,
                                     QDomDocument& aDom,
                                     const std::shared_ptr<TrafficSign> &aTrafficSign);
    void writeTrafficSignCoordinates(QDomElement& aElement,
                                     QDomDocument& aDom,
                                     const std::shared_ptr<TrafficSign> &aTrafficSign);

private:
    double mOutputInterval;
    std::string mPaintFolder;
    std::string mLaneBoundaryFolder;
    std::string mTrafficSignFolder;
    StringListPtr mPaintFiles;
    StringListPtr mLaneBoundaryFiles;
    StringListPtr mTrafficSignFiles;
};

typedef std::shared_ptr<KMLInterpreter> KMLInterpreterPtr;
typedef std::shared_ptr<const KMLInterpreter> KMLInterpreterConstPtr;

}
