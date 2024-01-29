#include "uitransport.h"


UiTransport::UiTransport(NetManager *netManager, QObject *parent)
    : QObject{parent}
{
    QObject::connect(this, &UiTransport::sgRequest, netManager, &NetManager::sendRequest);
    QObject::connect(netManager, &NetManager::sgDataUpdated, this, &UiTransport::slDataUpdated);
}

float UiTransport::progress() const
{
    return m_progress;
}

void UiTransport::setProgress(float newProgress)
{
    if (qFuzzyCompare(m_progress, newProgress))
        return;
    m_progress = newProgress;
    emit sgProgressChanged();
}

void UiTransport::slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data)
{
    if(frameType != FrameType::TRANSPORT_ACTIONS) return;

    switch ((Data::Transport)dataType)
    {
    case Data::Transport::PROGRESS:
    {
        quint16 currentPoint = data.at(0).toInt();
        quint16 pointsCount = data.at(1).toInt();
        setProgress((float)currentPoint/(float)pointsCount);
        break;
    }
    default:
        break;
    }
}
