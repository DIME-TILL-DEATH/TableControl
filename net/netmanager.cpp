#include <QFile>
#include <QFileInfo>

#include "intvaluemessage.h"

#include "netmanager.h"
#include "devicecontentmodel.h"

#include "filemanager.h"

NetManager::NetManager(FileManager *fileManager, QObject *parent)
    : QObject{parent}
{
    netClient = new NetClient(this);
    timer = new QTimer(this);

    timer->setInterval(2000);
    connect(timer, &QTimer::timeout, this, &NetManager::timerEvent);
    timer->start();

    connect(netClient, &NetClient::sgDataRecieved, this, &NetManager::processRecievedData);

    connect(netClient, &NetClient::sgConnected, this, &NetManager::sgDeviceConnected);
    connect(netClient, &NetClient::sgDisconnected, this, &NetManager::sgDeviceDisconnected);
    lastRecvFrameHeader.frameSize = 0;
}

void NetManager::sendNetRequest(FrameType frameType, uint8_t request,
                                                     uint32_t data0,
                                                     uint32_t data1,
                                                     uint32_t parameters)
{
    if(netClient->isSocketReady())
    {
        FrameHeader frameHeader;
        frameHeader.frameType = frameType;
        frameHeader.action = request;
        frameHeader.frameSize = sizeof(FrameHeader);

        frameHeader.data0 = data0;
        frameHeader.data1 = data1;
        frameHeader.frameParameters = parameters;

        FrameHeader_uni unionData;
        unionData.structData = frameHeader;

        QByteArray dataToSend;
        dataToSend.append(unionData.rawData, sizeof(FrameHeader));
        netClient->sendData(dataToSend);
    }
}

void NetManager::slSendMessage(std::shared_ptr<AbstractMessage> msg_ptr)
{
    if(netClient->isSocketReady())
    {
        netClient->sendData(msg_ptr->rawData());
    }
    else
    {
        qDebug() << "Trying to send frame, but socket not ready";
    }
}

void NetManager::slUpdateData(FrameType frameType, uint8_t dataType, QVariantList data)
{
    FrameHeader_uni frameHeader;
    memset(frameHeader.rawData, 0, sizeof(FrameHeader));
    frameHeader.structData.frameType = frameType;
    frameHeader.structData.action = dataType;

    switch(frameType)
    {
    case FrameType::HARDWARE_ACTIONS:
    {
        sendHardwareData(data, frameHeader);
        break;
    }
    case FrameType::PLAYLIST_ACTIONS:
    {
        sendPlaylistData(data, frameHeader);
        break;
    }
    case FrameType::FILE_ACTIONS:
    {
        sendFileData(data, frameHeader);
        break;
    }
    case FrameType::FIRMWARE_ACTIONS:
    {
        sendFirmwareData(data, frameHeader);
        break;
    }
    default: break;
    }
}

void NetManager::sendHardwareData(const QVariantList &data, FrameHeader_uni frameHeader)
{
    frameHeader.structData.frameSize = sizeof(FrameHeader);
    netClient->sendData(QByteArray(frameHeader.rawData, sizeof(FrameHeader)));
}

void NetManager::sendPlaylistData(const QVariantList &data, FrameHeader_uni frameHeader)
{
    QByteArray dataToSend;
    switch((Requests::Playlist)frameHeader.structData.action)
    {
    case Requests::Playlist::CHANGE_PLAYLIST:
    {
        foreach (QVariant item, data)
        {
            QString curName = item.toString();
            dataToSend.append(curName.toLocal8Bit());
            dataToSend.append("|");
        }
        break;
    }
    default: {}
    }

    frameHeader.structData.frameSize = sizeof(FrameHeader) + dataToSend.size();
    dataToSend.prepend(frameHeader.rawData, sizeof(FrameHeader));
    netClient->sendData(dataToSend);
}

void NetManager::sendFileData(const QVariantList& data, FrameHeader_uni frameHeader)
{
    QByteArray dataToSend;

    QString dstPath = data.at(0).toString();
    dataToSend.append(dstPath.toLocal8Bit());

    frameHeader.structData.frameParameters = dstPath.size();

    switch((Requests::File)frameHeader.structData.action)
    {
    case Requests::File::GET_FILE:
    {
        qDebug() << "Requesting file: " << dstPath;
        frameHeader.structData.frameSize = sizeof(FrameHeader) + dataToSend.size();
        dataToSend.prepend(frameHeader.rawData, sizeof(FrameHeader));
        netClient->sendData(dataToSend);
        break;
    }
    case Requests::File::GET_FOLDER_CONTENT:
    {
        qDebug() << "Requesting folder: " << dstPath;
        frameHeader.structData.frameSize = sizeof(FrameHeader) + dataToSend.size();
        dataToSend.prepend(frameHeader.rawData, sizeof(FrameHeader));
        netClient->sendData(dataToSend);
        break;
    }
    case Requests::File::FILE_CREATE:
    {
        QString srcPath = data.at(1).toString();
        qDebug() << "Upload file " << srcPath  << " to" << dstPath;

        QFile file(srcPath);
        if(!file.open(QIODevice::ReadOnly))
        {
            qDebug() << __FUNCTION__ << "Can't open file " << srcPath;
            return;
        }
        frameHeader.structData.frameSize = sizeof(frameHeader) + dstPath.size();
        frameHeader.structData.data0 = 0;
        frameHeader.structData.data1 = file.size();
        dataToSend.prepend(frameHeader.rawData, sizeof(FrameHeader));
        netClient->sendData(dataToSend);

        updateFileUploadProgress(NetEvents::UploadData, dstPath, 0, file.size());

        while(!file.atEnd())
        {
            int partSize = 8192;
            QByteArray partData = file.read(partSize);

            frameHeader.structData.action = (uint8_t)Requests::File::FILE_APPEND_DATA;
            frameHeader.structData.frameSize = sizeof(frameHeader) + dstPath.size() + partData.size();
            frameHeader.structData.data0 = partData.size();

            dataToSend.clear();
            dataToSend.append(frameHeader.rawData, sizeof(FrameHeader));
            dataToSend.append(dstPath.toLocal8Bit());
            dataToSend.append(partData);

            netClient->sendData(dataToSend);
            updateFileUploadProgress(NetEvents::UploadData, dstPath, 0, file.size());
        }
        file.close();

        QStringList pathElements = dstPath.split("/", Qt::SkipEmptyParts);
        pathElements.pop_back();
        QString path = pathElements.join("/");

        QVariantList request;
        request.append("/" + path + "/");
        slUpdateData(FrameType::FILE_ACTIONS, (uint8_t)(Requests::File::GET_FOLDER_CONTENT), request);
        break;
    }
    default: {}
    }
}

void NetManager::sendFirmwareData(const QVariantList &data, FrameHeader_uni frameHeader)
{
    QByteArray dataToSend;

    switch((Requests::Firmware)frameHeader.structData.action)
    {

    case Requests::Firmware::FIRMWARE_UPLOAD_START:
    {
        QString srcPath = data.at(0).toString();
        qDebug() << "Upload firmware from " << srcPath;

        QFile file(srcPath);
        if(!file.open(QIODevice::ReadOnly))
        {
            qDebug() << __FUNCTION__ << "Can't open firmware file " << srcPath;
            return;
        }
        frameHeader.structData.frameSize = sizeof(frameHeader);
        frameHeader.structData.data0 = 0;
        frameHeader.structData.data1 = file.size();
        dataToSend.prepend(frameHeader.rawData, sizeof(FrameHeader));
        netClient->sendData(dataToSend);

        updateFileUploadProgress(NetEvents::UploadFirmware, "fimware.bin", 0, file.size());

        while(!file.atEnd())
        {
            int partSize = 8192;
            QByteArray fileData = file.read(partSize);

            frameHeader.structData.action = (uint8_t)Requests::Firmware::FIRMWARE_UPLOAD_PROCEED;
            frameHeader.structData.frameSize = sizeof(frameHeader) + fileData.size();
            frameHeader.structData.data0 = fileData.size();

            dataToSend.clear();
            dataToSend.append(frameHeader.rawData, sizeof(FrameHeader));
            dataToSend.append(fileData);

            netClient->sendData(dataToSend);

            updateFileUploadProgress(NetEvents::UploadFirmware, "firmware.bin", 0, file.size());
        }
        file.close();
        break;
    }
    default: {}
    }
}

void NetManager::processRecievedData(QByteArray data)
{
    txBuffer.append(data);
    while(txBuffer.size()>=sizeof(FrameHeader))
    {
        FrameHeader_uni header;
        memcpy(header.rawData, txBuffer.data(), sizeof(FrameHeader));
        lastRecvFrameHeader = header.structData;


        if(txBuffer.size() >= lastRecvFrameHeader.frameSize)
        {
            lastRecvFrame = txBuffer.left(lastRecvFrameHeader.frameSize);

            AbstractMessage* message = nullptr;
            std::shared_ptr<AbstractMessage> msg_ptr;

            switch(lastRecvFrameHeader.frameType)
            {
            case FrameType::UNDEFINED: break;
                
            case FrameType::HARDWARE_ACTIONS:
            {
                message = processHardwareAnswer();
                break;
            }
            case FrameType::PLAYLIST_ACTIONS:
            {
                message = processPlaylistAnswer();
                break;
            }
            case FrameType::FILE_ACTIONS:
            {
                message = processFileAnswer();
                break;
            }
            case FrameType::FIRMWARE_ACTIONS:
            {
                message = processFirmwareAnswer();
                break;
            }
            }

            if(message != nullptr)
            {
                std::shared_ptr<AbstractMessage> msg_ptr(message);
                emit sgRecievingMessage(msg_ptr);
            }

            txBuffer.remove(0, lastRecvFrameHeader.frameSize);
            lastRecvFrameHeader.frameSize = 0;
        }
        else
        {
            break;
        }
    }
}

AbstractMessage* NetManager::processHardwareAnswer()
{
    QVariantList dataList;
    union { float f; uint32_t i; } u;

    switch((Requests::Hardware)lastRecvFrameHeader.action)
    {
    case Requests::Hardware::GET_SERIAL_ID:
    {
        lastRecvFrame.remove(0, sizeof(FrameHeader));
        QString result(lastRecvFrame);
        dataList.append(result);
        qDebug() << "Serial ID:" << result;
        emit sgDataUpdated(FrameType::HARDWARE_ACTIONS, (uint8_t)Data::Hardware::SERIAL_ID, dataList);
        break;
    }
    case Requests::Hardware::REQUEST_PROGRESS: return new IntValueMessage(lastRecvFrame);
    // {
    //     dataList.append(lastRecvFrameHeader.data0);
    //     dataList.append(lastRecvFrameHeader.data1);
    //     //qDebug() << "Progress" << lastRecvFrameHeader.data0 << lastRecvFrameHeader.data1;
    //     emit sgDataUpdated(FrameType::HARDWARE_ACTIONS, (uint8_t)Data::Hardware::PROGRESS, dataList);
    //     break;
    // }

    case Requests::Hardware::GET_PRINT_SPEED: return new FloatValueMessage(lastRecvFrame);
    case Requests::Hardware::GET_LED_BRIGHTNESS: return new FloatValueMessage(lastRecvFrame);
    case Requests::Hardware::GET_SCALE_COEFFICIENT: return new FloatValueMessage(lastRecvFrame);
    case Requests::Hardware::GET_ROTATION: return new FloatValueMessage(lastRecvFrame);
    case Requests::Hardware::GET_CORRECTION: return new FloatValueMessage(lastRecvFrame);
    case Requests::Hardware::GET_PAUSE_INTERVAL: return new IntValueMessage(lastRecvFrame);
    case Requests::Hardware::GET_FI_GEAR2_TEETH_COUNT: return new IntValueMessage(lastRecvFrame);
    case Requests::Hardware::GET_MACHINE_MINUTES: return new IntValueMessage(lastRecvFrame);
    default: break;
    }
    return nullptr;
}

AbstractMessage* NetManager::processPlaylistAnswer()
{
    switch((Requests::Playlist)lastRecvFrameHeader.action)
    {
    case Requests::Playlist::REQUEST_PLAYLIST:
    {
        lastRecvFrame.remove(0, sizeof(FrameHeader));
        QString result(lastRecvFrame);
        QStringList playlist = result.split("\r\n", Qt::SkipEmptyParts);
        QVariantList resultData;
        foreach (QString name, playlist)
        {
            resultData.append(QVariant(name));
        }
        emit sgDataUpdated(FrameType::PLAYLIST_ACTIONS, (uint8_t)Data::Playlist::PLAYLIST, resultData);
        break;
    }

    case Requests::Playlist::REQUEST_PLAYLIST_POSITION:
    {
        QVariantList resultData;
        resultData.append(lastRecvFrameHeader.data0);
        resultData.append(lastRecvFrameHeader.data1);
        emit sgDataUpdated(FrameType::PLAYLIST_ACTIONS, (uint8_t)Data::Playlist::PLAYLIST_POSITION, resultData);
        break;
    }
        default: break;
    }
    return nullptr;
}

AbstractMessage* NetManager::processFileAnswer()
{
    lastRecvFrame.remove(0, sizeof(FrameHeader));

    switch((Requests::File)lastRecvFrameHeader.action)
    {
    case Requests::File::GET_FILE:
    { 
        QString fileName = lastRecvFrame.left(lastRecvFrameHeader.frameParameters);
        qint32 filesize = static_cast<qint32>(lastRecvFrameHeader.data0);

        fileName.remove(DeviceContentModel::librarySdcardPath);

        QVariantList dataList;
        dataList.append(fileName);

        if(filesize == -1)
        {
            qDebug() << "Requested file not found" << fileName;
            emit sgDataUpdated(FrameType::FILE_ACTIONS, (uint8_t)Data::File::REQUESTED_FILE_NOT_FOUND, dataList);
        }
        else
        {
            lastRecvFrame.remove(0, fileName.size());

            fileManager->savePreviewFile(fileName, lastRecvFrame);

            emit sgDataUpdated(FrameType::FILE_ACTIONS, (uint8_t)Data::File::REQUESTED_FILE, dataList);
            qDebug() << "File downloaded" << fileName;
        }
        break;
    }
    case Requests::File::GET_FOLDER_CONTENT:
    {
        QString folderName = lastRecvFrame.left(lastRecvFrameHeader.frameParameters);
        lastRecvFrame.remove(0, lastRecvFrameHeader.frameParameters);
        QString result(lastRecvFrame);

        QVariantList resultData;
        resultData.append(folderName);

        QStringList resultList = result.split("\r", Qt::SkipEmptyParts);

        foreach (QString name, resultList)
        {
            resultData.append(QVariant(name));
        }

        emit sgDataUpdated(FrameType::FILE_ACTIONS, (uint8_t)Data::File::REQUESTED_FOLDER, resultData);
        break;
    }
    case Requests::File::FILE_CREATE:
    {
        // Same as FILE_APPEND_DATA
    }
    case Requests::File::FILE_APPEND_DATA:
    {
        QString filePath = lastRecvFrame.left(lastRecvFrameHeader.frameParameters);
        qint32 partSize = static_cast<qint32>(lastRecvFrameHeader.data0);
        qint32 fileSize = static_cast<qint32>(lastRecvFrameHeader.data1);

        if(partSize != -1)
        {
            updateFileUploadProgress(NetEvents::UploadDataCompleted, filePath, partSize, fileSize);
        }
        else
        {
            emit sgNetEvent(NetEvents::UploadDataError, filePath);
        }
        break;
    }
    }
    return nullptr;
}

AbstractMessage* NetManager::processFirmwareAnswer()
{
    lastRecvFrame.remove(0, sizeof(FrameHeader));

    switch((Requests::Firmware)lastRecvFrameHeader.action)
    {
    case Requests::Firmware::FIRMWARE_UPLOAD_START:
    {
        // Same as FIRMWARE_UPLOAD_PROCEED
    }
    case Requests::Firmware::FIRMWARE_UPLOAD_PROCEED:
    {
        qint32 dataProcessed = static_cast<qint32>(lastRecvFrameHeader.data0);
        qint32 fileSize = static_cast<qint32>(lastRecvFrameHeader.data1);

        if(fileSize != -1)
        {
            updateFileUploadProgress(NetEvents::UploadFirmware, "firmware.bin", dataProcessed, fileSize);

            if(dataProcessed == fileSize)
            {
                qDebug() << "Firmware uploaded, update";
                updateFileUploadProgress(NetEvents::UpdatingFirmware, "firmware.bin", dataProcessed, fileSize);
                sendNetRequest(FrameType::FIRMWARE_ACTIONS, (uint8_t)Requests::Firmware::FIRMWARE_UPDATE);
            }
        }
        else
        {
            emit sgNetEvent(NetEvents::UploadFirmwareError, "firmware.bin");
        }

        break;
    }
    case Requests::Firmware::FIRMWARE_UPDATE:
    {
        if(lastRecvFrameHeader.data0 == 1)
        {
            emit sgNetEvent(NetEvents::UpdatingFirmwareFinished, "firmware.bin");
            sendNetRequest(FrameType::FIRMWARE_ACTIONS, (uint8_t)Requests::Firmware::ESP_RESTART);
        }
        else if((qint32)lastRecvFrameHeader.data0 == -1)
        {
            emit sgNetEvent(NetEvents::UpdatingFirmwareError, "firmware.bin");
        }
        break;
    }
    case Requests::Firmware::FIRMWARE_VERSION:
    {
        QVariantList data;
        data.append(lastRecvFrame.left(lastRecvFrameHeader.frameParameters));
        emit sgDataUpdated(FrameType::FIRMWARE_ACTIONS, (uint8_t)Data::Firmware::FIRMWARE_VERSION, data);
        break;
    }
    default: {}
    }
    return nullptr;
}

void NetManager::updateFileUploadProgress(NetEvents type, QString filePath, quint64 currentPartSize, quint64 totalSize)
{
    QVariantList netEventData;
    netEventData.append(currentPartSize);
    netEventData.append(totalSize);
    emit sgNetEvent(type, filePath, netEventData);
}

void NetManager::timerEvent()
{
    if(!netClient->isSocketReady())
    {
        netClient->doConnect();
    }
}
