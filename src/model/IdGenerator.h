/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    IdGenerator.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <string>
#include <mutex>

namespace Model
{

class IdGenerator
{
public:
    IdGenerator();
    ~IdGenerator();

    std::uint64_t GetId() const;
    std::uint64_t GetNewId();

private:
    std::atomic<std::uint64_t> mId;
};

template <class T>
class IdGeneratorMap
{
public:
    IdGeneratorMap():mIdGenerator(std::make_shared<IdGenerator>()){}
    ~IdGeneratorMap(){}

    std::uint64_t GetId(std::string aId)
    {
        std::lock_guard<std::mutex> mutexGuard(mMutex);

        if (0 == mIdMap.count(aId))
        {
            mIdMap[aId] = mIdGenerator->GetNewId();
        }

        return mIdMap[aId];
    }
    void Reset()
    {
        std::lock_guard<std::mutex> mutexGuard(mMutex);

        mIdGenerator.reset();
        mIdGenerator = std::make_shared<IdGenerator>();
        mIdMap.clear();
    }

private:
    T mType;
    std::mutex mMutex;
    std::shared_ptr<IdGenerator> mIdGenerator;
    std::unordered_map<std::string, uint64_t> mIdMap;
};

}
