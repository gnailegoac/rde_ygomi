/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Poly.cpp
 * @brief
 *******************************************************************************
 */

#include "Poly.h"

Model::Poly2::Poly2():
    mA(0.0),
    mB(0.0),
    mC(0.0)
{

}

Model::Poly2::Poly2(const double& aA, const double& aB, const double& aC):
    mA(aA),
    mB(aB),
    mC(aC)
{

}

void Model::Poly2::SetA(const double& aA)
{
    mA = aA;
}

void Model::Poly2::SetB(const double& aB)
{
    mB = aB;
}

void Model::Poly2::SetC(const double& aC)
{
    mC = aC;
}

const double& Model::Poly2::GetA() const
{
    return mA;
}

const double& Model::Poly2::GetB() const
{
    return mB;
}

const double& Model::Poly2::GetC() const
{
    return mC;
}

Model::Poly2XYZT::Poly2XYZT():
    mStart(0.0),
    mEnd(1.0),
    mPoly2X(new Poly2),
    mPoly2Y(new Poly2),
    mPoly2Z(new Poly2)
{

}

Model::Point3DPtr Model::Poly2XYZT::GetPosition(const double& t)
{
    double x = mPoly2X->GetA() * t * t + mPoly2X->GetB() * t + mPoly2X->GetC();
    double y = mPoly2Y->GetA() * t * t + mPoly2Y->GetB() * t + mPoly2Y->GetC();
    double z = mPoly2Z->GetA() * t * t + mPoly2Z->GetB() * t + mPoly2Z->GetC();

    return std::make_shared<Point3D>(x, y, z);
}

Model::Point3DPtr Model::Poly2XYZT::GetStartPoint()
{
    return GetPosition(mStart);
}

Model::Point3DPtr Model::Poly2XYZT::GetEndPoint()
{
    return GetPosition(mEnd);
}

void Model::Poly2XYZT::SetStart(const double& aStart)
{
    mStart = aStart;
}

void Model::Poly2XYZT::SetEnd(const double& aEnd)
{
    mEnd = aEnd;
}

void Model::Poly2XYZT::SetPoly2X(const Model::Poly2Ptr& aPoly2X)
{
    mPoly2X->SetA(aPoly2X->GetA());
    mPoly2X->SetB(aPoly2X->GetB());
    mPoly2X->SetC(aPoly2X->GetC());
}

void Model::Poly2XYZT::SetPoly2Y(const Model::Poly2Ptr& aPoly2Y)
{
    mPoly2Y->SetA(aPoly2Y->GetA());
    mPoly2Y->SetB(aPoly2Y->GetB());
    mPoly2Y->SetC(aPoly2Y->GetC());
}

void Model::Poly2XYZT::SetPoly2Z(const Model::Poly2Ptr& aPoly2Z)
{
    mPoly2Z->SetA(aPoly2Z->GetA());
    mPoly2Z->SetB(aPoly2Z->GetB());
    mPoly2Z->SetC(aPoly2Z->GetC());
}

const double& Model::Poly2XYZT::GetStart() const
{
    return mStart;
}

const double& Model::Poly2XYZT::GetEnd() const
{
    return mEnd;
}

const Model::Poly2Ptr& Model::Poly2XYZT::GetPoly2X() const
{
    return mPoly2X;
}

const Model::Poly2Ptr& Model::Poly2XYZT::GetPoly2Y() const
{
    return mPoly2Y;
}

const Model::Poly2Ptr& Model::Poly2XYZT::GetPoly2Z() const
{
    return mPoly2Z;
}

Model::Poly2Ptr Model::Poly2XYZT::GetMutablePoly2X()
{
    return mPoly2X;
}

Model::Poly2Ptr Model::Poly2XYZT::GetMutablePoly2Y()
{
    return mPoly2Y;
}

Model::Poly2Ptr Model::Poly2XYZT::GetMutablePoly2Z()
{
    return mPoly2Z;
}
