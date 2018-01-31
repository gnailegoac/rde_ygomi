/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DLMInterpreter.h
 * @brief
 *******************************************************************************
 */

#pragma once

#include <string>

#include <QDomDocument>
#include <QDomElement>

#include "../MemoryModel.h"

namespace Model
{
typedef std::shared_ptr<MemoryModel> MemoryModelPtr;

class DLMInterpreter
{
public:
    DLMInterpreter(const std::string& aFileName, const double& aInterval = 0.5);
    ~DLMInterpreter();

    bool SaveLaneGroups(const MemoryModelPtr& aMemoryModel);

private:
    bool saveDLMFile(QDomDocument& aDocument) const;
    void writeInstruction(QDomDocument& aDom);
    void writeMetaInfo(QDomDocument& aDom, QDomElement& aElement);
    void writeJournalAttributes(QDomElement& aElement);

private:
    std::string mFileName;
    double mInterval;
};

typedef std::shared_ptr<DLMInterpreter> DLMInterpreterPtr;

}
