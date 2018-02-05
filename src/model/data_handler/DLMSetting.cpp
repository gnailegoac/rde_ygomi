/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DLMSetting.cpp
 * @brief
 *******************************************************************************
 */

#include "DLMSetting.h"

Model::DLMSetting::DLMSetting() :
    mSourceVersion(1),
    mTargetVersion(1),
    mBaselineMapId(1),
    mJournalVersion("2.0.1"),
    mIsoCountryCode("DEU"),
    mTrafficeSense(DLM::TrafficSense::RightHand)
{

}

const std::shared_ptr<Model::DLMSetting>& Model::DLMSetting::Instance()
{
    static std::shared_ptr<Model::DLMSetting> dlmSetting(new Model::DLMSetting());
    return dlmSetting;
}

void Model::DLMSetting::SetSourceVersion(const uint32_t& aSourceVersion)
{
    mSourceVersion = aSourceVersion;
}

void Model::DLMSetting::SetTargetVersion(const uint32_t& aTargetVersion)
{
    mTargetVersion = aTargetVersion;
}

void Model::DLMSetting::SetBaselineMapId(const uint32_t& aBaselineMapId)
{
    mBaselineMapId = aBaselineMapId;
}

void Model::DLMSetting::SetJournalVersion(const QString& aJournalVersion)
{
    mJournalVersion = aJournalVersion;
}

void Model::DLMSetting::SetIsoCountryCode(const QString& aCountryCode)
{
    mIsoCountryCode = aCountryCode;
}

void Model::DLMSetting::SetTrafficSense(const DLM::TrafficSense& aTrafficSense)
{
    mTrafficeSense = aTrafficSense;
}

const uint32_t& Model::DLMSetting::GetSourceVersion() const
{
    return mSourceVersion;
}

const uint32_t& Model::DLMSetting::GetTargetVersion() const
{
    return mTargetVersion;
}

const uint32_t& Model::DLMSetting::GetBaselineMapId() const
{
    return mBaselineMapId;
}

const QString& Model::DLMSetting::GetJournalVersion() const
{
    return mJournalVersion;
}

const QString& Model::DLMSetting::GetIsoCountryCode() const
{
    return mIsoCountryCode;
}

const DLM::TrafficSense& Model::DLMSetting::GetTrafficSense() const
{
    return mTrafficeSense;
}

