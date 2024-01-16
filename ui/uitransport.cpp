#include "uitransport.h"


UiTransport::UiTransport(QObject *parent)
    : QObject{parent}
{

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

void UiTransport::slTransportUpdate(Data::Transport transportData, QVariantList dataList)
{
    switch (transportData)
    {
        case Data::Transport::PROGRESS:
        {
            quint16 currentPoint = dataList.at(0).toInt();
            quint16 pointsCount = dataList.at(1).toInt();
            setProgress((float)currentPoint/(float)pointsCount);
                break;
        }
        default:
            break;
    }
}
