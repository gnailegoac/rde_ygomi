/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file   NURBS.h
 * @brief  Header of NURBS fitting method.
 ********************************************************************************
*/

#ifndef NURBS_H_
#define NURBS_H_

#include <numeric>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core/types.hpp>
#include <cmath>
#include <sstream>

//#include <pcl/point_cloud.h>
//#include <pcl/kdtree/kdtree_flann.h>
//#include <boost/graph/graph_concepts.hpp>
//#include "PerformanceAnalysis/PerformAnalysisInterface.h"

//#include "LogWrapper/LogWrapper.h"
//#include "CommunicateDef/RdbV2SGeometry.h"
//#include "CommunicateDef/RdbV2SRoadObject.h"
//#include "algoInterface/IRoad.h"
// #include <eigen/Eigen/src/plugins/ArrayCwiseBinaryOps.h>

namespace roadDBCore
{

/**
 * A class which implements NURBS fitting method
 */
class NURBS
{

    /**
     * Configuration parameters for NURBS fitting method
     */
public:
    struct ConfigParam
    {
        ConfigParam(const int32_t order = 3,
                    const int32_t maxNumPoints = INT_MAX,
                    const int32_t minNumCtrPoints = 7,
                    const float minCtrPointsRatio = 1.f / 300,
                    const float maxCtrPointsRatio = 1.f / 10,
                    const float translation = 1.0f,
                    const float errorThresh = 0.5f)
            : order(order),
              maxNumPoints(maxNumPoints),
              minNumCtrPoints(minNumCtrPoints),
              minCtrPointsRatio(minCtrPointsRatio),
              maxCtrPointsRatio(maxCtrPointsRatio),
              translation(translation),
              errorThresh(errorThresh)
        {
        }

        bool isValid() const
        {
            bool bValid = true;

            if (minCtrPointsRatio >= maxCtrPointsRatio ||
                    minCtrPointsRatio <  FLT_EPSILON ||
                    maxCtrPointsRatio <  FLT_EPSILON ||
                    minCtrPointsRatio >  1.0f ||
                    maxCtrPointsRatio >  1.0f)
            {
                SDOR_LOG_ERROR << "The ratio of the control points are invalid.";
                bValid = false;
            }
            else if (minNumCtrPoints < 3)
            {
                SDOR_LOG_ERROR << "The minimum number of the control points must be large than 3.";
                bValid = false;
            }
            else if (translation < FLT_EPSILON)
            {
                SDOR_LOG_ERROR << "The translation must be positive.";
                bValid = false;
            }
            else if (errorThresh < FLT_EPSILON)
            {
                SDOR_LOG_ERROR << "The error threshold must be large than 1.";
                bValid = false;
            }
            else
            {
            }

            return bValid;
        }

        const int32_t  order;
        const int32_t  maxNumPoints;           // Maximum number of points to fit.
        // If the input points are too many, this process will occupy too much memory and cpu.
        const int32_t  minNumCtrPoints;        // Minimum number of points to fit
        const float    minCtrPointsRatio;      // Used to calculate the original control points.
        // First of all the number of control points is input points divided by minCtrPointsRatio.
        const float    maxCtrPointsRatio;      // The max number of control points.
        const float    translation;            // Used to translate the sigmoid function along x axis
        const float    errorThresh;            // Threshold of error.
    };

private:
    template <class Point3_T>
    class DouglasPeucker
    {
    public:
        explicit DouglasPeucker() : pPointSetRef_(nullptr), mTotalPointNum_(0)
        {
        }

        ~DouglasPeucker() {}

        bool getDPresult(const std::vector<Point3_T>& vPoints,
                         const float& tolerance,
                         std::vector<int32_t>& vResult)
        {
            MONITOR_FUNCTION_PERFORMANCE("NURBS")

            bool bRet = true;
            if (vPoints.empty() || tolerance < FLT_EPSILON)
            {
                SDOR_LOG_ERROR << "The parameter is error.";
                bRet = false;
            }
            else
            {
                pPointSetRef_ = &vPoints;
                mTotalPointNum_ = static_cast<int32_t>(vPoints.size());
                mTags_.resize(mTotalPointNum_, false);

                DouglasPeuckerReduction(0, mTotalPointNum_ - 1, tolerance);

                vResult.clear();
                //first point
                vResult.emplace_back(0);
                for (int index = 1; index < (mTotalPointNum_ - 1); ++index)
                {
                    if (mTags_[index])
                    {
                        vResult.emplace_back(index);
                    }
                }
                //last point
                vResult.emplace_back(mTotalPointNum_ - 1);
            }

            return bRet;
        }

    private:
        void DouglasPeuckerReduction(const int firstPoint, const int lastPoint, const float tolerance)
        {
            float maxDistance(0);
            int indexFarthest(0);

            const auto& point1 = (*pPointSetRef_)[firstPoint];
            const auto& point2 = (*pPointSetRef_)[lastPoint];

            for (int index = firstPoint + 1; index < lastPoint; ++index)
            {
                float distance = PerpendicularDistance(point1, point2, (*pPointSetRef_)[index]);
                if (distance > maxDistance)
                {
                    maxDistance = distance;
                    indexFarthest = index;
                }
            }

            if (maxDistance > tolerance && indexFarthest >= 0)
            {
                mTags_[indexFarthest] = true;
                DouglasPeuckerReduction(firstPoint, indexFarthest, tolerance);
                DouglasPeuckerReduction(indexFarthest, lastPoint, tolerance);
            }
        }

        inline double PerpendicularDistance(const Point3_T& point1,
                                            const Point3_T& point2,
                                            const Point3_T& point3) noexcept
        {
            const cv::Point3d point1d(point1.x, point1.y, point1.z);
            const cv::Point3d point2d(point2.x, point2.y, point2.z);
            const cv::Point3d point3d(point3.x, point3.y, point3.z);

            double A = point2d.z - point1d.z;
            double B = point1d.x - point2d.x;
            double C = point2d.x * point1d.z - point1d.x * point2d.z;

            double denominator = std::sqrt(A * A + B * B), dist(0);

            if (denominator < FLT_EPSILON)
            {
                dist = FLT_MAX;
            }
            else
            {
                dist = std::abs((A * point3d.x + B * point3d.z + C) / denominator);
            }

            return dist;
        }

        const std::vector<Point3_T>* pPointSetRef_;
        std::vector<bool> mTags_;
        int mTotalPointNum_;
    };

private:
    struct ColOfMatrix
    {
        int32_t start_;
        int32_t end_;
        std::vector<double> vec_;
        ColOfMatrix() : start_(-1), end_(0)
        {
        }
    };

    ConfigParam  configPara_;
    size_t       minNumPoint_;
    int32_t      numPointsToFit_;  // Number of points used to fit NURBS curve

public:
    explicit NURBS(const ConfigParam& configPara) : configPara_(configPara), numPointsToFit_(0)
    {
        minNumPoint_ = configPara_.minNumCtrPoints + 2;
    }

    virtual ~NURBS() {};

private:
    template <class Point3_T>
    bool checkFitData(const std::vector<Point3_T>& vInputPoints, const std::vector<std::pair<int32_t, int32_t>>& vEndIndex)
    {
        bool bRet(true);
        if (vInputPoints.size() < minNumPoint_)
        {
            SDOR_LOG_ERROR << "The number of points is too less.";
            bRet = false;
        }
        else if (vEndIndex.empty())
        {
            SDOR_LOG_ERROR << "The end index is empty.";
            bRet = false;
        }
        else if (!configPara_.isValid())
        {
            SDOR_LOG_ERROR << "Configuration parameters are invalid.";
            bRet = false;
        }
        else
        {
            numPointsToFit_ = static_cast<int32_t>(vInputPoints.size());
            for (auto it = vEndIndex.begin(); it < vEndIndex.end() && bRet; ++it)
            {
                if (it->first < 0 || it->first >= numPointsToFit_ ||
                        it->second < 0 || it->second >= numPointsToFit_ ||
                        it->first > it->second)
                {
                    SDOR_LOG_ERROR << "The end index is invalid.";
                    SDOR_LOG_INFO << "number of input points is " << vInputPoints.size();
                    SDOR_LOG_INFO << "vEndIndex.size() = " << vEndIndex.size();
                    std::ostringstream outString;
                    for (const auto& endPair : vEndIndex)
                    {
                        outString << endPair.first << ", " << endPair.second << ", ";
                    }
                    SDOR_LOG_INFO << outString;
                    bRet = false;
                }
            }

            for (auto it1 = vEndIndex.begin(), it2 = it1 + 1; it2 < vEndIndex.end() && bRet; ++it1, ++it2)
            {
                if (it1->second > it2->first)
                {
                    SDOR_LOG_ERROR << "The end index is invalid.";
                    {
                        std::ostringstream outString;
                        for (auto it = vEndIndex.begin(); it < vEndIndex.end(); ++it)
                        {
                            outString << it->first << ", " << it->second << ", ";
                        }
                        SDOR_LOG_INFO << outString;
                    }
                    bRet = false;
                }
            }
        }

        return bRet;
    }

    //return true for sample successful, return false for sample failure, but not error.
    template <class Point3_T>
    bool sampleAndEnd(const std::vector<Point3_T>& vInputPoints,
                      const std::vector<std::pair<int32_t, int32_t>>& vEndIndex,
                      std::vector<Point3_T>& vSamplePoint,
                      std::vector<std::pair<int32_t, int32_t>>& vSampleEndIdx)
    {
        std::vector<int> vSampleIdx;
        bool bRet = subSampleByDistance(vInputPoints, 0.5f, vSampleIdx);
        size_t numSamplePoint = vSampleIdx.size() + static_cast<size_t>(vEndIndex.size() << 1);

        if (bRet && numSamplePoint > minNumPoint_)
        {
            //second of every element is it's type, whether it is end index
            std::vector<std::pair<int, bool>> vIdxAndType, vTmpIdxType;
            vIdxAndType.reserve(numSamplePoint);
            for (auto it = vEndIndex.begin(); it < vEndIndex.end(); ++it)
            {
                vIdxAndType.emplace_back(it->first, true);
                vIdxAndType.emplace_back(it->second, true);
            }

            for (auto it = vSampleIdx.begin(); it < vSampleIdx.end(); ++it)
            {
                vIdxAndType.emplace_back(*it, false);
            }

            //         SDOR_LOG_DEBUG << "vEndIndex.size() = " << vEndIndex.size();
            //         for (auto elem : vEndIndex)
            //         {
            //             std::cout << "[" << elem.first << ", " << elem.second << "]" << ", ";
            //         }
            //         std::cout << std::endl;

            std::stable_sort(vIdxAndType.begin(), vIdxAndType.end(),
                             [](const std::pair<int, bool>& pair1, const std::pair<int, bool>& pair2)
            {
                if (pair1.first < pair2.first)
                {
                    return true;
                }
                else if (pair1.first == pair2.first)
                {
                    return pair1.second;
                }
                else
                {
                    return false;
                }
            });

            //         SDOR_LOG_DEBUG << "vIdxAndType.size() = " << vIdxAndType.size();
            //         for (auto elem : vIdxAndType)
            //         {
            //             std::cout << "[" << elem.first << ", " << elem.second << "]" << ", ";
            //         }
            //         std::cout << std::endl;

            int index(0);
            for (auto it = vIdxAndType.begin(); it < vIdxAndType.end(); ++it)
            {
                if (it->second)
                {
                    index = it->first;
                }
                else if (index == it->first)
                {
                    continue;
                }
                else
                {
                }

                vTmpIdxType.emplace_back(*it);
            }

            vIdxAndType.swap(vTmpIdxType);

            //         SDOR_LOG_DEBUG << "vIdxAndType.size() = " << vIdxAndType.size();
            //         for (auto e : vIdxAndType)
            //         {
            //             std::cout << "[" << e.first << ", " << e.second << "]" << ", ";
            //         }
            //         std::cout << std::endl;

            vSamplePoint.clear();
            vSamplePoint.reserve(vIdxAndType.size());
            vSampleEndIdx.clear();
            vSampleEndIdx.reserve(vEndIndex.size());

            std::pair<int32_t, int32_t> tmpPair;
            bool bFirst = true;
            int i = 0;
            for (auto it = vIdxAndType.begin(); it < vIdxAndType.end(); ++it, ++i)
            {
                vSamplePoint.emplace_back(vInputPoints[it->first]);
                if (it->second)
                {
                    if (bFirst)
                    {
                        tmpPair.first = i;
                    }
                    else
                    {
                        tmpPair.second = i;
                        vSampleEndIdx.emplace_back(tmpPair);
                    }
                    bFirst = !bFirst;
                }
            }
        }
        else
        {
            //sub sample is unnecessary
            bRet = false;
        }

        return bRet;
    }

    template <class Point3_T>
    float getRadius(const Point3_T& point1, const Point3_T& point2, const Point3_T& point3) const
    {
        float difference1 = point2.z - point1.z, difference2 = point2.z - point3.z;
        float denominator = (point2.x - point3.x) * difference1 - (point2.x - point1.x) * difference2;

        float item1 = 0.5f * (point3.z - point1.z) * (point2.z - point3.z) * (point2.z - point1.z);
        float item2 = 0.5f * (point1.x * point1.x - point2.x * point2.x) * (point2.z - point3.z);
        float item3 = 0.5f * (point2.x * point2.x - point3.x * point3.x) * (point2.z - point1.z);

        float radius(0);
        if (denominator < FLT_EPSILON && denominator > -FLT_EPSILON) // denominator == 0
        {
            //             SDOR_LOG_INFO << "The three points are on a line.";
            radius = FLT_MAX;  //the radius is infinite
        }
        else // denominator != 0
        {
            float x = (item1 + item2 + item3) / denominator, z(0);
            if (difference1 > FLT_EPSILON || difference1 < -FLT_EPSILON) // difference1 ！= 0
            {
                z = -(point2.x - point1.x) * (x - 0.5f * (point2.x + point1.x)) / difference1 + 0.5f * (point2.z + point1.z);
            }
            else if (difference2 > FLT_EPSILON || difference2 < -FLT_EPSILON) // difference2 ！= 0
            {
                z = -(point2.x - point3.x) * (x - 0.5f * (point2.x + point3.x)) / difference2 + 0.5f * (point2.z + point3.z);
            }
            else
            {
            }

            float coeff1 = x - point1.x, coeff2 = z - point1.z;
            radius = std::sqrt(coeff1 * coeff1 + coeff2 * coeff2);
        }

        return radius;
    }


    template <class Point3_T>
    bool smooth(const std::vector<Point3_T>& vInputPoint, const float& rThresh, std::vector<int>& vIndex) const
    {
        bool bRet = true;
        if (vInputPoint.empty() || rThresh < FLT_EPSILON || vIndex.empty())
        {
            SDOR_LOG_ERROR << "vInputPoint.size() = " << vInputPoint.size() << ", rThresh = " << rThresh
                           << ", vIndex.size() = " << vIndex.size();
            bRet = false;
        }
        else if (vIndex.front() != 0 || vIndex.back() != static_cast<int>(vInputPoint.size()) - 1)
        {
            SDOR_LOG_ERROR << "The first and last element of vIndex is: " << vIndex.front() << ", " << vIndex.back();
            SDOR_LOG_ERROR <<  "vInputPoint.size() = " << vInputPoint.size();
            bRet = false;
        }
        else
        {
            std::vector<int> vRemove;
            int i = 1;
            for (auto it1 = vIndex.begin(), it2 = it1 + 1, it3 = it2 + 1; it3 < vIndex.end(); ++it2, ++it3, ++i)
            {
                const auto& point1 = vInputPoint[*it1], &point2 = vInputPoint[*it2], &point3 = vInputPoint[*it3];
                if (getRadius(point1, point2, point3) < rThresh)
                {
                    vRemove.emplace_back(i);
                }
                else
                {
                    it1 = it2;
                }
            }

            for (auto it = vRemove.rbegin(); it < vRemove.rend(); ++it)
            {
                vIndex.erase(vIndex.begin() + *it);
            }
        }

        return bRet;
    }

    template <class Point3_T>
    bool splitParamSpace(const std::vector<Point3_T>& vInputPoint,
                         const std::vector<int>& vDPIndex,
                         std::vector<float>& vU,
                         std::vector<float>& vKnot) const
    {
        bool bRet = true;
        if (vInputPoint.empty() || vDPIndex.empty())
        {
            SDOR_LOG_ERROR << "vInputPoint.size() = " << vInputPoint.size() << ", vDPIndex.size() = " << vDPIndex.size();
            bRet = false;
        }
        else if (vDPIndex.front() != 0 || vDPIndex.back() != static_cast<int>(vInputPoint.size()) - 1)
        {
            SDOR_LOG_ERROR << "The first and last element of vDPIndex is: " << vDPIndex.front() << ", " << vDPIndex.back()
                           <<  "vInputPoint.size() = " << vInputPoint.size();
            bRet = false;
        }
        else
        {
            int32_t numDistance = static_cast<int>(vInputPoint.size()) - 1;
            std::vector<float> vDistance;
            vDistance.reserve(numDistance);

            float32_t tmpDistance, tmpDiff;
            for (auto first = vInputPoint.begin(), second = first + 1; second < vInputPoint.end(); ++first, ++second)
            {
                tmpDiff = first->x - second->x;
                tmpDistance = tmpDiff * tmpDiff;

                tmpDiff = first->z - second->z;
                tmpDistance += tmpDiff * tmpDiff;

                vDistance.emplace_back(std::sqrt(tmpDistance));
            }

            float32_t sumDistance = std::accumulate(vDistance.begin(), vDistance.end(), 0.f);

            //Generate the vector U, U is u bar
            vU.clear();
            vU.reserve(vInputPoint.size());

            if (sumDistance < 0.1f)
            {
                SDOR_LOG_ERROR << "The line is too short.";
                bRet = false;
            }
            else
            {
                float32_t divisor = 1.f / sumDistance, elem(0);
                for (auto it = vDistance.begin(); it < vDistance.end(); ++it)
                {
                    vU.emplace_back(elem);
                    elem += (*it) * divisor;
                    if (elem - 1.f > FLT_MIN)
                    {
                        elem = 1.f;
                    }
                }
                vU.emplace_back(1.f);
            }

            if (bRet)
            {
                //generate knot
                vKnot.clear();
                vKnot.reserve(vDPIndex.size() + 4u);
                vKnot.emplace_back(0);
                vKnot.emplace_back(0);

                for (auto it = vDPIndex.begin(); it < vDPIndex.end(); ++it)
                {
                    vKnot.emplace_back(vU[*it]);
                }

                vKnot.emplace_back(1);
                vKnot.emplace_back(1);
            }
        }

        return bRet;
    }


public:
    /**
     * Fit dashed curve.
     *
     * @param origPoints      [ IN] points to fit, x and z are the ground, y is the height
     * @param endPointsIndice [ IN] indices of end points of each piece in input points
     * @param nurbsParam      [OUT] parameters of fitted curve
     *
     * @return true on success, fail otherwise
     */
    template <class Point3_T>
    bool fitCurve(const std::vector<Point3_T>& vInputPoint,
                  const std::vector<std::pair<int32_t, int32_t>>& vEndIndex,
                  roadDBCore::NURBS_t& curveParam)
    {

        bool bRet = checkFitData(vInputPoint, vEndIndex);
        if (bRet)
        {
            const std::vector<Point3_T>* pInputPoint = &vInputPoint;
            const std::vector<std::pair<int32_t, int32_t>>* pEndIdx = &vEndIndex;

            std::vector<Point3_T> vSamplePoint;
            std::vector<std::pair<int32_t, int32_t>> vSampleEndIdx;

            //subSample
            if (sampleAndEnd(vInputPoint, vEndIndex, vSamplePoint, vSampleEndIdx))
            {
                pInputPoint = &vSamplePoint;
                pEndIdx = &vSampleEndIdx;
                numPointsToFit_ = static_cast<int32_t>(vSamplePoint.size());
            }

            DouglasPeucker<Point3_T> DPobj;
            std::vector<int> vDPIndex;
            DPobj.getDPresult(*pInputPoint, 0.1f, vDPIndex);

            std::vector<float> vU;
            std::vector<NURBS::ColOfMatrix> matrixN;
            std::vector<Point3_T> R;
            bRet = smooth(*pInputPoint, 25.f, vDPIndex) &&
                   splitParamSpace(*pInputPoint, vDPIndex, vU, curveParam.vecKnot) &&
                   solveMatrixN(static_cast<int>(curveParam.vecKnot.size()) - 3, curveParam.vecKnot, vU, matrixN) &&
                   generateR(matrixN, *pInputPoint, R) &&
                   solveCtrPoints(*pInputPoint, matrixN, R, curveParam.vecCtrlPoint);

            if (bRet)
            {
                //Generate the endPoints with parameter u
                curveParam.endPoint.clear();
                curveParam.endPoint.reserve(pEndIdx->size() << 1);

                for (auto& endPair : *pEndIdx)
                {
                    if (endPair.first >= static_cast<int32_t>(vU.size()) ||
                            endPair.first < 0 ||
                            endPair.second >= static_cast<int32_t>(vU.size()) ||
                            endPair.second < endPair.first)
                    {
                        SDOR_LOG_ERROR << "not valid endPair data.";
                        {
                            std::ostringstream outString;
                            outString << "number of input points is " << vInputPoint.size()
                                      << "and the vecEndIndex is : " << std::endl;
                            for (auto& endPair : *pEndIdx)
                            {
                                outString << endPair.first << ", " << endPair.second << ", ";
                            }
                            SDOR_LOG_INFO << outString;
                        }
                        bRet = false;
                        break;
                    }

                    curveParam.endPoint.emplace_back(vU[endPair.first]);
                    curveParam.endPoint.emplace_back(vU[endPair.second]);
                }
            }

            if (bRet)
            {
                bRet = calcAccuracyLineLength(*pInputPoint, *pEndIdx, vU, curveParam);
            }
        }

        return bRet;
    }



    bool generateKnot(int32_t numCtrPoitns, const std::vector<float32_t>& U, std::vector<float32_t>& knot) const
    {
        if (numCtrPoitns > 0 && !U.empty() && numCtrPoitns < static_cast<int32_t>(U.size()))
        {
            int32_t degree = configPara_.order - 1;
            int32_t n = numCtrPoitns - 1;
            int32_t numKnot = n + degree + 2;

            knot.clear();
            knot.reserve(numKnot);
            knot.resize(degree + 1, 0);    // The first degree+1 elements are zero.

            if(0 == n - degree + 1)
            {
                SDOR_LOG_ERROR << "n-p+1 value is 0";
                return false;
            }
            float32_t d = static_cast<float32_t>(U.size()) / static_cast<float32_t>(n - degree + 1);
            int32_t index;
            float32_t i, alpha, tmp;
            for (int32_t j = 1; j <= n - degree; ++j)
            {
                i = std::floor(j * d);
                alpha = static_cast<float>(j) * d - i;
                index = static_cast<int32_t>(i);
                tmp = (1 - alpha) * U[index - 1] + alpha * U[index];
                knot.emplace_back(tmp);
            }

            knot.resize(numKnot, 1);  // The last p+1 elements are one.

            return true;
        }
        else
        {
            SDOR_LOG_ERROR << "The arguments are invalid in function generateKnot.";
            return false;
        }
    }


    bool solveMatrixN(int32_t numCtrPoints,
                      const std::vector<float32_t>& knot,
                      const std::vector<float32_t>& U,
                      std::vector<NURBS::ColOfMatrix>& matrixN) const
    {
        int32_t numPoints = static_cast<int32_t>(U.size());
        if(numCtrPoints > numPoints || knot.empty() || U.empty())
        {
            SDOR_LOG_ERROR << "The arguments is error in function solveMatrixN.";
            return false;
        }

        int32_t degree = configPara_.order - 1;
        matrixN.clear();
        matrixN.resize(numCtrPoints);

        int32_t index, s = degree;
        std::vector<float32_t> N;          //It is a temporary variable
        for (int32_t row = 0; row < numPoints; ++row)
        {
            if (!findSpan(numCtrPoints - 1, s, U[row], knot, s))
            {
                return false;
            }

            if (!basisFun(s, U[row], degree, knot, N))
            {
                return false;
            }

            index = s - degree;
            for (int32_t i = 0; i <= degree; ++i)
            {
                if (matrixN[index + i].start_ == -1)
                {
                    matrixN[index + i].start_ = row;
                }
                matrixN[index + i].vec_.emplace_back(N[i]);
            }
        }

        std::for_each(matrixN.begin(), matrixN.end(), [](NURBS::ColOfMatrix & elem)
        {
            elem.end_ = elem.start_ + static_cast<int32_t>(elem.vec_.size());
        });

        return true;
    }


    template <class Point3_T>
    bool solveCtrPoints(const std::vector<Point3_T>& inputPoints,
                        const std::vector<NURBS::ColOfMatrix>& matrixN,
                        const std::vector<Point3_T>& R,
                        std::vector<roadDBCore::Point3f_t>& ctrPoints) const
    {
        //matrixN represent a matrix
        if (inputPoints.empty() || matrixN.empty() || R.empty())
        {
            SDOR_LOG_ERROR << "The arguments are invalid in function solveCtrPoints.";
            return false;
        }

        int32_t cols = static_cast<int32_t>(matrixN.size());  //Every element of matrixN is a column of a matrix
        if (cols - 2 != static_cast<int32_t>(R.size()))
        {
            SDOR_LOG_ERROR << "The vector R does not match the matrix NRIO";
            return false;
        }

        //calculate (matrixN^T * matrixN)
        cv::Mat squareMatrix = cv::Mat::zeros(cols, cols, CV_32F); // square matrix, squareMatrix = matrixN' * matrixN
        auto itr = matrixN.begin();
        for (int32_t r = 0; r < cols; ++r)  // square matrix
        {
            auto p = squareMatrix.ptr<float32_t>(r);
            float32_t tmp(0);
            for (auto itCol = itr->vec_.begin(); itCol < itr->vec_.end(); ++itCol)
            {
                tmp += (*itCol) * (*itCol);
            }
            p[r] = tmp * 0.5f; // because it calculate a half of the matrix only, this function will sum this matrix and it's transposition
            auto itc = itr + 1;

            for (int32_t c = r + 1; c < cols; ++c) // square matrix
            {
                auto length = itr->end_ - itc->start_;
                if (length <= 0)
                {
                    break;
                }
                else if (length > static_cast<int32_t>(itc->vec_.size()))
                {
                    length = itc->vec_.size();
                }
                else
                {
                }

                tmp = 0;
                auto index = itc->start_ - itr->start_;
                for (int32_t i = 0; i < length; ++i)
                {
                    tmp += itr->vec_[index] * itc->vec_[i];
                    ++index;
                }

                p[c] = tmp;
                ++itc;
            }
            ++itr;
        }

        squareMatrix = squareMatrix + squareMatrix.t();

        cv::Rect center(1, 1, cols - 2, cols - 2);
        cv::Mat sMatrix = squareMatrix(center);

        //calculate (matrixN^T * matrixN)
        sMatrix = sMatrix.inv(cv::DECOMP_LU);

        //calculate control points. control points = squareMatrix*R
        ctrPoints.clear();
        ctrPoints.reserve(cols);
        cols = sMatrix.cols;

        roadDBCore::Point3f_t tmpPoint, zeroPoint(0, 0, 0);

        auto& point1 = inputPoints.front();
        tmpPoint.relLon = point1.x;
        tmpPoint.relAlt = point1.y;
        tmpPoint.relLat = point1.z;

        ctrPoints.emplace_back(tmpPoint);  //the first control point is the first point of the curve
        for (int32_t i = 0; i < cols; ++i)
        {
            tmpPoint = zeroPoint;
            float32_t* q = sMatrix.ptr<float32_t>(i);
            auto itR = R.begin();
            for (int32_t j = 0; j < cols; ++j)
            {
                const auto& element = q[j];
                tmpPoint.relLon += itR->x * element;
                tmpPoint.relAlt += itR->y * element;
                tmpPoint.relLat += itR->z * element;
                ++itR;
            }
            ctrPoints.emplace_back(tmpPoint);
        }

        auto& point2 = inputPoints.back();
        tmpPoint.relLon = point2.x;
        tmpPoint.relAlt = point2.y;
        tmpPoint.relLat = point2.z;
        ctrPoints.emplace_back(tmpPoint); //the last control point is the last point of the curve

        return true;
    }


    template <class Point3_T>
    bool generateR(const std::vector<NURBS::ColOfMatrix>& matrixN,
                   const std::vector<Point3_T>& inputPoints,
                   std::vector<Point3_T>& R) const
    {
        if (inputPoints.empty() || matrixN.empty())
        {
            SDOR_LOG_ERROR << "No input Points in generateR";
            return false;
        }

        // It equal to the number of control Points. Every element of matrixN is a column of a matrix
        int32_t cols = static_cast<int32_t>(matrixN.size());
        if (cols < 2)
        {
            SDOR_LOG_ERROR << "data error.";
            return false;
        }
        int32_t rows = static_cast<int32_t>(inputPoints.size());

        std::vector<Point3_T> Rmatrix(rows);
        ColOfMatrix firstCol = matrixN.front(), lastCol = matrixN.back();
        float32_t element1, element2;
        for (int32_t k = 1; k < rows - 1; ++k)
        {
            if (k < firstCol.start_ || k >= firstCol.end_)
            {
                element1 = 0;
            }
            else
            {
                element1 = firstCol.vec_[k - firstCol.start_];
            }

            if (k < lastCol.start_ || k >= lastCol.end_)
            {
                element2 = 0;
            }
            else
            {
                element2 = lastCol.vec_[k - lastCol.start_];
            }

            //Rmatrix[k] = inputPoints[k] - element1 * inputPoints.front() - element2 * inputPoints.back()
            Rmatrix[k] = subtract(inputPoints[k], add(numericalMultiply(element1, inputPoints.front()), numericalMultiply(element2, inputPoints.back())));
        }

        R.clear();
        R.reserve(cols - 2);

        auto itN = matrixN.begin() + 1;
        int32_t startIdxN, endIdxN, startIdxR;
        Point3_T tmpPoint;
        for (int32_t i = 1; i < cols - 1; ++i)
        {
            if (itN->start_ < 1)
            {
                startIdxR = 1;
                startIdxN = 1 - itN->start_;
            }
            else
            {
                startIdxR = itN->start_;
                startIdxN = 0;
            }

            if (itN->end_ > rows - 1)
            {
                endIdxN = rows - 1 - itN->start_;
            }
            else
            {
                endIdxN = itN->end_ - itN->start_;
            }

            auto it = Rmatrix.begin() + startIdxR;

            reset(tmpPoint);
            for (int32_t j = startIdxN; j < endIdxN; ++j)
            {
                tmpPoint += numericalMultiply(itN->vec_[j], *it);
                ++it;
            }
            R.emplace_back(tmpPoint);
            ++itN;
        }

        return true;
    }


    template <class Point3_T>
    bool generateCurveAndRegion(const roadDBCore::NURBS_t& NURBSParam,
                                const float32_t step,
                                std::vector<Point3_T>& outputPoints,
                                std::vector<size_t>& vRange,
                                std::vector<float32_t>& U) const
    {
        if (NURBSParam.vecCtrlPoint.empty() || step < FLT_EPSILON || NURBSParam.endPoint.empty())
        {
            SDOR_LOG_ERROR << "The arguments are invalid in function generateCurve.";
            return false;
        }

        int32_t outputPointNum = static_cast<int32_t>(std::ceil(NURBSParam.lineLength / step));
        outputPointNum = std::max(outputPointNum, 2);

        if (NURBSParam.vecCtrlPoint.empty())
        {
            SDOR_LOG_ERROR << "Dash line need end points.";
            return false;
        }
        else if ((NURBSParam.endPoint.size() & 0x1) != 0)
        {
            SDOR_LOG_ERROR << "The number of end points must be even.";
            return false;
        }
        else
        {
            float32_t scale = (NURBSParam.endPoint.back() - NURBSParam.endPoint.front()) / (outputPointNum - 1);

            U.clear();
            U.reserve(outputPointNum);

            //fill the vector U with fractional part that is between every pair of endpoints
            //the endPoint is float type.

            float32_t tmp = NURBSParam.endPoint.front();
            while (tmp < NURBSParam.endPoint.back())
            {
                U.emplace_back(tmp);
                tmp += scale;
            }
            U.emplace_back(NURBSParam.endPoint.back());

            if (!outSample(NURBSParam, U, outputPoints, vRange))
            {
                SDOR_LOG_ERROR << "Sample error";
                return false;
            }
        }
        return true;
    }


    //This function is called frequently
    bool basisFun(const int32_t s, const float32_t u, const int32_t p, const std::vector<float32_t>& U, std::vector<float32_t>& N) const
    {
        if (!U.empty())
        {
            std::vector<float32_t> left(p + 1, 0.0f);
            std::vector<float32_t> right(p + 1, 0.0f);

            N.clear();
            N.resize(p + 1, 0.0f);
            N[0] = 1.0f;

            float32_t saved, tmp;
            for (int32_t j = 1; j <= p; ++j)
            {
                left[j] = u - U[s + 1 - j];
                right[j] = U[s + j] - u;
                saved = 0.0f;

                for (int32_t r = 0; r < j; ++r)
                {
                    tmp = N[r] / (right[r + 1] + left[j - r]);
                    N[r] = saved + right[r + 1] * tmp;
                    saved = left[j - r] * tmp;
                }

                N[j] = saved;
            }

            return true;
        }
        else
        {
            SDOR_LOG_DEBUG << "Input data are empty";
            return false;
        }
    }

    //This function is called frequently
    bool findSpan(const int32_t n, const int32_t p, float32_t u, const std::vector<float32_t>& knot, int32_t& s) const
    {
        if (n < static_cast<int32_t>(knot.size()) && p <= n)
        {
            if (u < knot[p])
            {
                s = p;
            }
            else if (u > knot[n])
            {
                s = n;
            }
            else
            {
                int32_t low = p;
                int32_t high = n + 1;
                int32_t mid = low + (high - low) / 2; // mid = floor((high+low)/2)

                while (low + 1 < high) // +1 for low = high -1 case
                {
                    if (u - knot[mid] < -FLT_MIN)
                    {
                        high = mid;
                    }
                    else if (u - knot[mid + 1] > -FLT_MIN)
                    {
                        low = mid;
                    }
                    else
                    {
                        break;
                    }

                    mid = low + (high - low) / 2; // mid = floor((high+low)/2)
                }
                s = mid;
            }

            return true;
        }
        else
        {
            SDOR_LOG_ERROR << "The arguments are invalid in function findSpan.";
            return false;
        }
    }

    template <class Point3_T>
    bool checkResult(const std::vector<Point3_T>&    inputPoints,
                     const roadDBCore::NURBS_t&      curveParam,
                     const std::vector<cv::Point2f>& fitPoints,
                     const float&                    refSquareDist,
                     float&                          coef) const
    {
        if (fitPoints.size() != inputPoints.size())
        {
            SDOR_LOG_ERROR << "The number of fitting points does not match the number of input points in checkResult.";
            return false;
        }
        else if (refSquareDist - configPara_.errorThresh < FLT_EPSILON)
        {
            SDOR_LOG_DEBUG << "Maximum error is too small.";
            return false;
        }
        else
        {
            float error(0), tmp, squareDis;
            auto inputIt = inputPoints.begin();
            auto fitIt = fitPoints.begin();
            while (inputIt < inputPoints.end())
            {
                tmp = inputIt->x - fitIt->x;
                squareDis = tmp * tmp;

                tmp = inputIt->z - fitIt->y;
                squareDis += tmp * tmp;

                error += std::sqrt(squareDis);

                ++inputIt;
                ++fitIt;
            }

            coef = error / refSquareDist - configPara_.errorThresh;

            return true;
        }
    }


    bool lineLength(const std::vector<cv::Point2f>& fitPoints,
                    const std::vector<std::pair<int32_t, int32_t>>& vecEndIndex,
                    float32_t& lineLength,
                    float32_t& paintTotalLength) const
    {
        if (!fitPoints.empty())
        {
            lineLength = 0.0f;
            paintTotalLength = 0.0f;
            float32_t tmp(0);
            int32_t i(0);

            //first point the first point of the segment, and second point the last, not the next of the last
            for (auto& endPair : vecEndIndex)
            {
                while (i < endPair.first)
                {
                    tmp = cv::norm(fitPoints[i + 1] - fitPoints[i]);
                    lineLength += tmp;
                    ++i;
                }

                while (i < endPair.second)
                {
                    tmp = cv::norm(fitPoints[i + 1] - fitPoints[i]);
                    paintTotalLength += tmp;
                    lineLength += tmp;
                    ++i;
                }
            }

            return true;
        }
        else
        {
            SDOR_LOG_ERROR << "The arguments are invalid in function lineLength.";
            return false;
        }
    }


    bool calcAccuracyLineLength(const std::vector<cv::Point3f>& inputPoints,
                                const std::vector<std::pair<int32_t, int32_t>>& vecEndIndex,
                                const std::vector<float>& denseU,
                                roadDBCore::NURBS_t& curveParam) const
    {
        bool bRet = false;
        std::vector<cv::Point3f> fitPoints;
        if (!denseU.empty() && !inputPoints.empty())
        {
            bRet = outSample(curveParam, denseU, fitPoints);
        }

        if (bRet)
        {
            std::vector<std::pair<int32_t, int32_t>> vecDenseEndIndex;
            pcl::PointCloud<pcl::PointXY>::Ptr cloud(new pcl::PointCloud<pcl::PointXY>);
            if (nullptr == cloud)
            {
                SDOR_LOG_ERROR << "PointCloud Ptr alloc error";
                return false;
            }

            // Generate pointcloud data
            cloud->width = fitPoints.size();
            cloud->height = 1;
            cloud->points.resize(cloud->width * cloud->height);

            auto itCloud = cloud->points.begin();
            auto itFit = fitPoints.begin();
            for ( ; itCloud < cloud->points.end(); ++itCloud, ++itFit)
            {
                itCloud->x = itFit->x;
                itCloud->y = itFit->z;
            }

            pcl::KdTreeFLANN<pcl::PointXY> kdtree;
            kdtree.setInputCloud(cloud);

            // K nearest neighbor search
            const int32_t K = 10;

            std::vector<int32_t> pointIdxNKNSearchFitst(K), pointIdxNKNSearchSecond(K);
            std::vector<float> pointNKNSquaredDistanceFirst(K), pointNKNSquaredDistanceSecond(K);

            std::vector<float>::iterator itFirst, itSecond;
            pcl::PointXY searchFirstPoint, searchSecondPoint;

            for (auto itEndIdx = vecEndIndex.begin(); itEndIdx < vecEndIndex.end() && bRet; ++itEndIdx)
            {
                searchFirstPoint.x = inputPoints[itEndIdx->first].x;
                searchFirstPoint.y = inputPoints[itEndIdx->first].z;

                searchSecondPoint.x = inputPoints[itEndIdx->second].x;
                searchSecondPoint.y = inputPoints[itEndIdx->second].z;

                int minIdx(0), idx1(-1), idx2(-1);

                if (kdtree.nearestKSearch(searchFirstPoint, K, pointIdxNKNSearchFitst, pointNKNSquaredDistanceFirst) > 0 &&
                        kdtree.nearestKSearch(searchSecondPoint, K, pointIdxNKNSearchSecond, pointNKNSquaredDistanceSecond) > 0)
                {
                    for (auto it = pointIdxNKNSearchFitst.begin(); it < pointIdxNKNSearchFitst.end(); ++it)
                    {
                        if (*it >= minIdx)
                        {
                            idx1 = *it;
                            minIdx = *it;
                            break;
                        }
                    }

                    if (minIdx < idx2)
                    {
                        SDOR_LOG_DEBUG << "calculte length failure.";
                        bRet = false;
                    }

                    for (auto it = pointIdxNKNSearchSecond.begin(); it < pointIdxNKNSearchSecond.end(); ++it)
                    {
                        if (*it >= minIdx)
                        {
                            idx2 = *it;
                            minIdx = *it;
                            break;
                        }
                    }

                    if (minIdx < idx1)
                    {
                        bRet = false;
                    }

                    vecDenseEndIndex.emplace_back(idx1, idx2);
                }
                else
                {
                    SDOR_LOG_DEBUG << "Search point failure";
                    bRet = false;
                }
            }

            if (!vecDenseEndIndex.empty())
            {
                vecDenseEndIndex.front().first = 0;
                vecDenseEndIndex.back().second = static_cast<int>(fitPoints.size()) - 1;
            }

            if (bRet)
            {
                float lineLength(0), paintTotalLength(0);
                float32_t tmp(0);
                int32_t i(0);

                //first point the first point of the segment, and second point the last, not the next of the last
                for (auto& endPair : vecDenseEndIndex)
                {
                    while (i < endPair.first)
                    {
                        tmp = normXZ(fitPoints[i + 1] - fitPoints[i]);
                        lineLength += tmp;
                        ++i;
                    }

                    while (i < endPair.second)
                    {
                        tmp = normXZ(fitPoints[i + 1] - fitPoints[i]);
                        paintTotalLength += tmp;
                        lineLength += tmp;
                        ++i;
                    }
                }

                curveParam.lineLength = lineLength;
                curveParam.paintTotalLength = paintTotalLength;
            }

        }

        return bRet;
    }

    //contain first, doesn't contain last.
    template <class Point3_T>
    bool linearFit(const typename std::vector<Point3_T>::const_iterator first,
                   const typename std::vector<Point3_T>::const_iterator last,
                   cv::Point2f& direction) const
    {
        float num = static_cast<float>(last - first);
        if (num < 2.f)
        {
            SDOR_LOG_ERROR << "No input data in function linearFit!";
            return false;
        }

        float x_mean(0), z_mean(0);    //record the mean position of a paint.
        for (auto it = first; it < last; ++it)
        {
            x_mean += it->x;
            z_mean += it->z;
        }

        x_mean /= num;
        z_mean /= num;

        float covariance(0), x_variance(0);
        float x_temp, z_temp;

        for (auto it = first; it < last; ++it)
        {
            x_temp = it->x - x_mean;
            z_temp = it->z - z_mean;

            covariance += x_temp * z_temp;
            x_variance += x_temp * x_temp;
        }

        float x_sign(0);
        int32_t halfNum = static_cast<int32_t>(std::floor(num * 0.5f));
        for (auto it = first; it < first + halfNum; ++it)
        {
            x_sign -= it->x;
        }

        for (auto it = last - 1; it > last - 1 - halfNum; --it)
        {
            x_sign += it->x;
        }

        x_sign -= x_mean * halfNum * 2.f;

        //linear fit, (x, y) is the direction.
        float length = std::sqrt(covariance * covariance + x_variance * x_variance);
        if (length < DBL_MIN)
        {
            direction.x = 0.0f;
            direction.y = 0.0f;
        }
        else if (x_sign > FLT_EPSILON)
        {
            direction.x = x_variance / length;
            direction.y = covariance / length;
        }
        else
        {
            direction.x = -x_variance / length;
            direction.y = -covariance / length;
        }

        return true;
    }

    template <typename T>
    std::vector<size_t> sortIndex(const std::vector<T>& v)
    {
        // initialize original index locations
        size_t lengthV = v.sizez();
        std::vector<size_t>  idx(lengthV, 0);
        for (size_t i = 0; i < lengthV; ++i)
        {
            idx[i] = i;
        }

        // sort indexes based on comparing values in v
        std::sort(idx.begin(), idx.end(), [& v](size_t i1, size_t i2)
        {
            return v[i1] < v[i2];
        });
        return idx;
    }



private:
    template <class Point3_T>
    inline Point3_T add(const Point3_T& a, const Point3_T& b) const
    {
        Point3_T c;
        c.x = a.x + b.x;
        c.y = a.y + b.y;
        c.z = a.z + b.z;
        return c;
    }


    template <class Point3_T>
    inline Point3_T subtract(const Point3_T& a, const Point3_T& b) const
    {
        Point3_T c;
        c.x = a.x - b.x;
        c.y = a.y - b.y;
        c.z = a.z - b.z;
        return c;
    }

    template <class Point3_T>
    inline float scalarMultiply(const Point3_T& a, const Point3_T& b) const
    {
        float multiple(a.x * b.x);
        multiple += a.y * b.y;
        multiple += a.z * b.z;
        return multiple;
    }

    template <class Point3_T>
    inline float scalarMultiplyInXOZ(const Point3_T& a, const Point3_T& b) const
    {
        float multiple(a.x * b.x);
        multiple += a.z * b.z;
        return multiple;
    }

    template <class Point3_T>
    inline Point3_T numericalMultiply(const float& a, const Point3_T& point) const
    {
        Point3_T c;
        c.x = point.x * a;
        c.y = point.y * a;
        c.z = point.z * a;
        return c;
    }

    template <class Point3_T>
    inline float norm(const Point3_T& a) const
    {
        float tmp(a.x * a.x);
        tmp += a.y * a.y;
        tmp += a.z * a.z;
        return sqrt(tmp);
    }

    template <class Point3_T>
    inline float normXZ(const Point3_T& a) const
    {
        return std::sqrt(a.x * a.x + a.z * a.z);
    }

    template <class Point3_T>

    inline void reset(Point3_T& a) const
    {
        a.x = 0.f;
        a.y = 0.f;
        a.z = 0.f;
    }

    template <typename T1, typename T2>
    bool isElement(const T1& element, const std::vector<T2>& container) const
    {
        T2 elem = static_cast<T2>(element);
        for (auto& i : container)
        {
            if (elem == i)
            {
                return true;
            }
        }
        return false;
    }
};

}

#endif /* NURBS_H_ */
