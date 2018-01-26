/**
 *******************************************************************************
 *                       Continental Confidential
 *                  Copyright (c) Continental AG. 2018
 *
 *      This software is furnished under license and may be used or
 *      copied only in accordance with the terms of such license.
 *******************************************************************************
 * @file   FileSerializeCommand.cpp
 * @brief  Definition of FileSerializeCommand.cpp
 *******************************************************************************
 */
#include "FileSerializeCommand.h"

#include <QString>
#include <QDebug>

#include "facade/ApplicationFacade.h"
#include "CommonFunction.h"
#include "model/data_handler/Factory.h"
#include "model/data_handler/ISerializer.h"
#include "model/MemoryModel.h"
#include "proxy/MainProxy.h"

std::string Controller::FileSerializeCommand::GetCommandName()
{
    return "FileSerializeCommand";
}

void Controller::FileSerializeCommand::execute(const PureMVC::Interfaces::INotification& aNotification)
{
    QString saveResult;
    MainProxy& mainProxy = dynamic_cast<MainProxy&>(ApplicationFacade::RetriveProxy(MainProxy::NAME));
    const std::shared_ptr<Model::MemoryModel>& memoryModel = mainProxy.GetMemoryModel();
    if (memoryModel == nullptr)
    {
        saveResult = "Please open logicDB first.";
    }
    else
    {
        QString folderPath = *CommonFunction::ConvertToNonConstType<QString>(aNotification.getBody());

        if (aNotification.getName() == ApplicationFacade::SAVE_LOGICDB)
        {
            Model::IFactoryPtr factory = Model::Factory::CreateLogicDbFactory(folderPath.toStdString());
            Model::ISerializerPtr serializer = factory->CreateSerializer();
            if (serializer->Serialize(memoryModel))
            {
                saveResult = "LogicDB saved successfully.";
            }
            else
            {
                saveResult = "Failed to save logicDB.";
            }
        }
        else if (aNotification.getName() == ApplicationFacade::EXPORT_TO_KML)
        {
            Model::IFactoryPtr factory = Model::Factory::CreateKMLFactory(folderPath.toStdString());
            Model::ISerializerPtr serializer = factory->CreateSerializer();
            if (serializer->Serialize(memoryModel))
            {
                saveResult = "Export to KML successfully.";
            }
            else
            {
                saveResult = "Failed to export to KML.";
            }
        }
        else if (aNotification.getName() == ApplicationFacade::EXPORT_TO_PROTOBUF)
        {
            Model::IFactoryPtr factory = Model::Factory::CreateProtoBufferFactory(folderPath.toStdString());
            Model::ISerializerPtr serializer = factory->CreateSerializer();
            if (serializer->Serialize(memoryModel))
            {
                saveResult = "Export to ProtoBuffer successfully.";
            }
            else
            {
                saveResult = "Failed to export to ProtoBuffer.";
            }
        }
    }

    ApplicationFacade::SendNotification(ApplicationFacade::NOTIFY_RESULT, &saveResult);
}
