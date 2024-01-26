#include "firmwaremanager.h"

FirmwareManager::FirmwareManager(NetManager* netManager, QObject *parent)
    : QObject{parent}
{
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
