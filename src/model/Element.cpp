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

#include "BoundingBox.h"

Model::Element::Element(bool aIsEditable, bool aIsEdited):
    mIsEditable(aIsEditable),
    mIsEdited(aIsEdited),
    mBox(std::make_shared<BoundingBox>())
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

const Model::BoundingBoxPtr& Model::Element::GetBox() const
{
    return mBox;
}

Model::BoundingBoxPtr Model::Element::GetMutableBox()
{
    return mBox;
}

void Model::Element::SetBox(const Model::BoundingBoxPtr& aBox)
{
    mBox->SetHeading(aBox->GetHeading());
    mBox->SetHeight(aBox->GetHeight());
    mBox->SetLength(aBox->GetLength());
    mBox->SetPosition(aBox->GetPosition());
    mBox->SetWidth(aBox->GetWidth());
}
