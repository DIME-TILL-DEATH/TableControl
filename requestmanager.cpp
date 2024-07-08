#include "requestmanager.h"
#include "firmware.h"

#include "devicecontentmodel.h"

#include "intvaluemessage.h"

RequestManager::RequestManager(QObject *parent)
    : QObject{parent}
{
    updateDataTimer = new QTimer(this);
    updateDataTimer->setInterval(500);

    connect(updateDataTimer, &QTimer::timeout, this, &RequestManager::checkDataUpdate);
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
                // emit sgNetRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_SERIAL_ID);
                requestParameter(Requests::Hardware::GET_SERIAL_ID);
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
    // emit sgNetRequest(FrameType::FIRMWARE_ACTIONS, (uint8_t)Requests::Firmware::FIRMWARE_VERSION);
    requestParameter(Requests::Firmware::FIRMWARE_VERSION);
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
    requestParameter(Requests::Hardware::REQUEST_PROGRESS);
    requestParameter(Requests::Hardware::GET_PRINT_SPEED);
    requestParameter(Requests::Hardware::GET_LED_BRIGHTNESS);
    requestParameter(Requests::Hardware::GET_SCALE_COEFFICIENT);
    requestParameter(Requests::Hardware::GET_ROTATION);
    requestParameter(Requests::Hardware::GET_CORRECTION);
    requestParameter(Requests::Hardware::GET_PAUSE_INTERVAL);
    requestParameter(Requests::Hardware::GET_FI_GEAR2_TEETH_COUNT);
    requestParameter(Requests::Hardware::GET_MACHINE_MINUTES);

    // emit sgNetRequest(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::REQUEST_PLAYLIST);
    // emit sgNetRequest(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::REQUEST_PLAYLIST_POSITION);
    requestParameter(Requests::Playlist::REQUEST_PLAYLIST);
    requestParameter(Requests::Playlist::REQUEST_PLAYLIST_POSITION);
    requestParameter(Requests::Hardware::REQUEST_PROGRESS);

    QVariantList data;
    data.append(DeviceContentModel::librarySdcardPath);
    emit sgUpdateData(FrameType::FILE_ACTIONS, (uint8_t)(Requests::File::GET_FOLDER_CONTENT), data);

    emit sgTableAvaliable();
}

void RequestManager::checkDataUpdate()
{
    requestParameter(Requests::Hardware::REQUEST_PROGRESS);
    requestParameter(Requests::Hardware::GET_MACHINE_MINUTES);
    // emit sgNetRequest(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::REQUEST_PLAYLIST_POSITION);
    requestParameter(Requests::Playlist::REQUEST_PLAYLIST_POSITION);
}

void RequestManager::requestParameter(Requests::Hardware request)
{
    emit sgSendMessage(std::shared_ptr<AbstractMessage>(new IntValueMessage(FrameType::HARDWARE_ACTIONS, (uint8_t)request)));
}

void RequestManager::setHardwareParameter(Requests::Hardware request, uint32_t value)
{
    IntValueMessage* msg = new IntValueMessage(FrameType::HARDWARE_ACTIONS, (uint8_t)request, value);
    emit sgSendMessage(std::shared_ptr<AbstractMessage>(msg));
}

void RequestManager::setHardwareParameter(Requests::Hardware request, float_t value)
{
    FloatValueMessage* msg = new FloatValueMessage(FrameType::HARDWARE_ACTIONS, (uint8_t)request, value);
    emit sgSendMessage(std::shared_ptr<AbstractMessage>(msg));
}

void RequestManager::requestParameter(Requests::Playlist request)
{
    emit sgSendMessage(std::shared_ptr<AbstractMessage>(new IntValueMessage(FrameType::PLAYLIST_ACTIONS, (uint8_t)request)));
}

void RequestManager::requestParameter(Requests::Firmware request)
{
    emit sgSendMessage(std::shared_ptr<AbstractMessage>(new IntValueMessage(FrameType::FIRMWARE_ACTIONS, (uint8_t)request)));
}
