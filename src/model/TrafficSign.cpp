/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    TrafficSign.cpp
 * @brief
 *******************************************************************************
 */

#include "TrafficSign.h"

#include "CoordinateTransform/Factory.h"
#include "LookUpTable.h"

Model::TrafficSign::TrafficSign():
    Element(),
    mTrafficSignType(0),
    mOrientation(0),
    mShapeWidth(0.0),
    mShapeHeight(0.0),
    mConfidence(0.0),
    mGeodeticPosition(std::make_shared<Point3D>()),
    mViewPosition(std::make_shared<Point3D>()),
    mTile(nullptr)
{

}

Model::TrafficSign::~TrafficSign()
{

}

const uint64_t& Model::TrafficSign::GetTrafficSignId() const
{
    return mTrafficSignId;
}

void Model::TrafficSign::SetTrafficSignId(const uint64_t& aTrafficSignId)
{
    mTrafficSignId = aTrafficSignId;
}

const uint64_t& Model::TrafficSign::GetTrafficSignType() const
{
    return mTrafficSignType;
}

void Model::TrafficSign::SetTrafficSignType(const uint64_t& aTrafficSignType)
{
    mTrafficSignType = aTrafficSignType;
}

const double& Model::TrafficSign::GetOrientation() const
{
    return mOrientation;
}

void Model::TrafficSign::SetOrientation(const double& aOrientation)
{
    mOrientation = aOrientation;
}

const double& Model::TrafficSign::GetShapeWidth() const
{
    return mShapeWidth;
}

void Model::TrafficSign::SetShapeWidth(const double& aShapeWidth)
{
    mShapeWidth = aShapeWidth;
}

const double& Model::TrafficSign::GetShapeHeight() const
{
    return mShapeHeight;
}

void Model::TrafficSign::SetShapeHeight(const double& aShapeHeight)
{
    mShapeHeight = aShapeHeight;
}

float Model::TrafficSign::GetConfidence() const
{
    return mConfidence;
}

void Model::TrafficSign::SetConfidence(float aConfidence)
{
    mConfidence = aConfidence;
}

const Model::Point3DPtr& Model::TrafficSign::GetGeodeticPosition() const
{
    return mGeodeticPosition;
}

void Model::TrafficSign::SetGeodeticPosition(const Point3DPtr& aGeodeticPosition)
{
    mGeodeticPosition->SetX(aGeodeticPosition->GetX());
    mGeodeticPosition->SetY(aGeodeticPosition->GetY());
    mGeodeticPosition->SetZ(aGeodeticPosition->GetZ());
}

void Model::TrafficSign::SetGeodeticFromRelative(const Point3DPtr& aRelative, const Point3DPtr& aReference)
{
    // Convert geodetic coordinates into ECEF coordinates
    std::shared_ptr<CRS::Factory> factory = std::make_shared<CRS::Factory>();
    std::unique_ptr<CRS::ICoordinateTransform> relativeToWgs84 =
                    factory->CreateRelativeTransform(CRS::CoordinateType::Relative,
                                                     CRS::CoordinateType::Wgs84,
                                                     aReference->GetX(),
                                                     aReference->GetY(),
                                                     aReference->GetZ());

    double x = aRelative->GetX();
    double y = aRelative->GetY();
    double z = aRelative->GetZ();
    relativeToWgs84->Transform(x, y, z);
    mGeodeticPosition->SetX(x);
    mGeodeticPosition->SetY(y);
    mGeodeticPosition->SetZ(z);
}

const Model::Point3DPtr& Model::TrafficSign::GetViewPosition() const
{
    return mViewPosition;
}

void Model::TrafficSign::SetViewPosition(const Point3DPtr& aViewPosition)
{
    mViewPosition->SetX(aViewPosition->GetX());
    mViewPosition->SetY(aViewPosition->GetY());
    mViewPosition->SetZ(aViewPosition->GetZ());
}

void Model::TrafficSign::GenerateViewPosition(std::unique_ptr<CRS::ICoordinateTransform>& aTransformer)
{
    double lon = mGeodeticPosition->GetX();
    double lat = mGeodeticPosition->GetY();
    double ele = mGeodeticPosition->GetZ();
    aTransformer->Transform(lon, lat, ele);
    mViewPosition->SetX(lon);
    mViewPosition->SetY(lat);
    mViewPosition->SetZ(ele);
}

const std::string& Model::TrafficSign::GetImagePath() const
{
    if (0 != Model::LookUpTable::mTrafficSignImageMap.count(mTrafficSignType))
    {
        return Model::LookUpTable::mTrafficSignImageMap[mTrafficSignType];
    }

    return Model::LookUpTable::mDefaultTrafficSignImagePath;
}

const std::shared_ptr<Model::Tile> Model::TrafficSign::GetTile() const
{
    return mTile;
}

void Model::TrafficSign::SetTile(const std::shared_ptr<Model::Tile>& aTile)
{
    mTile = aTile;
}
