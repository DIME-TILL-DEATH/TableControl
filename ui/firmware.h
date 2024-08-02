#ifndef FIRMWARE_H
#define FIRMWARE_H

#include <QObject>

#include "requestmanager.h"
#include "answermanager.h"

#ifdef __ANDROID__
#include <jni.h>
#include "activityresultmanager.h"
#endif

struct FirmwareVersion
{
    quint8 major;
    quint8 minor;
};

class Firmware : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentFwVersion READ currentFwVersion WRITE setCurrentFwVersion NOTIFY currentFwVersionChanged FINAL)
public:
    explicit Firmware(AnswerManager* answerManager, RequestManager *requestManager, QObject *parent = nullptr);

    Q_INVOKABLE void selectFile();
    Q_INVOKABLE void updateFirmware(QString filePath);

    QString currentFwVersion() const {return m_currentFwVersion;};
    void setCurrentFwVersion(const QString &newCurrentFwVersion);

    static bool isVerisonSufficient(QString versionString, QString minimalVersionString = MINIMAL_FIRMWARE_VERSION);

    constexpr static const char dstPath[] = "/sdcard/firmware.bin";

signals:
    void sgOpenPlatformFileDialog();
    void sgFirmwareVersionInsufficient();

    void currentFwVersionChanged();

public slots:
    void slAndroidFilePicked(QString filePath, QString fileName);

private:
    RequestManager* m_requestManager;

    QString m_currentFwVersion;
    static FirmwareVersion extractFirmwareVersion(QString versionString);

#ifdef __ANDROID__
    ActivityResultManager activityResultHandler;
#endif
};

#endif // FIRMWARE_H
