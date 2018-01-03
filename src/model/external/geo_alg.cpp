/*******************************************************************************
*                       Continental Confidential
*                  Copyright (c) Continental AG, 2017

*
*      This software is furnished under license and may be used or
*      copied only in accordance with the terms of such license.
*******************************************************************************
* @file   geo_alg.cpp
* @brief
*       This file define the interface of sdk api
*
*
* Change Log:
*      Date              Who               What
*      2017.10.01        Lindun Tang       Create
*******************************************************************************/
#include "geo_alg.h"
#include <string.h>
#include <cmath>
#include <string>

static const int DIVISION = 500;

using namespace std;
#define NEAR_ZERO(expr) std::fabs(expr) < 1e-10
#define DOUBLE_EQ(a, b) NEAR_ZERO(a - b)
#define DELT 1E-10

double get_para_poly3_value(poly3_coef_t fx, double u)
{
    return fx.a + fx.b * u + fx.c * u * u + fx.d * u * u * u;
}

double get_poly2_Len(const poly3_curve_t* aCurve, double aMin, double aMax)
{
    double order_0 = aCurve->fx.b * aCurve->fx.b + aCurve->fy.b * aCurve->fy.b + aCurve->fz.b * aCurve->fz.b;
    double order_1 = 4 * (aCurve->fx.b * aCurve->fx.c + aCurve->fy.b * aCurve->fy.c + aCurve->fz.b * aCurve->fz.c);
    double order_2 = 4 * (aCurve->fx.c * aCurve->fx.c + aCurve->fy.c * aCurve->fy.c + aCurve->fz.c * aCurve->fz.c);
    double length = 0;
    if (NEAR_ZERO(aCurve->fx.c) && NEAR_ZERO(aCurve->fy.c) && NEAR_ZERO(aCurve->fz.c))
    {
        length = (aMax - aMin) * sqrt(aCurve->fx.b * aCurve->fx.b + aCurve->fy.b * aCurve->fy.b + aCurve->fz.b * aCurve->fz.b);
    }
    else
    {
        double scale = sqrt(order_2);
        double new_order_0 = order_0 / order_2;
        double new_order_1 = order_1 / order_2;
        if (NEAR_ZERO(new_order_0 - 0.25 * new_order_1 * new_order_1))
        {
            length = (0.5 * aMax * aMax + 0.5 * new_order_1 * aMax) - (0.5 * aMin * aMin + 0.5 * new_order_1 * aMin);
            if (length < 0)
            {
                length = -length;
            }
            length = length * scale;
        }
        else
        {
            double scale_2 = sqrt(new_order_0 - 0.25 * new_order_1 * new_order_1);
            double a = 0.5 / scale_2;
            double b = 0.5 * new_order_1 / scale_2;
            double abMax = 2 * a * aMax + b;
            double abMin = 2 * a * aMin + b;
            double value_max = abMax * sqrt(1 + abMax * abMax) / (4 * a) + log((sqrt(1 + abMax * abMax) + abMax) / (sqrt(1 + abMax * abMax) - abMax)) / (8 * a);
            double value_min = abMin * sqrt(1 + abMin * abMin) / (4 * a) + log((sqrt(1 + abMin * abMin) + abMin) / (sqrt(1 + abMin * abMin) - abMin)) / (8 * a);
            length = (value_max - value_min) * scale_2 * scale;
        }
    }
    return length;
}

namespace YGEO
{

NURBS::NURBS():
    degree(0), uMin(0.0), uMax(0.0)
{
}

NURBS::NURBS(const std::vector<vec3d_t>& aCtrlPts, const std::vector<double>& aKnots, double aMin, double aMax):
    uMin(aMin), uMax(aMax)
{
    for (const auto& pt : aCtrlPts)
    {
        vecCtrlPoints.push_back(pt);
    }
    for (const auto& t : aKnots)
    {
        vecKnots.push_back(t);
    }
    degree = aKnots.size() - aCtrlPts.size() - 1;
    convertToPoly2();
}

NURBS::NURBS(const NURBS& nurbs_curve):
    uMin(nurbs_curve.uMin), uMax(nurbs_curve.uMax)
{
    for (const auto& pt : nurbs_curve.GetCtrlPoints())
    {
        vecCtrlPoints.push_back(pt);
    }
    for (const auto& t : nurbs_curve.GetKnots())
    {
        vecKnots.push_back(t);
    }
    degree = vecKnots.size() - vecCtrlPoints.size() - 1;
    convertToPoly2();
}

const std::vector<vec3d_t>& NURBS::GetCtrlPoints() const
{
    return vecCtrlPoints;
}

const std::vector<double>& NURBS::GetKnots() const
{
    return vecKnots;
}

const std::vector<poly3_curve_t>& NURBS::GetCurve() const
{
    return vecCurve;
}

void  NURBS::GetParamRange(double& aMin, double& aMax) const
{
    aMin = uMin;
    aMax = uMax;
}

int32_t NURBS::findSpan(double u) const
{
    int32_t n = static_cast<int32_t>(vecCtrlPoints.size()) - 1;
    int32_t low = degree;
    int32_t high = n + 1;
    int32_t mid = low + (high - low) / 2;

    while(1)
    {
        if (u < vecKnots[mid])
        {
            high = mid;
        }
        else if (u > vecKnots[mid + 1])
        {
            low = mid;
        }
        else
        {
            break;
        }
        mid = low + (high - low) / 2;
    }
    return mid;
}

const  poly3_curve_t&   NURBS::FindCurve(double t) const
{
    if (DOUBLE_EQ(t, uMin))
    {
        t = uMin;
    }
    if (DOUBLE_EQ(t, uMax))
    {
        t = uMax;
    }
    assert(t >= uMin && t <= uMax);
    std::vector<poly3_curve_t>::const_iterator iter;
    for(iter = vecCurve.begin(); iter != vecCurve.end(); iter++)
    {
        if (iter->t_min <= t && iter->t_max >= t)
        {
            return *iter;
        }
    }
    return *(vecCurve.end());
}

void NURBS::basisFun(double u, int32_t k, std::vector<double>& N) const
{
    int32_t p = degree;
    std::vector<double> left(p + 1, 0.0f);
    std::vector<double> right(p + 1, 0.0f);

    N.clear();
    N.resize(p + 1, 0.0f);
    N[0] = 1.0f;

    double saved, tmp;
    for (int32_t j = 1; j <= p; ++j)
    {
        left[j] = u - vecKnots[k + 1 - j];
        right[j] = vecKnots[k + j] - u;
        saved = 0.0f;

        for (int32_t r = 0; r < j; ++r)
        {
            tmp = N[r] / (right[r + 1] + left[j - r]);
            N[r] = saved + right[r + 1] * tmp;
            saved = left[j - r] * tmp;
        }

        N[j] = saved;
    }
}

vec3d_t NURBS::GetPoint(double u) const
{
#if 0
    std::vector<double> N;  // N contain the non-zero elements of knots interval.
    GEO_LOG("[GEO]findSpan begin\n");
    int32_t k = findSpan(u);
    GEO_LOG("[GEO]findSpan end\n");
    int32_t tmpInd = k - degree;
    vec3d_t pt = {0, 0, 0};
    basisFun(u, k, N);
    reset(pt);
    for (int32_t i = 0; i <= degree; i++)
    {
        pt.x += N[i] * vecCtrlPoints[tmpInd + i].x;
        pt.y += N[i] * vecCtrlPoints[tmpInd + i].y;
        pt.z += N[i] * vecCtrlPoints[tmpInd + i].z;
    }
    return pt;
#endif

    vec3d_t pt = {0, 0, 0};
    for(size_t i = 0; i < vecCurve.size(); i++)
    {
        if (u >= vecCurve[i].t_min && u <= vecCurve[i].t_max)
        {
            pt.x = get_para_poly3_value(vecCurve[i].fx, u);
            pt.y = get_para_poly3_value(vecCurve[i].fy, u);
            pt.z = get_para_poly3_value(vecCurve[i].fz, u);
            break;
        }
    }
    return pt;
}

void NURBS::convertToPoly2()
{
    for (size_t i = 0; i < vecKnots.size() - 1; i++)
    {
        if (vecKnots[i] < vecKnots[i + 1])
        {
            poly3_curve_t curve;
            double N_i22_A = 0;
            double N_i22_B = 0;
            double N_i22_C = 0;
            double N_i12_A = 0;
            double N_i12_B = 0;
            double N_i12_C = 0;
            double N_i02_A = 0;
            double N_i02_B = 0;
            double N_i02_C = 0;

            if (i >= 2 && i <= vecKnots.size() - 2)
            {
                double denominator_i22 = (vecKnots[i + 1] - vecKnots[i - 1]) * (vecKnots[i + 1] - vecKnots[i]);
                N_i22_A = vecKnots[i + 1] * vecKnots[i + 1] / denominator_i22;
                N_i22_B = (-2 * vecKnots[i + 1]) / denominator_i22;
                N_i22_C = 1 / denominator_i22;
            }

            if (i >= 1 && i <= vecKnots.size() - 3)
            {
                double denominator_i12_l = (vecKnots[i + 1] - vecKnots[i - 1]) * (vecKnots[i + 1] - vecKnots[i]);
                double denominator_i12_r = (vecKnots[i + 2] - vecKnots[i]) * (vecKnots[i + 1] - vecKnots[i]);
                N_i12_A = -vecKnots[i - 1] * vecKnots[i + 1] / denominator_i12_l - vecKnots[i] * vecKnots[i + 2] / denominator_i12_r;
                N_i12_B = (vecKnots[i - 1] + vecKnots[i + 1]) / denominator_i12_l + (vecKnots[i] + vecKnots[i + 2]) / denominator_i12_r;
                N_i12_C = -1 / denominator_i12_l - 1 / denominator_i12_r;
            }

            if (i < vecKnots.size() - 3)
            {
                double denominator_i02 = (vecKnots[i + 2] - vecKnots[i]) * (vecKnots[i + 1] - vecKnots[i]);
                N_i02_A = vecKnots[i] * vecKnots[i] / denominator_i02;
                N_i02_B = (-2 * vecKnots[i]) / denominator_i02;
                N_i02_C = 1 / denominator_i02;
            }

            vec3d_t point_i_2 = {0, 0, 0};
            vec3d_t point_i_1 = {0, 0, 0};
            vec3d_t point_i_0 = {0, 0, 0};
            if (i  >= 2 && i <= vecCtrlPoints.size() + 1)
            {
                point_i_2.x = vecCtrlPoints[i - 2].x;
                point_i_2.y = vecCtrlPoints[i - 2].y;
                point_i_2.z = vecCtrlPoints[i - 2].z;
            }
            if (i >= 1 && i <= vecCtrlPoints.size())
            {
                point_i_1.x = vecCtrlPoints[i - 1].x;
                point_i_1.y = vecCtrlPoints[i - 1].y;
                point_i_1.z = vecCtrlPoints[i - 1].z;
            }
            if ((int)i >= 0 && i <= vecCtrlPoints.size() - 1)
            {
                point_i_0.x = vecCtrlPoints[i].x;
                point_i_0.y = vecCtrlPoints[i].y;
                point_i_0.z = vecCtrlPoints[i].z;
            }

            curve.fx.a = N_i22_A * point_i_2.x + N_i12_A * point_i_1.x + N_i02_A * point_i_0.x;
            curve.fx.b = N_i22_B * point_i_2.x + N_i12_B * point_i_1.x + N_i02_B * point_i_0.x;
            curve.fx.c = N_i22_C * point_i_2.x + N_i12_C * point_i_1.x + N_i02_C * point_i_0.x;
            curve.fx.d = 0;
            curve.fy.a = N_i22_A * point_i_2.y + N_i12_A * point_i_1.y + N_i02_A * point_i_0.y;
            curve.fy.b = N_i22_B * point_i_2.y + N_i12_B * point_i_1.y + N_i02_B * point_i_0.y;
            curve.fy.c = N_i22_C * point_i_2.y + N_i12_C * point_i_1.y + N_i02_C * point_i_0.y;
            curve.fy.d = 0;
            curve.fz.a = N_i22_A * point_i_2.z + N_i12_A * point_i_1.z + N_i02_A * point_i_0.z;
            curve.fz.b = N_i22_B * point_i_2.z + N_i12_B * point_i_1.z + N_i02_B * point_i_0.z;
            curve.fz.c = N_i22_C * point_i_2.z + N_i12_C * point_i_1.z + N_i02_C * point_i_0.z;
            curve.fz.d = 0;


            curve.t_min = vecKnots[i];
            curve.t_max = vecKnots[i + 1];

            vecCurve.push_back(curve);
        }
    }
}

double NURBS::GetLengthByPoint(double aMin, double aMax) const
{
    if (DOUBLE_EQ(aMin, uMin))
    {
        aMin = uMin;
    }
    if (DOUBLE_EQ(aMax, uMax))
    {
        aMax = uMax;
    }
    assert(aMin <= aMax && uMin <= aMin && aMax <= uMax);
    if (DOUBLE_EQ(aMin, aMax))
    {
        return 0.0;
    }
    double step = (aMax - aMin) / 500.0;
    vec3d_t point_l = {0, 0, 0};
    vec3d_t point_r = {0, 0, 0};
    double param = aMin;
    point_r = GetPoint(aMin);
    double length = 0;

    for (; param < aMax; )
    {
        param += step;
        if (param > aMax)
        {
            param = aMax;
        }
        memcpy(&point_l, &point_r, sizeof(vec3d_t));
        point_r = GetPoint(param);
        length += sqrt((point_r.x - point_l.x) * (point_r.x - point_l.x) +
                       (point_r.y - point_l.y) * (point_r.y - point_l.y) +
                       (point_r.z - point_l.z) * (point_r.z - point_l.z));
    }

    return length;
}

double NURBS::GetLengthByCurve() const
{
    double length = 0;
    for (const auto iter : vecCurve)
    {
        length += get_poly2_Len(&iter, iter.t_min, iter.t_max);
    }
    return length;
}

double NURBS::getNearestPointSubdivision(double begin, double end, int step, const vec3d_t p, vec3d_t& foot, double& t) const
{
    double  min_len = DBL_MAX;
    double  dt = (end - begin) / step;
    double  tm = begin;
    double  d = 0;
    vec3d_t current_p;

    while (tm <= end)
    {
        current_p = GetPoint(tm);
        d = (current_p.x - p.x) * (current_p.x - p.x) + (current_p.y - p.y) * (current_p.y - p.y) + (current_p.z - p.z) * (current_p.z - p.z);
        if (d < min_len)
        {
            foot = current_p;
            t = tm;
            min_len = d;
        }
        tm += dt;
    }

    if (tm - dt < end)
    {
        current_p = GetPoint(end);
        d = (current_p.x - p.x) * (current_p.x - p.x) + (current_p.y - p.y) * (current_p.y - p.y) + (current_p.z - p.z) * (current_p.z - p.z);
        if (d < min_len)
        {
            foot = current_p;
            t = end;
            min_len = d;
        }
    }

    return min_len;
}

double NURBS::getPoly2Len(const poly3_curve_t* aCurve, double aMin, double aMax) const
{
    double order_0 = aCurve->fx.b * aCurve->fx.b + aCurve->fy.b * aCurve->fy.b + aCurve->fz.b * aCurve->fz.b;
    double order_1 = 4 * (aCurve->fx.b * aCurve->fx.c + aCurve->fy.b * aCurve->fy.c + aCurve->fz.b * aCurve->fz.c);
    double order_2 = 4 * (aCurve->fx.c * aCurve->fx.c + aCurve->fy.c * aCurve->fy.c + aCurve->fz.c * aCurve->fz.c);
    double length = 0;
    if (NEAR_ZERO(aCurve->fx.c) && NEAR_ZERO(aCurve->fy.c) && NEAR_ZERO(aCurve->fz.c))
    {
        length = (aMax - aMin) * sqrt(aCurve->fx.b * aCurve->fx.b + aCurve->fy.b * aCurve->fy.b + aCurve->fz.b * aCurve->fz.b);
    }
    else
    {
        double scale = sqrt(order_2);
        double new_order_0 = order_0 / order_2;
        double new_order_1 = order_1 / order_2;
        if (NEAR_ZERO(new_order_0 - 0.25 * new_order_1 * new_order_1))
        {
            length = (0.5 * aMax * aMax + 0.5 * new_order_1 * aMax) - (0.5 * aMin * aMin + 0.5 * new_order_1 * aMin);
            if (length < 0)
            {
                length = -length;
            }
            length = length * scale;
        }
        else
        {
            double scale_2 = sqrt(new_order_0 - 0.25 * new_order_1 * new_order_1);
            double a = 0.5 / scale_2;
            double b = 0.5 * new_order_1 / scale_2;
            double abMax = 2 * a * aMax + b;
            double abMin = 2 * a * aMin + b;
            double value_max = abMax * sqrt(1 + abMax * abMax) / (4 * a) + log((sqrt(1 + abMax * abMax) + abMax) / (sqrt(1 + abMax * abMax) - abMax)) / (8 * a);
            double value_min = abMin * sqrt(1 + abMin * abMin) / (4 * a) + log((sqrt(1 + abMin * abMin) + abMin) / (sqrt(1 + abMin * abMin) - abMin)) / (8 * a);
            length = (value_max - value_min) * scale_2 * scale;
        }
    }

    return length;
}

double NURBS::GetLength(double aMin, double aMax) const
{
    const  poly3_curve_t& curveBegin = FindCurve(aMin);
    const  poly3_curve_t& curveEnd = FindCurve(aMax);
    const  poly3_curve_t* tempCurve;
    double length = 0;
    for (tempCurve = &curveBegin; tempCurve <= &curveEnd; tempCurve++)
    {
        if (&curveBegin == &curveEnd)
        {
            length += getPoly2Len(tempCurve, aMin, aMax);
        }
        else if (&curveBegin != &curveEnd && tempCurve == &curveBegin)
        {
            length += getPoly2Len(tempCurve, aMin, tempCurve->t_max);
        }
        else if (&curveBegin != &curveEnd && tempCurve == &curveEnd)
        {
            length += getPoly2Len(tempCurve, tempCurve->t_min, aMax);
        }
        else
        {
            length += getPoly2Len(tempCurve, tempCurve->t_min, tempCurve->t_max);
        }
    }
    return length;
}

}
