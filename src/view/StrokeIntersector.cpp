/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. %YEAR%
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    StrokeIntersector.cpp
 *******************************************************************************
 */

#include "StrokeIntersector.h"
#include <osg/Geometry>

StrokeIntersector::StrokeIntersector() :
    osgUtil::LineSegmentIntersector(MODEL, 0.0, 0.0),
    m_offset(5.0f)
{

}

StrokeIntersector::StrokeIntersector(const osg::Vec3& aStart, const osg::Vec3& aEnd) :
    osgUtil::LineSegmentIntersector(aStart, aEnd),
    m_offset(5.0f)
{

}

StrokeIntersector::StrokeIntersector(const CoordinateFrame& aCoordinateFrame,
                                     const double& aX,
                                     const double& aY) :
    osgUtil::LineSegmentIntersector(aCoordinateFrame, aX, aY),
    m_offset(5.0f)
{

}

osgUtil::Intersector* StrokeIntersector::clone(osgUtil::IntersectionVisitor& aIntersectionVisitor)
{
    if (_coordinateFrame == MODEL && aIntersectionVisitor.getModelMatrix() == 0)
    {
        osg::ref_ptr<StrokeIntersector> cloned = new StrokeIntersector(_start, _end);
        cloned->_parent = this;
        cloned->m_offset = m_offset;
        return cloned.release();
    }

    osg::Matrix matrix;
    if (_coordinateFrame == WINDOW)
    {
        if (aIntersectionVisitor.getWindowMatrix())
        {
            matrix.preMult(*aIntersectionVisitor.getWindowMatrix());
        }
        if (aIntersectionVisitor.getProjectionMatrix())
        {
            matrix.preMult(*aIntersectionVisitor.getProjectionMatrix());
        }
        if (aIntersectionVisitor.getViewMatrix())
        {
            matrix.preMult(*aIntersectionVisitor.getViewMatrix());
        }
        if (aIntersectionVisitor.getModelMatrix())
        {
            matrix.preMult(*aIntersectionVisitor.getModelMatrix());
        }
    }
    else if (_coordinateFrame == PROJECTION)
    {
        if (aIntersectionVisitor.getProjectionMatrix())
        {
            matrix.preMult(*aIntersectionVisitor.getProjectionMatrix());
        }
        if (aIntersectionVisitor.getViewMatrix())
        {
            matrix.preMult(*aIntersectionVisitor.getViewMatrix());
        }
        if (aIntersectionVisitor.getModelMatrix())
        {
            matrix.preMult(*aIntersectionVisitor.getModelMatrix());
        }
    }
    else if (_coordinateFrame == VIEW)
    {
        if (aIntersectionVisitor.getViewMatrix())
        {
            matrix.preMult(*aIntersectionVisitor.getViewMatrix());
        }
        if (aIntersectionVisitor.getModelMatrix())
        {
            matrix.preMult(*aIntersectionVisitor.getModelMatrix());
        }
    }
    else if (_coordinateFrame == MODEL)
    {
        if (aIntersectionVisitor.getModelMatrix())
        {
            matrix = *aIntersectionVisitor.getModelMatrix();
        }
    }

    osg::Matrix inverse = osg::Matrix::inverse(matrix);
    osg::ref_ptr<StrokeIntersector> cloned = new StrokeIntersector(_start * inverse, _end * inverse);
    cloned->_parent = this;
    cloned->m_offset = m_offset;
    return cloned.release();
}

bool StrokeIntersector::isEqualDouble(double aFirstValue, double aSecondValue)
{
    if (fabs(aFirstValue - aSecondValue) < 1e-7)
    {
        return true;
    }

    return false;
}

double StrokeIntersector::squreDistanceSegmentToSegment(const osg::Vec3d& aFirstStartPoint,
                                     const osg::Vec3d& aFirstEndPoint,
                                     const osg::Vec3d& aSecondStartPoint,
                                     const osg::Vec3d& aSecondEndPoint)
{
    double ux = aFirstEndPoint.x() - aFirstStartPoint.x();
    double uy = aFirstEndPoint.y() - aFirstStartPoint.y();
    double uz = aFirstEndPoint.z() - aFirstStartPoint.z();

    double vx = aSecondEndPoint.x() - aSecondStartPoint.x();
    double vy = aSecondEndPoint.y() - aSecondStartPoint.y();
    double vz = aSecondEndPoint.z() - aSecondStartPoint.z();

    double wx = aFirstStartPoint.x() - aSecondStartPoint.x();
    double wy = aFirstStartPoint.y() - aSecondStartPoint.y();
    double wz = aFirstStartPoint.z() - aSecondStartPoint.z();

    double uu = (ux * ux + uy * uy + uz * uz);
    double uv = (ux * vx + uy * vy + uz * vz);
    double vv = (vx * vx + vy * vy + vz * vz);
    double uw = (ux * wx + uy * wy + uz * wz);
    double vw = (vx * wx + vy * wy + vz * wz);
    double dt = uu * vv - uv * uv;

    double sd = dt;
    double td = dt;

    double sn = 0.0;
    double tn = 0.0;

    if (isEqualDouble(dt, 0.0))
    {
        sn = 0.0;
        sd = 1.00;
        tn = vw;
        td = vv;
    }
    else
    {
        sn = (uv * vw - vv * uw);
        tn = (uu * vw - uv * uw);
        if (sn < 0.0)
        {
            sn = 0.0;
            tn = vw;
            td = vv;
        }
        else if (sn > sd)
        {
            sn = sd;
            tn = vw + uv;
            td = vv;
        }
    }

    if(tn < 0.0)
    {
        tn = 0.0;
        if(-uw < 0.0)
        {
            sn = 0.0;
        }
        else if(-uw > uu)
        {
            sn = sd;
        }
        else
        {
            sn = -uw;
            sd = uu;
        }
    }
    else if(tn > td)
    {
        tn = td;
        if((-uw + uv) < 0.0)
        {
            sn = 0.0;
        }
        else if((-uw + uv) > uu)
        {
            sn = sd;
        }
        else
        {
            sn = (-uw + uv);
            sd = uu;
        }
    }

    double sc = 0.0;
    double tc = 0.0;

    if(isEqualDouble(sn, 0.0))
    {
        sc = 0.0;
    }
    else
    {
        sc = sn / sd;
    }

    if (isEqualDouble(tn, 0.0))
    {
        tc = 0.0;
    }
    else
    {
        tc = tn / td;
    }

    double dx = wx + (sc * ux) - (tc * vx);
    double dy = wy + (sc * uy) - (tc * vy);
    double dz = wz + (sc * uz) - (tc * vz);
    return dx * dx + dy * dy + dz * dz;
}

void StrokeIntersector::intersect(osgUtil::IntersectionVisitor& aIntersectionVisitor, osg::Drawable* aDrawable)
{
    osg::BoundingBox bb = aDrawable->getBoundingBox();
    bb.xMin() -= m_offset;
    bb.xMax() += m_offset;
    bb.yMin() -= m_offset;
    bb.yMax() += m_offset;
    bb.zMin() -= m_offset;
    bb.zMax() += m_offset;

    osg::Vec3d start(_start);
    osg::Vec3d end(_end);
    if (!intersectAndClip(start, end, bb))
    {
        return;
    }

    if (aIntersectionVisitor.getDoDummyTraversal())
    {
        return;
    }

    osg::Geometry* geometry = aDrawable->asGeometry();
    if (geometry)
    {
        osg::Vec3dArray* vertices = dynamic_cast<osg::Vec3dArray*>(geometry->getVertexArray());
        if (!vertices)
        {
            return;
        }

        osg::Vec3d dir = end - start;
        for (unsigned int i = 0; i < vertices->size() - 1; ++i)
        {
            double distance = squreDistanceSegmentToSegment((*vertices)[i],
                                                            (*vertices)[i+1],
                                                            start,
                                                            end);
            if (m_offset < distance)
            {
                continue;
            }

            Intersection intersection;
            intersection.ratio = distance;
            intersection.nodePath = aIntersectionVisitor.getNodePath();
            intersection.drawable = aDrawable;
            intersection.matrix = aIntersectionVisitor.getModelMatrix();
            intersection.localIntersectionPoint = (*vertices)[i];
            insertIntersection(intersection);
        }
    }
}
