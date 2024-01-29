#include <QFile>
#include <QFileInfo>

#include "netmanager.h"
#include "filemanager.h"
#include "devicecontentmodel.h"

NetManager::NetManager(QObject *parent)
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

void NetManager::sendRequest(FrameType frameType, uint8_t request,
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

void NetManager::slUpdateData(FrameType frameType, uint8_t dataType, QVariantList data)
{
    FrameHeader_uni frameHeader;
    memset(frameHeader.rawData, 0, sizeof(FrameHeader));
    frameHeader.structData.frameType = frameType;
    frameHeader.structData.action = dataType;

    switch(frameType)
    {
    case FrameType::TRANSPORT_ACTIONS:
    {
        sendTransportData(data, frameHeader);
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

void NetManager::sendTransportData(const QVariantList &data, FrameHeader_uni frameHeader)
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
    case Requests::Playlist::CHANGE_PLAYLIST_POSITION:
    {
        qint16 plsPosition = data.at(0).toInt();
        dataToSend = QByteArray((const char*)&plsPosition, 2);
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

    // TODO: dstPath.size to frame parameters
    frameHeader.structData.data0 = dstPath.size();

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
        frameHeader.structData.data1 = 0;
        dataToSend.prepend(frameHeader.rawData, sizeof(FrameHeader));
        netClient->sendData(dataToSend);

        updateFileUploadProgress(NetEvents::UploadData,dstPath, 0, file.size());

        while(!file.atEnd())
        {
            int partSize = 8192;
            QByteArray fileData = file.read(partSize);

            frameHeader.structData.action = (uint8_t)Requests::File::FILE_APPEND_DATA;
            frameHeader.structData.frameSize = sizeof(frameHeader) + dstPath.size() + fileData.size();
            frameHeader.structData.data1 = fileData.size();

            dataToSend.clear();
            dataToSend.append(frameHeader.rawData, sizeof(FrameHeader));
            dataToSend.append(dstPath.toLocal8Bit());
            dataToSend.append(fileData);

            netClient->sendData(dataToSend);
            updateFileUploadProgress(NetEvents::UploadData, dstPath, fileData.size(), file.size());
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

            updateFileUploadProgress(NetEvents::UploadData, "firmware.bin", fileData.size(), file.size());
        }
        file.close();

        break;
    }
//    case Requests::Firmware::FIRMWARE_UPDATE:
//    {
//        qDebug() << "Trigger firmware update";
//        frameHeader.structData.frameSize = sizeof(FrameHeader);
//        dataToSend.prepend(frameHeader.rawData, sizeof(FrameHeader));
//        netClient->sendData(dataToSend);
//        break;
//    }
    default: {}
    }
}

void NetManager::processRecievedData(QByteArray data)
{
    txBuffer.append(data);
    //qDebug() << "Recieved data, len:" << data.size();

    while(txBuffer.size()>=sizeof(FrameHeader))
    {
        FrameHeader_uni header;
        memcpy(header.rawData, txBuffer.data(), sizeof(FrameHeader));
        lastRecvFrameHeader = header.structData;


        if(txBuffer.size() >= lastRecvFrameHeader.frameSize)
        {
            //qDebug() << "Parsing frame. Size:" << lastRecvFrameHeader.frameSize << " recieved buffer:" << txBuffer.size();
            lastRecvFrame = txBuffer.left(lastRecvFrameHeader.frameSize);
            switch(lastRecvFrameHeader.frameType)
            {

            case FrameType::TRANSPORT_ACTIONS:
            {
                processTransportAnswer();
                break;
            }
            case FrameType::PLAYLIST_ACTIONS:
            {
                processPlaylistAnswer();
                break;
            }
            case FrameType::FILE_ACTIONS:
            {
                processFileAnswer();
                break;
            }
            case FrameType::FIRMWARE_ACTIONS:
            {
                processFirmwareAnswer();
                break;
            }
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

void NetManager::processTransportAnswer()
{
    switch((Requests::Transport)lastRecvFrameHeader.action)
    {
    case Requests::Transport::REQUEST_PROGRESS:
    {
        QVariantList dataList;
        dataList.append(lastRecvFrameHeader.data0);
        dataList.append(lastRecvFrameHeader.data1);
        emit sgDataUpdated(FrameType::TRANSPORT_ACTIONS, (uint8_t)Data::Transport::PROGRESS, dataList);
        break;
    }
    case Requests::Transport::PAUSE_PRINTING:
    {
        break;
    }
    default: break;
    }
}

void NetManager::processPlaylistAnswer()
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

    case Requests::Playlist::CHANGE_PLAYLIST:
    {
        break;
    }

    case Requests::Playlist::CHANGE_PLAYLIST_POSITION:
    {
        break;
    }
    }
}

void NetManager::processFileAnswer()
{
    lastRecvFrame.remove(0, sizeof(FrameHeader));

    switch((Requests::File)lastRecvFrameHeader.action)
    {
    case Requests::File::GET_FILE:
    { 
        QString fileName = lastRecvFrame.left(lastRecvFrameHeader.data0);
        qint32 filesize = static_cast<qint32>(lastRecvFrameHeader.data1);

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

            FileManager::savePreviewFile(fileName, lastRecvFrame);

            emit sgDataUpdated(FrameType::FILE_ACTIONS, (uint8_t)Data::File::REQUESTED_FILE, dataList);
            qDebug() << "File downloaded" << fileName;
        }
        break;
    }
    case Requests::File::GET_FOLDER_CONTENT:
    {
        QString result(lastRecvFrame);
        QStringList resultList = result.split("*", Qt::SkipEmptyParts);

        // TODO путь к папке не сепаратором, а просто длинной в frameParameters, как для имени файла в остальных местах
        QString folderName = resultList.at(0);

        QVariantList resultData;
        resultData.append(folderName);

        if(resultList.size() > 1)
        {
            result = resultList.at(1);

            resultList = result.split("\r", Qt::SkipEmptyParts);


            foreach (QString name, resultList)
            {
                resultData.append(QVariant(name));
            }
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
        QString filePath = lastRecvFrame.left(lastRecvFrameHeader.data0);
        qint32 fileSize = static_cast<qint32>(lastRecvFrameHeader.data1);

        if(fileSize != -1)
        {
            updateFileUploadProgress(NetEvents::UploadDataCompleted, filePath, fileSize, 0);
        }
        else
        {
            emit sgNetEvent(NetEvents::UploadDataError, filePath);
        }
        break;
    }
    }
}

void NetManager::processFirmwareAnswer()
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
            updateFileUploadProgress(NetEvents::UploadFirmwareCompleted, "firmware.bin", dataProcessed ,fileSize);
        }
        else
        {
            emit sgNetEvent(NetEvents::UploadDataError, "firmware.bin");
        }
        break;
    }
    case Requests::Firmware::FIRMWARE_UPLOAD_END:
    {
        sendRequest(FrameType::FIRMWARE_ACTIONS, (uint8_t)Requests::Firmware::FIRMWARE_UPDATE);
        break;
    }
    case Requests::Firmware::FIRMWARE_VERSION:
    {
        QVariantList data;
        data.append(lastRecvFrame.left(lastRecvFrameHeader.data0));
        emit sgDataUpdated(FrameType::FIRMWARE_ACTIONS, (uint8_t)Data::Firmware::FIRMWARE_VERSION, data);
        break;
    }
    default: {}
    }
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
