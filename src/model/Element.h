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

namespace Model
{

class BoundingBox;

typedef std::shared_ptr<BoundingBox> BoundingBoxPtr;

class Element
{
public:
    Element(bool aIsEditable = true, bool aIsEdited = false);
    ~Element();

    bool IsEditable() const;
    void SetEditable(bool aIsEditable);

    bool IsEdited() const;
    void SetEdited(bool aIsEdited);

    BoundingBoxPtr GetBox();
    void SetBox(const BoundingBoxPtr& aBox);

protected:
    bool mIsEditable;
    bool mIsEdited;
    BoundingBoxPtr mBox;
};

typedef std::shared_ptr<Element> ElementPtr;
typedef std::shared_ptr<const Element> ElementConstPtr;
typedef std::vector<ElementPtr> ElementList;
typedef std::shared_ptr<ElementList> ElementListPtr;
typedef std::shared_ptr<const ElementList> ElementListConstPtr;

}
