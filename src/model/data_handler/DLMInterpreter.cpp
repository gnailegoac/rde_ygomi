/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file    DLMInterpreter.cpp
 * @brief
 *******************************************************************************
 */

#include "DLMInterpreter.h"

#include <QDomProcessingInstruction>
#include <QFile>
#include <QTextStream>

namespace
{
static const QString scSourceVersion = "1";
static const QString scTargetVersion = "1";
static const QString scBaselineMapId = "1";
static const QString scJournalVersion = "2.0.1";
}

Model::DLMInterpreter::DLMInterpreter(const std::string& aFileName, const double& aInterval) :
    mFileName(aFileName),
    mInterval(aInterval)
{

}

Model::DLMInterpreter::~DLMInterpreter()
{

}

bool Model::DLMInterpreter::SaveLaneGroups(const Model::MemoryModelPtr& aMemoryModel)
{
    QDomDocument domDocument;
    writeInstruction(domDocument);
    QDomElement journal = domDocument.createElement("journal");
    writeJournalAttributes(journal);
    writeMetaInfo(domDocument, journal);
    domDocument.appendChild(journal);


    return saveDLMFile(domDocument);
}

bool Model::DLMInterpreter::saveDLMFile(QDomDocument& aDocument) const
{
    QFile file(QString::fromStdString(mFileName));

    try
    {
        if(!file.open(QIODevice::WriteOnly))
        {
            return false;
        }

        QTextStream out(&file);
        aDocument.save(out, 2);
        file.close();
    }
    catch(...)
    {
        file.close();
        return false;
    }

    return true;
}

void Model::DLMInterpreter::writeInstruction(QDomDocument& aDom)
{
    QDomProcessingInstruction instruction;
    instruction = aDom.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"");
    aDom.appendChild(instruction);
}

void Model::DLMInterpreter::writeMetaInfo(QDomDocument& aDom, QDomElement& aElement)
{
    QDomElement meta = aDom.createElement("meta");
    QDomElement sourceVersion = aDom.createElement("sourceVersion");
    QDomText txSourceVersion = aDom.createTextNode(scSourceVersion);
    sourceVersion.appendChild(txSourceVersion);
    meta.appendChild(sourceVersion);
    QDomElement targetVersion = aDom.createElement("targetVersion");
    QDomText txTargetVersion = aDom.createTextNode(scTargetVersion);
    targetVersion.appendChild(txTargetVersion);
    meta.appendChild(targetVersion);
    QDomElement baselineMapId = aDom.createElement("baselineMapId");
    QDomText txBaselineMapId = aDom.createTextNode(scBaselineMapId);
    baselineMapId.appendChild(txBaselineMapId);
    meta.appendChild(baselineMapId);
    aElement.appendChild(meta);
}

void Model::DLMInterpreter::writeJournalAttributes(QDomElement& aElement)
{
    // TODO: how to get country code.
    aElement.setAttribute("isoCountryCode", "DEU");
    aElement.setAttribute("version", scJournalVersion);
    aElement.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    aElement.setAttribute("xsi:noNamespaceSchemaLocation", "had.xsd");
}
