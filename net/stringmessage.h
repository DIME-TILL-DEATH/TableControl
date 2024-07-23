#ifndef STRINGMESSAGE_H
#define STRINGMESSAGE_H

#include <QString>
#include "abstractmessage.h"

class StringMessage : public AbstractMessage
{
public:
    StringMessage(FrameType frameType, uint8_t action, QString string = {});
    StringMessage(QByteArray recievedData);
    ~StringMessage();

    QString string() const {return m_string;};

private:
    QString m_string;
};

#endif // STRINGMESSAGE_H
