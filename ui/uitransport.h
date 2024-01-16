#ifndef UITRANSPORT_H
#define UITRANSPORT_H

#include <QObject>
#include <QVariant>

#include "frames.h"
#include "requestactions.h"

class UiTransport : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY sgProgressChanged FINAL)

public:
    explicit UiTransport(QObject *parent = nullptr);

    float progress() const;
    void setProgress(float newProgress);

    Q_INVOKABLE void pause() {emit sgRequest(FrameType::TRANSPORT_ACTIONS, (uint8_t)Requests::Transport::PAUSE_PRINTING);};
    Q_INVOKABLE void next() {emit sgRequest(FrameType::TRANSPORT_ACTIONS, (uint8_t)Requests::Transport::NEXT_PRINT);};
    Q_INVOKABLE void previous() {emit sgRequest(FrameType::TRANSPORT_ACTIONS, (uint8_t)Requests::Transport::PREVIOUS_PRINT);};

    //temporally:
    Q_INVOKABLE void requestPlaylist()
    {
        emit sgRequest(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::REQUEST_PLAYLIST);

    }

signals:
    void sgRequest(FrameType frameType, uint8_t requestType, uint32_t data0 = 0, uint32_t data1 = 0, uint32_t parameters = 0);
    void sgTransportAction(Requests::Transport action, QVariant data);

    void sgProgressChanged();

public slots:
    void slTransportUpdate(Data::Transport transportData, QVariantList dataList);

private:
    float m_progress{0};
};

#endif // UITRANSPORT_H
