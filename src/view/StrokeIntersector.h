/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    StrokeIntersector.h
 * @brief
 *******************************************************************************
 */

#pragma once
#include <osgUtil/LineSegmentIntersector>

class StrokeIntersector : public osgUtil::LineSegmentIntersector
{
public:
    StrokeIntersector();
    StrokeIntersector(const osg::Vec3& aStart, const osg::Vec3& aEnd);
    StrokeIntersector(const CoordinateFrame& aCoordinateFrame, const double& aX, const double& aY);

    osgUtil::Intersector* clone(osgUtil::IntersectionVisitor& iv) override;
    void intersect(osgUtil::IntersectionVisitor& aIntersectionVisitor, osg::Drawable* aDrawable) override;

protected:
    virtual ~StrokeIntersector(){}

    bool isEqualDouble(double aFirstValue, double aSecondValue);
    double squreDistanceSegmentToSegment(const osg::Vec3d& aFirstStartPoint,
                                         const osg::Vec3d& aFirstEndPoint,
                                         const osg::Vec3d& aSecondStartPoint,
                                         const osg::Vec3d& aSecondEndPoint);
private:
    float m_offset;
};
