#ifndef ANSWERMANAGER_H
#define ANSWERMANAGER_H

#include <QObject>


#include "netmanager.h"

#include "intvaluemessage.h"
#include "floatvaluemessage.h"

class AnswerManager : public QObject
{
    Q_OBJECT
public:
    explicit AnswerManager(QObject *parent = nullptr);

public slots:
    void slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data);
    void slNetEvent(NetEvents eventType, QString target, QVariantList data = {});

    void slRecievingMessage(std::shared_ptr<AbstractMessage> msg_ptr);
signals:
    void sgDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data);
    void sgNetEvent(NetEvents eventType, QString target, QVariantList data = {});

    // Int
    void sgFiGear2Teeths(quint16 teethsCount);
    void sgMachineMinutes(quint32 machineMinutes);
    void sgPauseInterval(quint32 pauseInterval, bool sendrequest=false);

    // Float
    void sgPrintSpeed(float value, bool sendRequest = false);
    void sgLedBrightness(float value, bool sendRequest = false);
    void sgScaleCoefficient(float value, bool sendRequest = false);
    void sgRotation(float value, bool sendRequest = false);
    void sgCorrection(float value, bool sendRequest = false);

    // Others
    void sgProgressUpdated(float progress);

private:
    void processIntValues(IntValueMessage* message);
    void processFloatValues(FloatValueMessage* message);
};

#endif // ANSWERMANAGER_H
