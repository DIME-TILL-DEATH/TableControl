#include "answermanager.h"

AnswerManager::AnswerManager(QObject *parent)
    : QObject{parent}
{}

void AnswerManager::slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data)
{
    emit sgDataUpdated(frameType, dataType, data);
}

void AnswerManager::slNetEvent(NetEvents eventType, QString target, QVariantList data)
{
    emit sgNetEvent(eventType, target, data);
}

void AnswerManager::slRecievingMessage(std::shared_ptr<AbstractMessage> msg_ptr)
{
    switch(msg_ptr->messageType())
    {
    case AbstractMessage::ABSTRACT: break;
    case AbstractMessage::INT_VALUE:
    {
        IntValueMessage* int_msg = dynamic_cast<IntValueMessage*>(msg_ptr.get());
        processIntValues(int_msg);
        break;
    }
    case AbstractMessage::FLOAT_VALUE:
        FloatValueMessage* float_msg = dynamic_cast<FloatValueMessage*>(msg_ptr.get());
        processFloatValues(float_msg);
        break;
    }
}

void AnswerManager::processIntValues(IntValueMessage *message)
{
    switch(message->frameType())
    {

    case UNDEFINED: return;
    case PLAYLIST_ACTIONS:
    {
        break;
    }
    case HARDWARE_ACTIONS:
    {
        switch(static_cast<Requests::Hardware>(message->action()))
        {
        case Requests::Hardware::PAUSE_PRINTING: break;
        case Requests::Hardware::REQUEST_PROGRESS:
        {
            emit sgProgressUpdated((float)message->firstValue()/(float)message->secondValue());
            break;
        }
        case Requests::Hardware::GET_PAUSE_INTERVAL:
        {
            emit sgPauseInterval(message->value());
            qDebug() << "Pause interval: " << message->value();
            break;
        }
        case Requests::Hardware::GET_FI_GEAR2_TEETH_COUNT:
        {
            emit sgFiGear2Teeths(message->value());
            qDebug() << "Fi gear2 teeth count: " << message->value();
            break;
        }
        case Requests::Hardware::GET_MACHINE_MINUTES:
        {
            emit sgMachineMinutes(message->value());
            break;
        }
        default: break;
        }
    }
    case FILE_ACTIONS: return;
    case FIRMWARE_ACTIONS: return;
        break;
    }
}

void AnswerManager::processFloatValues(FloatValueMessage *message)
{
    qDebug() << "Processing float value";
    switch(message->frameType())
    {
    case HARDWARE_ACTIONS:
    {
        switch(static_cast<Requests::Hardware>(message->action()))
        {
        case Requests::Hardware::GET_PRINT_SPEED:
        case Requests::Hardware::SET_PRINT_SPEED:
        {
            emit sgPrintSpeed(message->value());
            qDebug() << "Print speed: " << message->value();
            break;
        }
        case Requests::Hardware::GET_LED_BRIGHTNESS:
        case Requests::Hardware::SET_LED_BRIGHTNESS:
        {
            emit sgLedBrightness(message->value());
            qDebug() << "Led brightness: " << message->value();
            break;
        }
        case Requests::Hardware::GET_SCALE_COEFFICIENT:
        case Requests::Hardware::SET_SCALE_COEFFICIENT:
        {
            emit sgScaleCoefficient(message->value());
            qDebug() << "Scale coefficient: " << message->value();
            break;
        }
        case Requests::Hardware::GET_ROTATION:
        case Requests::Hardware::SET_ROTATION:
        {
            emit sgRotation(message->value());
            qDebug() << "Rotation: " << message->value();
            break;
        }
        case Requests::Hardware::GET_CORRECTION:
        case Requests::Hardware::SET_CORRECTION:
        {
            emit sgCorrection(message->value());
            qDebug() << "Correction: " << message->value();
            break;
        }
        default: break;
        }
    }
    default: break;
    }
}
