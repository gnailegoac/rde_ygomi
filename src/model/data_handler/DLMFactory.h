/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DLMFactory.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include "IFactory.h"

#include <string>

namespace Model
{

class DLMFactory : public IFactory
{
public:
    DLMFactory();
    ~DLMFactory();

    void SetOutputFileName(const std::string& aFileName);
    const std::string& GetOutputFileName() const;
    void SetOutputInterval(const double& aOutputInterval);
    const double& GetOutputInterval() const;

    IParserPtr CreateParser() override;
    ISerializerPtr CreateSerializer() override;

private:
    std::string mFileName;
    double mInterval;
};

}
