/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    CheckLogicConsistencyCommand.cpp
 *******************************************************************************
 */

#include "CheckLogicConsistencyCommand.h"
#include "facade/ApplicationFacade.h"
#include "proxy/MainProxy.h"
#include "model/MemoryModel.h"
#include "service/AlgoPointDataConverter.h"
#include "IDataQualityAnalyzer.h"
#include "IMeasuresCommon.h"
#include "model/QIModel.h"

void Controller::CheckLogicConsistencyCommand::execute(PureMVC::Interfaces::INotification const& aNotification)
{
    if (aNotification.getName() == ApplicationFacade::CHECK_LOGIC_CONSISTENCY)
    {
        MainProxy& mainProxy = dynamic_cast<MainProxy&>(ApplicationFacade::RetriveProxy(MainProxy::NAME));
        const std::shared_ptr<Model::MemoryModel>& memoryModel = mainProxy.GetMemoryModel();
        if (memoryModel != nullptr)
        {
            const std::vector<std::string>& filePathsList = memoryModel->getFilePathList();

            Service::LogicDbLoader loader;
            if (!loader.loadLogicalDb(filePathsList, 1, false))
            {
                return;
            }

            PeqiDataDefine::PointData sensedPointData;
            if (!Service::AlgoPointDataConverter().convert(loader, sensedPointData))
            {
                return;
            }

            createQIModel();
            mainProxy.GetQIModel()->SetRefPoint(loader.getReferencePoint().GetX(), loader.getReferencePoint().GetY(), loader.getReferencePoint().GetZ());
            checkLogicConsistency(sensedPointData);
        }
    }
    ApplicationFacade::SendNotification(ApplicationFacade::CHECK_LOGIC_CONSISTENCY_COMPLETE);
}

std::string Controller::CheckLogicConsistencyCommand::GetCommandName()
{
    return "CheckLogicConsistencyCommand";
}

void Controller::CheckLogicConsistencyCommand::checkLogicConsistency(PeqiDataDefine::PointData& sensorData)
{
    PeqiDataDefine::ResultInfo* rlt;
    std::vector<PeqiDataDefine::ResultInfo*> rltArrRegion;

    IDataQualityAnalyzer dataAna;

    PeqiDataDefine::XhNameList list;
    std::vector<MeasureType> test;

    dataAna.GetMeasureList(list);

    for(size_t i = 0; i < list.size(); i++)
    {
        test.push_back( (MeasureType)(list.at(i).mID));
    }

    PeqiDataDefine::ConfigParam cfg;

    cfg.mFeatureMeasureMap.insert(std::make_pair(PeqiDataDefine::ClyType::LaneLine,test));
    cfg.mFeatureMeasureMap.insert(std::make_pair(PeqiDataDefine::ClyType::TrafficSign,test));

    //set para
    dataAna.SetConfigParameter(cfg, "../AlgoConfig.ini");

    //slice data
    std::map<std::int64_t, PeqiDataDefine::PointData> allRefSlices;
    std::map<std::int64_t, PeqiDataDefine::PointData> allTestSlices;
    int64_t nMaxSlices;
    IDataProcessor::SliceData(nullptr,&sensorData,3,3,allRefSlices,allTestSlices,nMaxSlices);

    dataAna.SetWorkMode(PeqiDataDefine::WorkMode::StatsRegion);
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
            dataAna.SetPointData(1, nullptr, &allTestSlices[i]);
        }
        else if(it_sensor == allTestSlices.end())
        {
            dataAna.SetPointData(1, &allRefSlices[i], nullptr);
        }
        else
        {
            dataAna.SetPointData(1, &allRefSlices[i],&allTestSlices[i]);
        }


        dataAna.GetResult(rlt);

        PeqiDataDefine::ResultInfo* cpyInfo = new PeqiDataDefine::ResultInfo;
        *cpyInfo = *rlt;
        rltArrRegion.push_back(cpyInfo);

    }

    dataAna.SetWorkMode(PeqiDataDefine::WorkMode::StatsComplexMap);
    dataAna.SetStatsInfo(rltArrRegion);

    for(size_t k = 0 ; k < rltArrRegion.size(); k++)
    {
        delete rltArrRegion[k];
    }

    rltArrRegion.clear();
    if  (0 == dataAna.GetResult(rlt))
    {
        saveResult(*rlt);
    }
}

void Controller::CheckLogicConsistencyCommand::createQIModel()
{
    MainProxy& mainProxy = dynamic_cast<MainProxy&>(ApplicationFacade::RetriveProxy(MainProxy::NAME));
    std::shared_ptr<Model::QIModel> qiModel(new Model::QIModel);
    mainProxy.SetQIModel(qiModel);
}

void Controller::CheckLogicConsistencyCommand::saveResult(PeqiDataDefine::ResultInfo& result)
{
    MainProxy& mainProxy = dynamic_cast<MainProxy&>(ApplicationFacade::RetriveProxy(MainProxy::NAME));
    for (const auto& totalInfo : result.mMapInfoList[1])
    {
        for (const auto& iterMeasure : totalInfo.second.mMethodList)
        {
            ST_Method_Logical* logicalMeasure =
                    dynamic_cast<ST_Method_Logical*>(iterMeasure);
            if (logicalMeasure != Q_NULLPTR)
            {
                for (const auto& iterLogicalValue :
                     logicalMeasure->mRltsValue)
                {
                    if (iterLogicalValue == Q_NULLPTR)
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
                        Model::ErrorPoint errPoint;
                        errPoint.dX = iterLogicalValue->mPos[3 * i];
                        errPoint.dY = iterLogicalValue->mPos[3 * i + 1];
                        errPoint.dZ = iterLogicalValue->mPos[3 * i + 2];
                        mainProxy.GetQIModel()->AddErrPoint(logicalMeasure->mID, errPoint);
                    }
                }

                for (const auto& iterLogicalError :
                     logicalMeasure->mRltsError)
                {
                    if (iterLogicalError == Q_NULLPTR)
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
                        Model::ErrorPoint errPoint;
                        errPoint.dX = iterLogicalError->mPos[3 * i];
                        errPoint.dY = iterLogicalError->mPos[3 * i + 1];
                        errPoint.dZ = iterLogicalError->mPos[3 * i + 2];
                        mainProxy.GetQIModel()->AddErrPoint(logicalMeasure->mID, errPoint);
                    }
                }
            }
        }
    }
}
