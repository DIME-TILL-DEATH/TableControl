#ifndef ABSTRACTMESSAGE_H
#define ABSTRACTMESSAGE_H

#include <QByteArray>

#include "frames.h"

class AbstractMessage
{
public:
    typedef enum
    {
        ABSTRACT,
        INT_VALUE,
        FLOAT_VALUE
    }MessageType;

    AbstractMessage();
    AbstractMessage(QByteArray rawData);
    virtual ~AbstractMessage();

    virtual void formSendData(){};
    virtual void processRecievedData(){};

    QByteArray rawData() const;
    void setRawData(const QByteArray &newRawData);

    MessageType messageType() const;

protected:
    QByteArray m_rawData;
    MessageType m_messageType{MessageType::ABSTRACT};

    FrameHeader_uni m_frameHeader;
};

#endif // ABSTRACTMESSAGE_H
