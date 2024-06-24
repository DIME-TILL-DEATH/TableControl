#include "firmware.h"

#ifdef __ANDROID__
#include <jni.h>
#include "androidutils.h"
#endif

Firmware::Firmware(AnswerManager* answerManager, RequestManager *requestManager, QObject *parent)
    : QObject{parent}
{
    QObject::connect(this, &Firmware::sgUpdateData, requestManager, &RequestManager::sgUpdateData);
    QObject::connect(answerManager, &AnswerManager::sgDataUpdated, this, &Firmware::slDataUpdated);

#ifdef Q_OS_ANDROID
    QObject::connect(&activityResultHandler, &ActivityResultManager::sgFilePicked, this, &Firmware::slAndroidFilePicked);
#endif
}

void Firmware::selectFile()
{
#ifdef Q_OS_ANDROID
    AndroidUtils::pickFile(ActivityType::PICK_FILE, "*/*", &activityResultHandler);
#else
    emit sgOpenPlatformFileDialog();
    qDebug() << "emiting";
#endif
}

void Firmware::slAndroidFilePicked(QString filePath, QString fileName)
{
    updateFirmware(filePath);
}

void Firmware::updateFirmware(QString filePath)
{   
    QVariantList data;
    data.append(filePath);
    emit sgUpdateData(FrameType::FIRMWARE_ACTIONS, (uint8_t)Requests::Firmware::FIRMWARE_UPLOAD_START, data);
}

QString Firmware::currentFwVersion() const
{
    return m_currentFwVersion;
}

void Firmware::setCurrentFwVersion(const QString &newCurrentFwVersion)
{
    if (m_currentFwVersion == newCurrentFwVersion)
        return;
    m_currentFwVersion = newCurrentFwVersion;
    emit currentFwVersionChanged();
}

bool Firmware::isVerisonSufficient(QString versionString)
{
    FirmwareVersion minimalFw = extractFirmwareVersion(MINIMAL_FIRMWARE_VERSION);
    FirmwareVersion tableFw = extractFirmwareVersion(versionString);

    if(minimalFw.major > tableFw.major) return false;
    if(minimalFw.major < tableFw.major) return true;

    if(minimalFw.minor > tableFw.minor) return false;
    if(minimalFw.minor < tableFw.minor) return true;

    return true;
}

FirmwareVersion Firmware::extractFirmwareVersion(QString versionString)
{
    QStringList resultList = versionString.split('.');

    FirmwareVersion result;

    if(resultList.count()==2)
    {
        result.major = resultList.at(0).toInt();
        result.minor = resultList.at(1).toInt();
    }
    return result;
}

void Firmware::slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data)
{
    if(frameType != FrameType::FIRMWARE_ACTIONS) return;

    switch((Data::Firmware)dataType)
    {
    case Data::Firmware::FIRMWARE_VERSION:
    {
        setCurrentFwVersion(data.at(0).toString());
        if(!Firmware::isVerisonSufficient(m_currentFwVersion))
        {
            qDebug() << "Firmware version insufficient(" << m_currentFwVersion << ")";
            emit sgFirmwareVersionInsufficient();
        }

        break;
    }
    }
}