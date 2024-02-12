#include "transport.h"


Transport::Transport(NetManager *netManager, QObject *parent)
    : QObject{parent}
{
    QObject::connect(this, &Transport::sgRequest, netManager, &NetManager::sendRequest);
    QObject::connect(netManager, &NetManager::sgDataUpdated, this, &Transport::slDataUpdated);
    QObject::connect(netManager, &NetManager::sgDeviceConnected, this, &Transport::slDeviceAvaliable);
}

float Transport::progress() const
{
    return m_progress;
}

void Transport::setProgress(float newProgress)
{
    if (qFuzzyCompare(m_progress, newProgress))
        return;
    m_progress = newProgress;
    emit sgProgressChanged();
}

void Transport::slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data)
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
    case Data::Transport::PRINT_SPEED:
    {
        m_printSpeed = data.at(0).toFloat();
        qDebug() << "Print speed: " << m_printSpeed;
        emit sgPrintSpeedChanged();
    }
    default:
        break;
    }
}

void Transport::slDeviceAvaliable()
{
    emit sgRequest(FrameType::TRANSPORT_ACTIONS, (uint8_t)Requests::Transport::REQUEST_PROGRESS);
    emit sgRequest(FrameType::TRANSPORT_ACTIONS, (uint8_t)Requests::Transport::GET_PRINT_SPEED);
}

float Transport::printSpeed() const
{
    return m_printSpeed;
}

void Transport::setPrintSpeed(float newPrintSpeed)
{
    if (qFuzzyCompare(m_printSpeed, newPrintSpeed))
        return;
    m_printSpeed = newPrintSpeed;
    emit sgPrintSpeedChanged();

    emit sgRequest(FrameType::TRANSPORT_ACTIONS, (uint8_t)Requests::Transport::SET_PRINT_SPEED, newPrintSpeed);
}
