/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Element.cpp
 * @brief
 *******************************************************************************
 */

#include "Element.h"

Element::Element(bool aIsEditable, bool aIsEdited):
    mIsEditable(aIsEditable),
    mIsEdited(aIsEdited),
    mBox(nullptr)
{

}

Element::~Element()
{

}

bool Element::IsEditable() const
{
    return mIsEditable;
}

void Element::SetEditable(bool aIsEditable)
{
    mIsEditable = aIsEditable;
}

bool Element::IsEdited() const
{
    return mIsEdited;
}

void Element::SetEdited(bool aIsEdited)
{
    mIsEdited = aIsEdited;
}

std::shared_ptr<BoundingBox> Element::GetBox()
{
    return mBox;
}

void Element::SetBox(std::shared_ptr<BoundingBox> aBox)
{
    mBox = aBox;
}
