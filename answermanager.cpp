#include "answermanager.h"

#include "netmanager.h"
#include "firmware.h"

AnswerManager::AnswerManager(QObject *parent)
    : QObject{parent}
{}

void AnswerManager::slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data)
{
    emit sgDataUpdated(frameType, dataType, data);
}

void AnswerManager::slNetEvent(NetEvents eventType, QString target, QVariantList data)
{
    emit sgNetEvent(eventType, target, data);
}

void AnswerManager::slRecieveMessage(std::shared_ptr<AbstractMessage> msg_ptr)
{
    switch(msg_ptr->messageType())
    {
        case AbstractMessage::ABSTRACT: break;
        case AbstractMessage::INT_VALUE:
        {
            IntValueMessage* int_msg = dynamic_cast<IntValueMessage*>(msg_ptr.get());
            processMessage(int_msg);
            break;
        }
        case AbstractMessage::FLOAT_VALUE:
        {
            FloatValueMessage* float_msg = dynamic_cast<FloatValueMessage*>(msg_ptr.get());
            processMessage(float_msg);
            break;
        }
        case AbstractMessage::STRING:
        {
            StringMessage* string_msg = dynamic_cast<StringMessage*>(msg_ptr.get());
            processMessage(string_msg);
            break;
        }
        case AbstractMessage::FILE_PART:
        {
            FilePartMessage* file_msg = dynamic_cast<FilePartMessage*>(msg_ptr.get());
            processMessage(file_msg);
            break;
        }
        case AbstractMessage::FOLDER_CONTENT:
        {
            FolderContentMessage* msg = dynamic_cast<FolderContentMessage*>(msg_ptr.get());
            emit sgFolderContent(msg->path(), msg->files());
            break;
        }
    }
}

void AnswerManager::processMessage(IntValueMessage *message)
{
    switch(message->frameType())
    {
    case UNDEFINED: return;
    case PLAYLIST_ACTIONS:
    {
        switch(static_cast<Requests::Playlist>(message->action()))
        {
        case Requests::Playlist::REQUEST_PLAYLIST_POSITION:
        case Requests::Playlist::CHANGE_PLAYLIST_POSITION:
        {
            emit sgPlaylistPosition(message->value());
            break;
        }
        default: break;
        }
        break;
    }
    case HARDWARE_ACTIONS:
    {
        switch(static_cast<Requests::Hardware>(message->action()))
        {
        case Requests::Hardware::PAUSE_PRINTING: break;
        case Requests::Hardware::REQUEST_PROGRESS:
        {
            emit sgProgress((float)message->firstValue()/(float)message->secondValue());
            break;
        }
        case Requests::Hardware::GET_PAUSE_INTERVAL:
        {
            emit sgPauseInterval(message->value());
            qDebug() << "Pause interval: " << message->value();
            break;
        }
        case Requests::Hardware::GET_FI_GEAR2_TEETH_COUNT:
        {
            emit sgFiGear2Teeths(message->value());
            qDebug() << "Fi gear2 teeth count: " << message->value();
            break;
        }
        case Requests::Hardware::GET_MACHINE_MINUTES:
        {
            emit sgMachineMinutes(message->value());
            break;
        }
        case Requests::Hardware::GET_FIRST_MOTOR_INVERSION:
        {
            emit sgFirstMotorInversion(message->value());
            break;
        }
        case Requests::Hardware::GET_SECOND_MOTOR_INVERSION:
        {
            emit sgSecondMotorInversion(message->value());
            break;
        }
        default: break;
        }
        break;
    }
    case FILE_ACTIONS: return;
    case FIRMWARE_ACTIONS:
    {
        switch(static_cast<Requests::Firmware>(message->action()))
        {
        case Requests::Firmware::FIRMWARE_UPDATE:
        {
            qDebug() << "FW update finished recieved, result" << message->value();
            int operationResult = static_cast<int32_t>(message->value());
            if(operationResult == 1)
            {
                emit sgNetEvent(NetEvents::UpdatingFirmwareFinished, "firmware.bin");

                IntValueMessage* msg = new IntValueMessage(FrameType::FIRMWARE_ACTIONS, (uint8_t)Requests::Firmware::ESP_RESTART);
                emit sgSendMessage(std::shared_ptr<AbstractMessage>(msg));
            }
            else if(operationResult == -1)
            {
                emit sgNetEvent(NetEvents::UpdatingFirmwareError, "firmware.bin");
            }
            break;
        }
            default: break;
        }

        break;
    }
    }
}

void AnswerManager::processMessage(FloatValueMessage *message)
{
    switch(message->frameType())
    {
    case HARDWARE_ACTIONS:
    {
        switch(static_cast<Requests::Hardware>(message->action()))
        {
        case Requests::Hardware::GET_PRINT_SPEED:
        case Requests::Hardware::SET_PRINT_SPEED:
        {
            emit sgPrintSpeed(message->value());
            qDebug() << "Print speed: " << message->value();
            break;
        }
        case Requests::Hardware::GET_LED_BRIGHTNESS:
        case Requests::Hardware::SET_LED_BRIGHTNESS:
        {
            emit sgLedBrightness(message->value());
            qDebug() << "Led brightness: " << message->value();
            break;
        }
        case Requests::Hardware::GET_SCALE_COEFFICIENT:
        case Requests::Hardware::SET_SCALE_COEFFICIENT:
        {
            emit sgScaleCoefficient(message->value());
            qDebug() << "Scale coefficient: " << message->value();
            break;
        }
        case Requests::Hardware::GET_ROTATION:
        case Requests::Hardware::SET_ROTATION:
        {
            emit sgRotation(message->value());
            qDebug() << "Rotation: " << message->value();
            break;
        }
        case Requests::Hardware::GET_CORRECTION:
        case Requests::Hardware::SET_CORRECTION:
        {
            emit sgCorrection(message->value());
            qDebug() << "Correction: " << message->value();
            break;
        }
        default: return;
        }
        break;
    }
    default: return;
    }
}

void AnswerManager::processMessage(StringMessage *message)
{
    switch(message->frameType())
    {
    case UNDEFINED: return;
    case PLAYLIST_ACTIONS:
    {
        switch(static_cast<Requests::Playlist>(message->action()))
        {
        case Requests::Playlist::REQUEST_PLAYLIST:
        case Requests::Playlist::CHANGE_PLAYLIST:
        {
            qDebug() << "Playlist recieved";
            QStringList playlist = message->string().split("\r\n", Qt::SkipEmptyParts);
            emit sgPlaylist(playlist);
            break;
        }

        case Requests::Playlist::GET_CURRENT_GALLERY:
        case Requests::Playlist::SET_CURRENT_GALLERY:
        {
            qDebug() << "Current gallery is: " << message->string();
            emit sgCurrentGalleryName(message->string());
            break;
        }
        default: break;
        }
        break;
    }
    case HARDWARE_ACTIONS:
    {
        switch(static_cast<Requests::Hardware>(message->action()))
        {
        case Requests::Hardware::GET_SERIAL_ID:
        {
            qDebug() << "Serial ID:" << message->string();
            emit sgSerialId(message->string());
        }
        default: return;
        }
        break;
    }
    case FILE_ACTIONS: return;
    case FIRMWARE_ACTIONS:
    {
        switch(static_cast<Requests::Firmware>(message->action()))
        {
        case Requests::Firmware::FIRMWARE_VERSION:
        {
            currentFw = message->string();

            qDebug() << "Firmware version:" << currentFw;
            emit sgFirmwareVersion(currentFw);
            break;
        }
        default: return;
        }
        break;
    }
    }
}

void AnswerManager::processMessage(FilePartMessage *message)
{
    switch(message->frameType())
    {
        case FILE_ACTIONS:
        {
            switch(static_cast<Requests::File>(message->action()))
            {
                case Requests::File::GET_FILE:
                {
                    if(message->fileSize() == -1)
                    {
                        emit sgFileNotFound(message->srcPath());
                    }
                    else
                    {
                        emit sgFilePartDownloaded(message->srcPath(), message->filePart(), message->partPosition(), message->fileSize());
                    }
                    break;
                }
                case Requests::File::FILE_CREATE:
                case Requests::File::FILE_APPEND_DATA:
                {
                    QString filePath = message->srcPath();
                    qint32 filePosition = message->partPosition();

                    if(filePosition != -1)
                    {
                        updateFileUploadProgress(NetEvents::UploadDataCompleted, filePath, filePosition, message->fileSize());
                    }
                    else
                    {
                        emit sgNetEvent(NetEvents::UploadDataError, filePath);
                    }
                    break;
                }
                default: break;
            }
            break;
        }
        case FIRMWARE_ACTIONS:
        {
            switch(static_cast<Requests::Firmware>(message->action()))
            {
                case Requests::Firmware::FIRMWARE_UPLOAD_START:
                case Requests::Firmware::FIRMWARE_UPLOAD_PROCEED:
                {
                    qint32 fileSize = message->fileSize();
                    qint32 filePosition = message->partPosition();

                    if(fileSize != -1)
                    {
                        updateFileUploadProgress(NetEvents::UploadFirmware, Firmware::dstPath, filePosition, message->fileSize());
                    }
                    else
                    {
                        emit sgNetEvent(NetEvents::UploadFirmwareError, "firmware.bin");
                    }
                    break;
                }
                default: break;
            }
            break;
        }
        default: break;
    }
}

// TODO: move to file manager???
void AnswerManager::updateFileUploadProgress(NetEvents type, QString filePath, quint64 currentPartSize, quint64 totalSize)
{
    QVariantList netEventData;
    netEventData.append(currentPartSize);
    netEventData.append(totalSize);
    emit sgNetEvent(type, filePath, netEventData);
}
