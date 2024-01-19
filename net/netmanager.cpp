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

void NetManager::sendRequest(FrameType frameType, uint8_t requestType,
                                                     uint32_t data0,
                                                     uint32_t data1,
                                                     uint32_t parameters)
{
    if(netClient->isSocketReady())
    {
        FrameHeader frameHeader;
        frameHeader.frameType = frameType;
        frameHeader.actionType = requestType;
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

void NetManager::updateData(FrameType frameType, uint8_t dataType, QVariantList data)
{
    FrameHeader_uni frameHeader;
    memset(frameHeader.rawData, 0, sizeof(FrameHeader));
    frameHeader.structData.frameType = frameType;
    frameHeader.structData.actionType = dataType;
    QByteArray dataToSend;

    switch(frameType)
    {
    case FrameType::TRANSPORT_ACTIONS:
    {
        dataToSend = formUpdatedTransportData(dataType, data);
        break;
    }
    case FrameType::PLAYLIST_ACTIONS:
    {
        dataToSend = formUpdatedPlaylistData(dataType, data);
        break;
    }
    case FrameType::FILE_ACTIONS:
    {
        dataToSend = formUpdatedFileData(dataType, data);
        break;
    }
    default: break;
    }

    frameHeader.structData.frameSize = sizeof(FrameHeader) + dataToSend.size();
    dataToSend.prepend(frameHeader.rawData, sizeof(FrameHeader));
    netClient->sendData(dataToSend);
}


QByteArray NetManager::formUpdatedTransportData(uint8_t dataType, QVariantList data)
{
    QByteArray result;

    return result;
}

QByteArray NetManager::formUpdatedPlaylistData(uint8_t dataType, QVariantList data)
{
    QByteArray result;
    switch((Requests::Playlist)dataType)
    {
    case Requests::Playlist::CHANGE_PLAYLIST:
    {
        foreach (QVariant item, data)
        {
            QString curName = item.toString();
            result.append(curName.toLocal8Bit());
            result.append("|");
        }
        break;
    }
    case Requests::Playlist::CHANGE_PLAYLIST_POSITION:
    {
        qint16 plsPosition = data.at(0).toInt();
        result = QByteArray((const char*)&plsPosition, 2);
        break;
    }
    default: {}
    }
    return result;
}

QByteArray NetManager::formUpdatedFileData(uint8_t dataType, QVariantList data)
{
    QByteArray result;
    switch((Requests::File)dataType)
    {
    case Requests::File::GET_FILE:
    {
        QString name = data.at(0).toString();
        result.append(name.toLocal8Bit());
        qDebug() << "Requesting file: " << name;
        break;
    }
    case Requests::File::GET_FOLDER_CONTENT:
    {
        QString name = data.at(0).toString();
        result.append(name.toLocal8Bit());
        qDebug() << "Requesting folder: " << name;
        break;
    }
    }
    return result;
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
            default: break;
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
    switch((Requests::Transport)lastRecvFrameHeader.actionType)
    {
    case Requests::Transport::REQUEST_PROGRESS:
    {
        QVariantList dataList;
        dataList.append(lastRecvFrameHeader.data0);
        dataList.append(lastRecvFrameHeader.data1);
        emit sgTransportDataUpdated(Data::Transport::PROGRESS, dataList);
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
    switch((Requests::Playlist)lastRecvFrameHeader.actionType)
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
        emit sgPlaylistDataUpdated(Data::Playlist::PLAYLIST, resultData);
        break;
    }

    case Requests::Playlist::REQUEST_PLAYLIST_POSITION:
    {
        QVariantList resultData;
        resultData.append(lastRecvFrameHeader.data0);
        resultData.append(lastRecvFrameHeader.data1);
        emit sgPlaylistDataUpdated(Data::Playlist::PLAYLIST_POSITION, resultData);
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
    switch((Requests::File)lastRecvFrameHeader.actionType)
    {
    case Requests::File::GET_FILE:
    {
        lastRecvFrame.remove(0, sizeof(FrameHeader));
        QString fileName = lastRecvFrame.left(lastRecvFrameHeader.data0);
        qint32 filesize = static_cast<qint32>(lastRecvFrameHeader.data1);

        fileName.remove(DeviceContentModel().librarySdcardPath);

        QVariantList dataList;
        dataList.append(fileName);

        if(filesize == -1)
        {
            qDebug() << "Requested file not found" << fileName;
            emit sgContentDataUpdated(Data::File::REQUESTED_FILE_NOT_FOUND, dataList);
        }
        else
        {
            lastRecvFrame.remove(0, fileName.size());

            FileManager::savePreviewFile(fileName, lastRecvFrame);

            emit sgContentDataUpdated(Data::File::REQUESTED_FILE, dataList);
            qDebug() << "File downloaded" << fileName;
        }
        break;
    }
    case Requests::File::GET_FOLDER_CONTENT:
    {
        lastRecvFrame.remove(0, sizeof(FrameHeader));

        QString result(lastRecvFrame);
        QStringList resultList = result.split("*", Qt::SkipEmptyParts);
        QString folderName = resultList.at(0);
        result = resultList.at(1);

        resultList = result.split("\r", Qt::SkipEmptyParts);

        QVariantList resultData;
        resultData.append(folderName);
        foreach (QString name, resultList)
        {
            resultData.append(QVariant(name));
        }
        emit sgContentDataUpdated(Data::File::REQUESTED_FOLDER, resultData);
        break;
    }
    }
}

void NetManager::timerEvent()
{
    if(!netClient->isSocketReady())
    {
        netClient->doConnect();
    }
}
