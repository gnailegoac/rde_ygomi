/*******************************************************************************
*                       Continental Confidential
*                  Copyright (c) Continental AG, 2017

*
*      This software is furnished under license and may be used or
*      copied only in accordance with the terms of such license.
*******************************************************************************
* @file   geo_alg.h
* @brief
*       This file declare the interface of sdk api
*
*
* Change Log:
*      Date              Who               What
*      2017.10.01        Lindun Tang       Create
*******************************************************************************/

#pragma once

#ifndef __GEO_MATH_GEOMETRY_H__
#define __GEO_MATH_GEOMETRY_H__

#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include <assert.h>

/* Error codes. */
typedef int GEO_STATUS;
#define GEO_SUCCESS              0   /**< Success. */
#define GEO_FAIL                -1   /**< Common error. */
#define GEO_INVALID_PARAMETER   -2   /**< Invalid parameter. */
#define GEO_NULL_PARAMETER      -3   /**< NULL parameter. */
#define GEO_DIVIDE_ZERO         -4   /**< Divide by zero. */
#define GEO_DISTANCE_NOT_EXHAUSTED 101 /**< Request distance not exhausted. */
/**
 * @brief Struct to define a point or vector.
 */
typedef struct _vec3d {
    double x;    /**< x component. */
    double y;    /**< y component. */
    double z;    /**< z component. */
} vec3d_t;

/**
 * @brief Struct to define the attitude using Euler angle, the unit is rad.
 * @note The rotate sequence is roll, pitch, yaw. The degree is positive if rotate counter clockwise.
 */
typedef struct _attitude {
    double yaw;         /**< The angle rotate with z axis. */
    double pitch;       /**< The angle rotate with x axis. */
    double roll;        /**< The angle rotate with y axis. */
} attitude_t;

/**
 * @brief Struct to define coeficients of cubic function, the equation is f(t) = a + b*t + c*t^2 + d*t^3.
 */
typedef struct _poly3_coef {
    double a;    /**< The a coeficient. */
    double b;    /**< The b coeficient. */
    double c;    /**< The c coeficient. */
    double d;    /**< The d coeficient. */
} poly3_coef_t;

/**
 * @brief Struct to define a cubic curve.
 */
typedef struct _poly3_curve {
    poly3_coef_t fx;     /**< The cubic function coeficient for x. */
    poly3_coef_t fy;     /**< The cubic function coeficient for y. */
    poly3_coef_t fz;     /**< The cubic function coeficient for z. */
    double t_min;        /**< The minimum value of argument t. */
    double t_max;        /**< The maximum value of argument t. */
} poly3_curve_t;

/**
 * @brief To get the length of a poly2 curve using calculus method
 * @param[IN] aCurve             is the curve to be calculated.
 * @param[IN] aMin               is the length measure start point.
 * @param[IN] aMax               is the length measure stop point.
 * @return the length from aMin to aMax on curve.
 */
double  get_poly2_Len(const poly3_curve_t* aCurve, double aMin, double aMax);

inline void reset(vec3d_t &pt)
{
    pt.x = 0;
    pt.y = 0;
    pt.z = 0;
}

#include <vector>

namespace YGEO {

class NURBS
{
public:
    NURBS(const std::vector<vec3d_t> &aCtrlPts, const std::vector<double> &aKnots,     //construct with control points and knots vector
          double aMin = 0.0f, double aMax = 1.0f);                                      //default param range is [0, 1]
    NURBS(const NURBS& nurbs_curve);                                                  //copy constructor
    vec3d_t GetPoint(double u) const;                                                  //given param u, get the corresponding point
    double   GetLengthByPoint(double aMin, double aMax) const;                           //get the length of a nurbs curve by adding up all the line segment
    double   GetLengthByCurve() const;                                                 //get the length of a nurbs curve by adding up connected poly2 curve
    void    GetParamRange(double &aMin, double &aMax) const;                          //get the parameter range of a nurbs curve
    const std::vector<poly3_curve_t>& GetCurve() const;                               //get curve converted from nurbs
    const std::vector<vec3d_t>&       GetCtrlPoints() const;                          //get the control points
    const std::vector<double>&         GetKnots() const;                               //get the knots
    const poly3_curve_t&              FindCurve(double t) const;                      //find corresponding curve according to parameter
    double  GetLength(double aMin, double aMax) const;                                  //calc nurbs length from 'aMin' to 'aMax'

private:
    NURBS();                                                                          //default constructor
    int32_t findSpan(double u) const;                                                  //given param u, find the corresponding knot area
    void    basisFun(double u, int32_t k, std::vector<double> &N) const;                //given param u, calculate all the k degree basis function value
    void    convertToPoly2();                                                         //convert nurbs to serverl poly2 cure
    double   getPoly2Len(const poly3_curve_t* aCurve, double aMin, double aMax) const;   //get the length of a poly2 curve
    double  getNearestPointPoly2(const poly3_curve_t* aCurve, const vec3d_t &point,   //get the nearest point to a poly2 curve
        double aMin, double aMax, double &t,vec3d_t &foot)const;                        //by calculus method
    double  getNearestPointSubdivision(double begin, double end, int step,            //subdivide parameter from begin to end, with respect to point 'p'
        const vec3d_t p, vec3d_t &foot, double &t) const;                             //get the nearest point on the curve

private:
    std::vector<poly3_curve_t> vecCurve;                                              //the poly2 curves converted from nurbs
    std::vector<vec3d_t>       vecCtrlPoints;                                         //the control points of a nurbs
    std::vector<double>         vecKnots;                                              //the knots vector of a nurbs
    int32_t                    degree;                                                //the degress of a nurbs, current been set to 2
    double                      uMin;                                                  //the min value of nurbs domain of definition, current set to 0
    double                      uMax;                                                  //the max value of nubrs domain of definition, current set to 1
};

}


#endif
