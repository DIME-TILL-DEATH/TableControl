#ifndef UITRANSPORT_H
#define UITRANSPORT_H

#include <QObject>
#include <QVariant>

#include "netmanager.h"

#include "requestactions.h"


class UiTransport : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY sgProgressChanged FINAL)

public:
    explicit UiTransport(NetManager* netManager, QObject *parent = nullptr);

    float progress() const;
    void setProgress(float newProgress);

    Q_INVOKABLE void pause() {emit sgRequest(FrameType::TRANSPORT_ACTIONS, (uint8_t)Requests::Transport::PAUSE_PRINTING);};

signals:
    void sgRequest(FrameType frameType, uint8_t requestType, uint32_t data0 = 0, uint32_t data1 = 0, uint32_t parameters = 0);

    void sgProgressChanged();

public slots:
    void slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data);

private:
    float m_progress{0};
};

#endif // UITRANSPORT_H
