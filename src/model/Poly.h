/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    Poly.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "Common.h"
#include "Point3D.h"

namespace Model
{

// mA * t * t + mB * t + mC
class Poly2
{
public:
    Poly2();
    Poly2(const double& aA, const double& aB, const double& aC);

    void SetA(const double& aA);
    void SetB(const double& aB);
    void SetC(const double& aC);
    const double& GetA() const;
    const double& GetB() const;
    const double& GetC() const;

private:
    double mA;
    double mB;
    double mC;
};

typedef std::shared_ptr<Poly2> Poly2Ptr;

class Poly2XYZT
{
public:
    Poly2XYZT();

    Point3DPtr GetPosition(const double& t);
    Point3DPtr GetStartPoint();
    Point3DPtr GetEndPoint();

    void SetStart(const double& aStart);
    void SetEnd(const double& aEnd);
    void SetPoly2X(const Poly2Ptr& aPoly2X);
    void SetPoly2Y(const Poly2Ptr& aPoly2Y);
    void SetPoly2Z(const Poly2Ptr& aPoly2Z);
    const double& GetStart() const;
    const double& GetEnd() const;
    const Poly2Ptr& GetPoly2X() const;
    const Poly2Ptr& GetPoly2Y() const;
    const Poly2Ptr& GetPoly2Z() const;
    Poly2Ptr GetMutablePoly2X();
    Poly2Ptr GetMutablePoly2Y();
    Poly2Ptr GetMutablePoly2Z();

private:
    double mStart;
    double mEnd;
    Poly2Ptr mPoly2X;
    Poly2Ptr mPoly2Y;
    Poly2Ptr mPoly2Z;
};

typedef std::shared_ptr<Poly2XYZT> Poly2XYZTPtr;
typedef std::shared_ptr<const Poly2XYZT> Poly2XYZTConstPtr;
typedef std::vector<Poly2XYZTPtr> Poly2XYZTList;
typedef std::shared_ptr<Poly2XYZTList> Poly2XYZTListPtr;
typedef std::shared_ptr<const Poly2XYZTList> Poly2XYZTListConstPtr;

}
