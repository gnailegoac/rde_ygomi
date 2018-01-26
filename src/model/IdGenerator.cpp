/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2017
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    IdGenerator.cpp
 * @brief
 *******************************************************************************
 */

#include "IdGenerator.h"

#include "Road.h"
#include "Line.h"
#include "Lane.h"
#include "Curve.h"
#include "TrafficSign.h"

Model::IdGenerator::IdGenerator():
    mId(1)
{

}

Model::IdGenerator::~IdGenerator()
{

}

std::uint64_t Model::IdGenerator::GetId() const
{
    return mId;
}

std::uint64_t Model::IdGenerator::GetNewId()
{
    return ++mId;
}

template <class T>
Model::IdGeneratorMap<T>::IdGeneratorMap():
    mIdGenerator(std::make_shared<IdGenerator>())
{
}

template <class T>
Model::IdGeneratorMap<T>::~IdGeneratorMap()
{
}

template <class T>
void Model::IdGeneratorMap<T>::SetIdAndText(const std::uint64_t& aId, const std::string& aText)
{
    std::lock_guard<std::mutex> mutexGuard(mMutex);
    mIdMap[aText] = aId;
    mTextMap[aId] = aText;
}

template <class T>
const std::uint64_t& Model::IdGeneratorMap<T>::GenerateNewId(const std::string& aText)
{
    std::lock_guard<std::mutex> mutexGuard(mMutex);
    std::uint32_t index = 0;
    std::string text = "";

    do
    {
        text = aText + "_" + std::to_string(index++);
    }while (0 != mIdMap.count(text));

    mIdMap[text] = mIdGenerator->GetNewId();
    mTextMap[mIdMap[text]] = text;

    return mIdMap[text];
}

template <class T>
const std::uint64_t& Model::IdGeneratorMap<T>::GetRandomId(std::int32_t aHigh)
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

    return mIdMap[text];
}

template <class T>
const std::uint64_t& Model::IdGeneratorMap<T>::GetId(const std::string& aId)
{
    std::lock_guard<std::mutex> mutexGuard(mMutex);

    if (0 == mIdMap.count(aId))
    {
        mIdMap[aId] = mIdGenerator->GetNewId();
        mTextMap[mIdMap[aId]] = aId;
    }

    return mIdMap[aId];
}

template <class T>
const std::string& Model::IdGeneratorMap<T>::GetTextId(const std::uint64_t& aId)
{
    const static std::string INVALID = TEXT_NAN;
    std::lock_guard<std::mutex> mutexGuard(mMutex);

    if (0 != mTextMap.count(aId))
    {
       return mTextMap[aId];
    }

    return INVALID;
}

template <class T>
void Model::IdGeneratorMap<T>::Reset()
{
    std::lock_guard<std::mutex> mutexGuard(mMutex);

    mIdGenerator.reset();
    mIdGenerator = std::make_shared<IdGenerator>();
    mIdMap.clear();
    mTextMap.clear();
}

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template
class Model::IdGeneratorMap<Model::Curve>;

template
class Model::IdGeneratorMap<Model::Line>;

template
class Model::IdGeneratorMap<Model::Lane>;

template
class Model::IdGeneratorMap<Model::Road>;

template
class Model::IdGeneratorMap<Model::TrafficSign>;

template
class Model::IdGeneratorMap<void*>;

//----------------------------------------------------------------------------
