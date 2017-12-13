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

Model::Element::Element(bool aIsEditable, bool aIsEdited):
    mIsEditable(aIsEditable),
    mIsEdited(aIsEdited),
    mBox(nullptr)
{

}

Model::Element::~Element()
{

}

bool Model::Element::IsEditable() const
{
    return mIsEditable;
}

void Model::Element::SetEditable(bool aIsEditable)
{
    mIsEditable = aIsEditable;
}

bool Model::Element::IsEdited() const
{
    return mIsEdited;
}

void Model::Element::SetEdited(bool aIsEdited)
{
    mIsEdited = aIsEdited;
}

std::shared_ptr<Model::BoundingBox> Model::Element::GetBox()
{
    return mBox;
}

void Model::Element::SetBox(std::shared_ptr<BoundingBox> aBox)
{
    mBox = aBox;
}
