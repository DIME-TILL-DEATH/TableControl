#include <QFile>
#include <QFileInfo>

#include "floatvaluemessage.h"
#include "intvaluemessage.h"
#include "stringmessage.h"
#include "filepartmessage.h"
#include "foldercontentmessage.h"

#include "netmanager.h"

// #include <QNetworkAccessManager>

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


    // QNetworkAccessManager manager;
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

            //qDebug() << "Frame recieved" << msg_ptr->rawData();

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
    switch((Requests::Hardware)lastRecvFrameHeader.action)
    {
    case Requests::Hardware::GET_SERIAL_ID: return new StringMessage(lastRecvFrame);
    case Requests::Hardware::REQUEST_PROGRESS: return new IntValueMessage(lastRecvFrame);
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
    case Requests::Playlist::REQUEST_PLAYLIST: return new StringMessage(lastRecvFrame);
    case Requests::Playlist::REQUEST_PLAYLIST_POSITION: return new IntValueMessage(lastRecvFrame);
    case Requests::Playlist::GET_CURRENT_GALLERY:
    case Requests::Playlist::SET_CURRENT_GALLERY: return new StringMessage(lastRecvFrame);
        default: break;
    }
    return nullptr;
}

AbstractMessage* NetManager::processFileAnswer()
{
    switch((Requests::File)lastRecvFrameHeader.action)
    {
    case Requests::File::GET_FILE: return new FilePartMessage(lastRecvFrame);
    case Requests::File::GET_FOLDER_CONTENT: return new FolderContentMessage(lastRecvFrame);
    case Requests::File::FILE_CREATE:
    case Requests::File::FILE_APPEND_DATA: return new FilePartMessage(lastRecvFrame);
        default: break;
    }
    return nullptr;
}

AbstractMessage* NetManager::processFirmwareAnswer()
{
    switch((Requests::Firmware)lastRecvFrameHeader.action)
    {
    case Requests::Firmware::FIRMWARE_UPLOAD_START:
    case Requests::Firmware::FIRMWARE_UPLOAD_PROCEED: return new FilePartMessage(lastRecvFrame);
    case Requests::Firmware::FIRMWARE_UPDATE: return new IntValueMessage(lastRecvFrame);
    case Requests::Firmware::FIRMWARE_VERSION: return new StringMessage(lastRecvFrame);
        default: break;
    }
    return nullptr;
}

void NetManager::timerEvent()
{
    if(!netClient->isSocketReady())
    {
        netClient->doConnect();
    }
}
