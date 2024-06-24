#include "requestmanager.h"
#include "firmware.h"

#include "devicecontentmodel.h"

RequestManager::RequestManager(QObject *parent)
    : QObject{parent}
{
    updateDataTimer = new QTimer(this);
    updateDataTimer->setInterval(500);

    connect(updateDataTimer, &QTimer::timeout, this, &RequestManager::checkDataUpdate);
}

void RequestManager::slSendRequest(FrameType frameType, uint8_t request, uint32_t data0, uint32_t data1, uint32_t parameters)
{

}

void RequestManager::slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data)
{
    if(frameType == FrameType::FIRMWARE_ACTIONS)
    {
        switch((Data::Firmware)dataType)
        {
        case Data::Firmware::FIRMWARE_VERSION:
        {
            if(Firmware::isVerisonSufficient(data.at(0).toString()))
            {
                emit sgNetRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_SERIAL_ID);
            }
            break;
        }
        }
    }

    if(frameType == FrameType::HARDWARE_ACTIONS)
    {
        switch ((Data::Hardware)dataType)
        {
        case Data::Hardware::SERIAL_ID:
        {
            // Table identified, start working
            updateDataTimer->start();
            getTableBaseData();
            break;
        }
        default: {}
        }
    }
}

void RequestManager::slDeviceConnected()
{
    updateDataTimer->start();
    emit sgNetRequest(FrameType::FIRMWARE_ACTIONS, (uint8_t)Requests::Firmware::FIRMWARE_VERSION);
}

void RequestManager::slDeviceDisconnected()
{
    qDebug() << "Device unavaliable";

    updateDataTimer->stop();
    emit sgTableUnavaliable();
}

void RequestManager::slNetEvent(NetEvents eventType, QString target, QVariantList data)
{

}

void RequestManager::getTableBaseData()
{
    emit sgNetRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::REQUEST_PROGRESS);
    emit sgNetRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_PRINT_SPEED);
    emit sgNetRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_LED_BRIGHTNESS);
    emit sgNetRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_SCALE_COEFFICIENT);
    emit sgNetRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_ROTATION);
    emit sgNetRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_CORRECTION);
    emit sgNetRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_PAUSE_INTERVAL);
    emit sgNetRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_FI_GEAR2_TEETH_COUNT);
    emit sgNetRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_MACHINE_MINUTES);

    emit sgNetRequest(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::REQUEST_PLAYLIST);
    emit sgNetRequest(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::REQUEST_PLAYLIST_POSITION);
    emit sgNetRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::REQUEST_PROGRESS);

    QVariantList data;
    data.append(DeviceContentModel::librarySdcardPath);
    emit sgUpdateData(FrameType::FILE_ACTIONS, (uint8_t)(Requests::File::GET_FOLDER_CONTENT), data);

    emit sgTableAvaliable();
}

void RequestManager::checkDataUpdate()
{
    emit sgNetRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::REQUEST_PROGRESS);
    emit sgNetRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_MACHINE_MINUTES);
    emit sgNetRequest(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::REQUEST_PLAYLIST_POSITION);
}
