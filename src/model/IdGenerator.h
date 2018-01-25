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
    IdGeneratorMap();
    ~IdGeneratorMap();

    void SetIdAndText(const std::uint64_t& aId, const std::string& aText);
    const std::uint64_t& GenerateNewId(const std::string& aText);
    const std::uint64_t& GetRandomId(std::int32_t aHigh);
    const std::uint64_t& GetId(const std::string& aId);
    const std::string& GetTextId(const std::uint64_t& aId);
    void Reset();

private:
    T mType;
    std::mutex mMutex;
    std::shared_ptr<IdGenerator> mIdGenerator;
    std::unordered_map<std::string, uint64_t> mIdMap;
    std::unordered_map<uint64_t, std::string> mTextMap;
};

}
