#ifndef ANSWERMANAGER_H
#define ANSWERMANAGER_H

#include <QObject>

#include "netmanager.h"

#include "intvaluemessage.h"
#include "floatvaluemessage.h"
#include "stringmessage.h"
#include "filepartmessage.h"
#include "foldercontentmessage.h"

class AnswerManager : public QObject
{
    Q_OBJECT
public:
    explicit AnswerManager(QObject *parent = nullptr);

public slots:
    void slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data);
    void slNetEvent(NetEvents eventType, QString target, QVariantList data = {});
    
    void slRecieveMessage(std::shared_ptr<AbstractMessage> msg_ptr);
signals:
    void sgDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data);
    void sgSendMessage(std::shared_ptr<AbstractMessage> msg_ptr);
    void sgNetEvent(NetEvents eventType, QString target, QVariantList data = {});

    // Int
    void sgFiGear2Teeths(quint16 teethsCount);
    void sgMachineMinutes(quint32 machineMinutes);
    void sgPauseInterval(quint32 pauseInterval, bool sendrequest=false);
    void sgPlaylistPosition(qint32 newPlsPos);

    // Float
    void sgPrintSpeed(float value, bool sendRequest = false);
    void sgLedBrightness(float value, bool sendRequest = false);
    void sgScaleCoefficient(float value, bool sendRequest = false);
    void sgRotation(float value, bool sendRequest = false);
    void sgCorrection(float value, bool sendRequest = false);

    // String
    void sgSerialId(QString serialId);
    void sgFirmwareVersion(QString firmwareVersion);
    void sgPlaylist(QStringList newPlaylist);
    void sgCurrentGalleryName(QString galleryName);

    // Others
    void sgProgress(float progress);
    void sgFilePartDownloaded(QString filePath, const QByteArray& filePart, int64_t partPosition, int64_t fileSize);
    void sgFileNotFound(QString filePath);
    void sgFolderContent(QString folderPath, QStringList folderContent);

private:
    void processMessage(IntValueMessage* message);
    void processMessage(FloatValueMessage* message);
    void processMessage(StringMessage* message);
    void processMessage(FilePartMessage* message);

    void updateFileUploadProgress(NetEvents type, QString filePath, quint64 currentPartSize, quint64 totalSize);

    QString currentFw;
};

#endif // ANSWERMANAGER_H
