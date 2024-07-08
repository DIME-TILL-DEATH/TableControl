#include "intvaluemessage.h"

#include <QDebug>
#include "requestactions.h"

IntValueMessage::IntValueMessage(FrameType frameType, uint8_t action, uint32_t value1, uint32_t value2)
    : m_value1(value1),
    m_value2(value2),
    m_frameType(frameType),
    m_action(action)
{
    m_messageType = AbstractMessage::INT_VALUE;
    IntValueMessage::formSendData();
}

IntValueMessage::IntValueMessage(QByteArray recievedData)
                : AbstractMessage(recievedData)
{
    m_messageType = AbstractMessage::INT_VALUE;
    IntValueMessage::processRecievedData();
}

IntValueMessage::~IntValueMessage()
{
}

void IntValueMessage::formSendData()
{
    m_frameHeader.structData.frameType = m_frameType;
    m_frameHeader.structData.action = m_action;
    m_frameHeader.structData.frameSize = sizeof(FrameHeader);
    
    m_frameHeader.structData.data0 = m_value1;
    m_frameHeader.structData.data1 = m_value2;
    m_frameHeader.structData.frameParameters = 0;

    m_rawData.append(m_frameHeader.rawData, sizeof(FrameHeader));
}

void IntValueMessage::processRecievedData()
{
    memcpy(m_frameHeader.rawData, m_rawData.data(), sizeof(FrameHeader));

    m_frameType = m_frameHeader.structData.frameType;
    m_action = m_frameHeader.structData.action;
    m_value1 = m_frameHeader.structData.data0;
    m_value2 = m_frameHeader.structData.data1;

    m_actionType = recognizeActionType();
}

IntValueMessage::ActionType IntValueMessage::recognizeActionType()
{
    switch (m_frameType)
    {
    case PLAYLIST_ACTIONS:
    {
        switch(static_cast<Requests::Playlist>(m_action))
        {
        case Requests::Playlist::REQUEST_PLAYLIST_POSITION:
        case Requests::Playlist::CHANGE_PLAYLIST_POSITION: return ActionType::SET;
        case Requests::Playlist::CHANGE_PRINTNG_FILE: return ActionType::SET;
        default: return ActionType::NOT_INT_VALUE;
        }
    }

    case HARDWARE_ACTIONS:
    {
        switch(static_cast<Requests::Hardware>(m_action))
        {
        case Requests::Hardware::GET_PAUSE_INTERVAL: return ActionType::GET;
        case Requests::Hardware::SET_PAUSE_INTERVAL: return ActionType::SET;
        case Requests::Hardware::GET_FI_GEAR2_TEETH_COUNT: return ActionType::GET;
        case Requests::Hardware::GET_MACHINE_MINUTES: return ActionType::GET;
        default: return ActionType::NOT_INT_VALUE;
        }
    }

    default: return ActionType::NOT_INT_VALUE;
    }
}
