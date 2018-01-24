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
#include <time.h>

#include "Common.h"

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

    void SetIdAndText(const std::uint64_t& aId, const std::string& aText)
    {
        std::lock_guard<std::mutex> mutexGuard(mMutex);
        mIdMap[aText] = aId;
        mTextMap[aId] = aText;
    }
    const std::uint64_t& CreateNewIdFromText(const std::string& aId)
    {
        std::lock_guard<std::mutex> mutexGuard(mMutex);
        std::uint32_t index = 0;
        std::string text = "";

        do
        {
            text = aId + "_" + std::to_string(index++);
        }while (0 != mIdMap.count(text));

        mIdMap[text] = mIdGenerator->GetNewId();
        mTextMap[mIdMap[text]] = text;

        return mIdMap[text];
    }
    const std::uint64_t& GetRandomId(std::int32_t aHigh)
    {
        std::lock_guard<std::mutex> mutexGuard(mMutex);
        std::string text = "";
        std::uint64_t id = 0;
        srand((unsigned)time(NULL));

        do
        {
            std::uint32_t newId = static_cast<std::uint32_t>(rand());
            id = static_cast<std::uint64_t>(aHigh);
            id <<= 32;
            id |= newId;
            text = std::to_string(id);
        }while (0 != mTextMap.count(id) || 0 != mIdMap.count(text));

        mTextMap[id] = text;
        mIdMap[text] = id;

        return id;
    }
    const std::uint64_t& GetId(const std::string& aId)
    {
        std::lock_guard<std::mutex> mutexGuard(mMutex);

        if (0 == mIdMap.count(aId))
        {
            mIdMap[aId] = mIdGenerator->GetNewId();
            mTextMap[mIdMap[aId]] = aId;
        }

        return mIdMap[aId];
    }
    const std::string& GetTextId(const std::uint64_t& aId)
    {
        const static std::string INVALID = TEXT_NAN;
        std::lock_guard<std::mutex> mutexGuard(mMutex);

        if (0 != mTextMap.count(aId))
        {
           return mTextMap[aId];
        }

        return INVALID;
    }
    void Reset()
    {
        std::lock_guard<std::mutex> mutexGuard(mMutex);

        mIdGenerator.reset();
        mIdGenerator = std::make_shared<IdGenerator>();
        mIdMap.clear();
        mTextMap.clear();
    }

private:
    T mType;
    std::mutex mMutex;
    std::shared_ptr<IdGenerator> mIdGenerator;
    std::unordered_map<std::string, uint64_t> mIdMap;
    std::unordered_map<uint64_t, std::string> mTextMap;
};

}
