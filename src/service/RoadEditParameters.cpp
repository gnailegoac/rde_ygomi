/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    RoadEditParameters.cpp
 * @brief
 *******************************************************************************
 */

#include "RoadEditParameters.h"

const std::shared_ptr<Service::RoadEditParameters>& Service::RoadEditParameters::Instance()
{
    static std::shared_ptr<Service::RoadEditParameters> roadEditParameters(new Service::RoadEditParameters());
    return roadEditParameters;
}

bool Service::RoadEditParameters::IsInMultiSelectMode() const
{
    return mMultiSelectMode;
}

void Service::RoadEditParameters::EnableMultiSelectMode(bool aEnable)
{
    mMultiSelectMode = aEnable;
}

void Service::RoadEditParameters::ClearSelectedElement()
{
    mIdVec.clear();
}

void Service::RoadEditParameters::AddSelectedElementId(const uint64_t& aId)
{
    mIdVec.push_back(aId);
}

void Service::RoadEditParameters::SetSelectedElementId(const uint64_t& aId)
{
    mIdVec.clear();
    mIdVec.push_back(aId);
}

const std::vector<uint64_t>& Service::RoadEditParameters::GetSelectedElementIds() const
{
    return mIdVec;
}

void Service::RoadEditParameters::SetEditType(const Service::EditType& aType)
{
    mEditType = aType;
}

const Service::EditType& Service::RoadEditParameters::GetEditType() const
{
    return mEditType;
}

Service::RoadEditParameters::RoadEditParameters() :
    mEditType(EditType::None),
    mMultiSelectMode(false)
{

}
