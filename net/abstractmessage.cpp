#include <QDebug>

#include "abstractmessage.h"

AbstractMessage::AbstractMessage() {}

AbstractMessage::AbstractMessage(QByteArray rawData)
    : m_rawData(rawData)
{
    if(m_rawData.size() >= sizeof(FrameHeader))
    {
        memcpy(m_frameHeader.rawData, m_rawData.data(), sizeof(FrameHeader));
    }
    else
    {
        qDebug() << __FUNCTION__ << "Error filling frame header. Raw data size("<< rawData.size() <<") is smaller than sizeof(FrameHeader)" << sizeof(FrameHeader);
    }
}

AbstractMessage::~AbstractMessage()
{
}

QByteArray AbstractMessage::rawData() const
{
    return m_rawData;
}

void AbstractMessage::setRawData(const QByteArray &newRawData)
{
    m_rawData = newRawData;
}

AbstractMessage::MessageType AbstractMessage::messageType() const
{
    return m_messageType;
}
