/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    RoadEditParameters.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include <memory>
#include <vector>

namespace Service
{
enum class EditType
{
    None,
    Road,
    Lane,
    Line
};

class RoadEditParameters
{
public:
    static const std::shared_ptr<RoadEditParameters>& Instance();

    bool IsInMultiSelectMode() const;
    void EnableMultiSelectMode(bool aEnable);
    void ClearSelectedElement();
    void AddSelectedElementId(const uint64_t& aId);
    void SetSelectedElementId(const uint64_t& aId);
    const std::vector<uint64_t>& GetSelectedElementIds() const;
    void SetEditType(const EditType& aType);
    const EditType& GetEditType() const;

private:
    RoadEditParameters();

private:
    EditType mEditType;
    bool mMultiSelectMode;
    std::vector<uint64_t> mIdVec;
};
}
