/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    QIModel.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Common.h"
#include <QObject>
#include "Point3D.h"
#include <map>
#include "service/LogicDbLoader.h"
#include "PeqiDataDefine.h"
#include "IDataQualityAnalyzer.h"

namespace Model
{

class QIModel : public QObject
{
    Q_OBJECT

public:
    QIModel();

    std::map<int, std::vector<Point3D>>& getErrPointMap();

    Point3D getRefPoint();

    void setFilePathList(const std::vector<std::string>& filePathList);

    bool isSuccess() { return isSuccess_; }

private slots:
    void process();

private:
    bool loadDataFromDBFiles();
    bool convertLoaderToPointData();
    bool runQualityAnalyize();
    bool extractLogicalResult();

private:
    std::vector<std::string> filePathsList_;

    Service::LogicDbLoader DBloader_;
    PeqiDataDefine::PointData pointData_;
    IDataQualityAnalyzer dataAna_;
    PeqiDataDefine::ResultInfo* pResultInfo_;

    std::map<int, std::vector<Point3D>> errPointMap_;

    Point3D referencePoint_;

    bool isSuccess_;
};

}
