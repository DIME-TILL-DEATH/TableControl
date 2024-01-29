#include "firmwaremanager.h"

FirmwareManager::FirmwareManager(NetManager* netManager, QObject *parent)
    : QObject{parent}
{

    QObject::connect(netManager, &NetManager::sgDeviceConnected, this, &FirmwareManager::slDeviceAvaliable);
    QObject::connect(netManager, &NetManager::sgDataUpdated, this, &FirmwareManager::slDataUpdated);

    QObject::connect(this, &FirmwareManager::sgRequest, netManager, &NetManager::sendRequest);
    QObject::connect(this, &FirmwareManager::sgUpdateData, netManager, &NetManager::slUpdateData);
}

void FirmwareManager::updateFirmware(QString filePath)
{
    QVariantList data;
    data.append(filePath);
    emit sgUpdateData(FrameType::FIRMWARE_ACTIONS, (uint8_t)Requests::Firmware::FIRMWARE_UPLOAD_START, data);
}

QString FirmwareManager::currentFwVersion() const
{
    return m_currentFwVersion;
}

void FirmwareManager::setCurrentFwVersion(const QString &newCurrentFwVersion)
{
    if (m_currentFwVersion == newCurrentFwVersion)
        return;
    m_currentFwVersion = newCurrentFwVersion;
    emit currentFwVersionChanged();
}

void FirmwareManager::slDeviceAvaliable()
{
    emit sgRequest(FrameType::FIRMWARE_ACTIONS, (uint8_t)Requests::Firmware::FIRMWARE_VERSION);
}

void FirmwareManager::slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data)
{
    if(frameType == FrameType::FIRMWARE_ACTIONS)
    {
        switch((Data::Firmware)dataType)
        {
        case Data::Firmware::FIRMWARE_VERSION:
        {
            setCurrentFwVersion(data.at(0).toString());
            break;
        }
        }
    }
}
