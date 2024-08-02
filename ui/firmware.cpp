#include "firmware.h"

#ifdef __ANDROID__
#include <jni.h>
#include "androidutils.h"
#endif

Firmware::Firmware(AnswerManager* answerManager, RequestManager *requestManager, QObject *parent)
    : QObject{parent}
{
    m_requestManager = requestManager;

    QObject::connect(answerManager, &AnswerManager::sgFirmwareVersion, this, &Firmware::setCurrentFwVersion);

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
    m_requestManager->updateFirmware(filePath);
}

void Firmware::setCurrentFwVersion(const QString &newCurrentFwVersion)
{
    if (m_currentFwVersion == newCurrentFwVersion)
        return;

    m_currentFwVersion = newCurrentFwVersion;

    if(!Firmware::isVerisonSufficient(m_currentFwVersion))
    {
        qDebug() << "Firmware version insufficient(" << m_currentFwVersion << ")";
        emit sgFirmwareVersionInsufficient();
    }

    emit currentFwVersionChanged();
}

bool Firmware::isVerisonSufficient(QString versionString, QString mininalVersionString)
{
    FirmwareVersion minimalFw = extractFirmwareVersion(mininalVersionString);
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
