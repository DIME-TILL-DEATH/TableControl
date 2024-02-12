#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <QObject>
#include <QVariant>

#include "netmanager.h"

#include "requestactions.h"


class Transport : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY sgProgressChanged FINAL)
    Q_PROPERTY(float printSpeed READ printSpeed WRITE setPrintSpeed NOTIFY sgPrintSpeedChanged FINAL)

public:
    explicit Transport(NetManager* netManager, QObject *parent = nullptr);

    float progress() const;
    void setProgress(float newProgress);

    Q_INVOKABLE void pause() {emit sgRequest(FrameType::TRANSPORT_ACTIONS, (uint8_t)Requests::Transport::PAUSE_PRINTING);};
    //Q_INVOKABLE void setSpeed(qreal value) {emit sgRequest(FrameType::TRANSPORT_ACTIONS, (uint8_t)Requests::Transport::SET_PRINT_SPEED, value);};

    float printSpeed() const;
    void setPrintSpeed(float newPrintSpeed);

signals:
    void sgRequest(FrameType frameType, uint8_t requestType, uint32_t data0 = 0, uint32_t data1 = 0, uint32_t parameters = 0);

    void sgProgressChanged();

    void sgPrintSpeedChanged();

public slots:
    void slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data);

    void slDeviceAvaliable();

private:
    float m_progress{0};
    float m_printSpeed;
};

#endif // TRANSPORT_H
