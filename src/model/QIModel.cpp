/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    QIModel.cpp
 *******************************************************************************
 */

#include <QThread>
#include "QIModel.h"
#include "service/AlgoPointDataConverter.h"
#include "IMeasuresCommon.h"

namespace Model {

QIModel::QIModel():
    pResultInfo_(nullptr),
    isSuccess_(false)
{}

bool QIModel::loadDataFromDBFiles()
{
    return DBloader_.loadLogicalDb(filePathsList_, 1, false);
}

bool QIModel::convertLoaderToPointData()
{
    return Service::AlgoPointDataConverter().convert(DBloader_, pointData_);
}

bool QIModel::runQualityAnalyize()
{
    PeqiDataDefine::XhNameList list;
    std::vector<MeasureType> test;
    std::vector<PeqiDataDefine::ResultInfo*> rltArrRegion;
    PeqiDataDefine::ConfigParam cfg;

    dataAna_.GetMeasureList(list);

    for(size_t i = 0; i < list.size(); i++)
    {
        test.push_back((MeasureType)(list.at(i).mID));
    }

    cfg.mFeatureMeasureMap.insert(std::make_pair(PeqiDataDefine::ClyType::LaneLine,test));
    cfg.mFeatureMeasureMap.insert(std::make_pair(PeqiDataDefine::ClyType::TrafficSign,test));

    //set para
    dataAna_.SetConfigParameter(cfg, "../AlgoConfig.ini");

    //slice data
    std::map<std::int64_t, PeqiDataDefine::PointData> allRefSlices;
    std::map<std::int64_t, PeqiDataDefine::PointData> allTestSlices;
    int64_t nMaxSlices;
    IDataProcessor::SliceData(nullptr,&pointData_,3,3,allRefSlices,allTestSlices,nMaxSlices);

    dataAna_.SetWorkMode(PeqiDataDefine::WorkMode::StatsRegion);

    for(int i = 0; i < nMaxSlices; i++)
    {
        std::map<std::int64_t, PeqiDataDefine::PointData>::iterator it_sensor = allTestSlices.find(i);
        std::map<std::int64_t, PeqiDataDefine::PointData>::iterator it_ref = allRefSlices.find(i);
        if((it_ref == allRefSlices.end())&& (it_sensor == allTestSlices.end()))
        {
            continue;
        }
        else if(it_ref == allRefSlices.end() && (it_sensor != allTestSlices.end()))
        {
            dataAna_.SetPointData(1, nullptr, &allTestSlices[i]);
        }
        else if(it_sensor == allTestSlices.end())
        {
            dataAna_.SetPointData(1, &allRefSlices[i], nullptr);
        }
        else
        {
            dataAna_.SetPointData(1, &allRefSlices[i],&allTestSlices[i]);
        }

        dataAna_.GetResult(pResultInfo_);

        PeqiDataDefine::ResultInfo* cpyInfo = new PeqiDataDefine::ResultInfo;
        *cpyInfo = *pResultInfo_;
        rltArrRegion.push_back(cpyInfo);
    }

    dataAna_.SetWorkMode(PeqiDataDefine::WorkMode::StatsComplexMap);
    dataAna_.SetStatsInfo(rltArrRegion);

    for(size_t k = 0 ; k < rltArrRegion.size(); k++)
    {
        delete rltArrRegion[k];
    }

    rltArrRegion.clear();
    return dataAna_.GetResult(pResultInfo_) == 0;
}

bool QIModel::extractLogicalResult()
{
    if (pResultInfo_ == nullptr)
        return false;

    for (const auto& totalInfo : pResultInfo_->mMapInfoList[1])
    {
        for (const auto& iterMeasure : totalInfo.second.mMethodList)
        {
            ST_Method_Logical* logicalMeasure =
                    dynamic_cast<ST_Method_Logical*>(iterMeasure);
            if (logicalMeasure != nullptr)
            {
                errDescMap_[logicalMeasure->mID] = logicalMeasure->mDescription;

                for (const auto& iterLogicalValue :
                     logicalMeasure->mRltsValue)
                {
                    if (iterLogicalValue == nullptr)
                    {
                        continue;
                    }
                    if (!iterLogicalValue->mExistErr)
                    {
                        continue;
                    }

                    for (std::size_t i = 0;
                         3 * i + 2 < iterLogicalValue->mPos.size(); ++i)
                    {
                        errPointMap_[logicalMeasure->mID].push_back(Point3D(iterLogicalValue->mPos[3 * i],
                                                        iterLogicalValue->mPos[3 * i +1],
                                                        iterLogicalValue->mPos[3 * i + 2]));
                    }
                }

                for (const auto& iterLogicalError :
                     logicalMeasure->mRltsError)
                {
                    if (iterLogicalError == nullptr)
                    {
                        continue;
                    }
                    if (!iterLogicalError->mError)
                    {
                        continue;
                    }

                    for (std::size_t i = 0;
                         3 * i + 2 < iterLogicalError->mPos.size(); ++i)
                    {
                        errPointMap_[logicalMeasure->mID].push_back(Point3D(iterLogicalError->mPos[3 * i],
                                                        iterLogicalError->mPos[3 * i +1],
                                                        iterLogicalError->mPos[3 * i + 2]));
                    }
                }
            }
        }
    }
    return true;
}

Point3D QIModel::getRefPoint()
{
    return Point3D(DBloader_.getReferencePoint().GetX(),
                   DBloader_.getReferencePoint().GetY(),
                   DBloader_.getReferencePoint().GetZ());
}

void QIModel::setFilePathList(const std::vector<std::string> &filePathList)
{
    filePathsList_ = filePathList;
}

void QIModel::process()
{
    isSuccess_ = false;
    if (loadDataFromDBFiles())
        if (convertLoaderToPointData())
            if (runQualityAnalyize())
                if (extractLogicalResult())
                    isSuccess_ = true;
    emit resultReady();
}

}
