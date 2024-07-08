#ifndef FLOATVALUEMESSAGE_H
#define FLOATVALUEMESSAGE_H

#include "abstractmessage.h"

class FloatValueMessage : public AbstractMessage
{
public:
    enum class ActionType
    {
        NOT_FLOAT_VALUE,
        SET,
        GET
    };

    FloatValueMessage(FrameType frameType, uint8_t action, uint32_t value = 0);
    FloatValueMessage(QByteArray recievedData);
    ~FloatValueMessage();


    void formSendData() override;
    void processRecievedData() override;

    ActionType actionType() const {return m_actionType;};
    uint32_t value() const {return m_value;};

    FrameType frameType() const {return m_frameType;};

    uint8_t action() const {return m_action;};

private:
    FrameType m_frameType;
    uint8_t m_action;

    float_t m_value;

    ActionType m_actionType;

    ActionType recognizeActionType();
};

#endif // FLOATVALUEMESSAGE_H
