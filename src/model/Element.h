/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Element.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Common.h"
#include "BoundingBox.h"

class Element
{
public:
    Element(bool aIsEditable = true, bool aIsEdited = false);
    ~Element();

    bool IsEditable() const;
    void SetEditable(bool aIsEditable);

    bool IsEdited() const;
    void SetEdited(bool aIsEdited);

    std::shared_ptr<BoundingBox> GetBox();
    void SetBox(std::shared_ptr<BoundingBox> aBox);

protected:
    bool mIsEditable;
    bool mIsEdited;
    std::shared_ptr<BoundingBox> mBox;
};
