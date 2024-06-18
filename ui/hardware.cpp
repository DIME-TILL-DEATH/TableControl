#include "hardware.h"


Hardware::Hardware(NetManager *netManager, QObject *parent)
    : QObject{parent}
{
    QObject::connect(this, &Hardware::sgRequest, netManager, &NetManager::sendRequest);
    QObject::connect(netManager, &NetManager::sgDataUpdated, this, &Hardware::slDataUpdated);
    QObject::connect(netManager, &NetManager::sgDeviceConnected, this, &Hardware::slDeviceAvaliable);
}

float Hardware::progress() const
{
    return m_progress;
}

void Hardware::setProgress(float newProgress)
{
    m_progress = newProgress;
    emit sgProgressChanged();
}

void Hardware::setPrintProperties()
{
    sendFloatRequest(Requests::Hardware::SET_SCALE_COEFFICIENT, m_scaleCoefficient);
    sendFloatRequest(Requests::Hardware::SET_ROTATION, m_rotation);
    sendFloatRequest(Requests::Hardware::SET_CORRECTION, m_correction);
}

void Hardware::slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data)
{
    if(frameType != FrameType::HARDWARE_ACTIONS) return;

    switch ((Data::Hardware)dataType)
    {
    case Data::Hardware::SERIAL_ID:
    {
        m_serialId = data.at(0).toString();
        emit serialIdChanged();
        break;
    }
    case Data::Hardware::PROGRESS:
    {
        quint16 currentPoint = data.at(0).toInt();
        quint16 pointsCount = data.at(1).toInt();
        setProgress((float)currentPoint/(float)pointsCount);
        break;
    }
    case Data::Hardware::PRINT_SPEED:
    {
        m_printSpeed = data.at(0).toFloat();
        qDebug() << "Print speed: " << m_printSpeed;
        emit sgPrintSpeedChanged();
        break;
    }
    case Data::Hardware::LED_BRIGHTNESS:
    {
        m_ledBrightness = data.at(0).toFloat();
        emit sgLedBrightnessChanged();
        break;
    }
    case Data::Hardware::SCALE_COEFFICIENT:
    {
        setScaleCoefficient(data.at(0).toFloat());
        break;
    }
    case Data::Hardware::ROTATION:
    {
        setRotation(data.at(0).toFloat());
        break;
    }
    case Data::Hardware::CORRECTION:
    {
        setCorrection(data.at(0).toFloat());
        break;
    }
    case Data::Hardware::PAUSE_INTERVAL:
    {
        m_pauseInterval = data.at(0).toInt();
        emit pauseIntervalChanged();
        break;
    }
    case Data::Hardware::FI_GEAR2_TEETHS:
    {
        m_fiGear2Teeths = data.at(0).toInt();
        emit fiGear2TeethsChanged();
    }
    case Data::Hardware::MACHINE_MINUTES:
    {
        m_machineMinutes = data.at(0).toInt();
        emit machineMinutesChanged();
    }
    default:
        break;
    }
}

void Hardware::slDeviceAvaliable()
{
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_SERIAL_ID);
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::REQUEST_PROGRESS);
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_PRINT_SPEED);
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_LED_BRIGHTNESS);
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_SCALE_COEFFICIENT);
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_ROTATION);
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_CORRECTION);
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_PAUSE_INTERVAL);
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_FI_GEAR2_TEETH_COUNT);
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_MACHINE_MINUTES);
}

void Hardware::sendFloatRequest(Requests::Hardware requestType, float data)
{
    union { float f; uint32_t i; } u;
    u.f = data;
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)requestType, u.i);
}

void Hardware::setPrintSpeed(float newPrintSpeed)
{
    m_printSpeed = newPrintSpeed;
    emit sgPrintSpeedChanged();

    sendFloatRequest(Requests::Hardware::SET_PRINT_SPEED, m_printSpeed);
}

void Hardware::setLedBrightness(float newLedBrightness)
{
    m_ledBrightness = newLedBrightness;
    emit sgLedBrightnessChanged();

    sendFloatRequest(Requests::Hardware::SET_LED_BRIGHTNESS, m_ledBrightness);
}

void Hardware::setScaleCoefficient(float newScaleCoefficient)
{
    m_scaleCoefficient = newScaleCoefficient;
    emit scaleCoefficientChanged();
}

void Hardware::setRotation(float newRotation)
{
    m_rotation = newRotation;
    emit rotationChanged();
}

void Hardware::setCorrection(float newCorrection)
{
    m_correction = newCorrection;
    emit correctionChanged();
}

void Hardware::setPauseInterval(quint32 newPauseInterval)
{
    if (m_pauseInterval == newPauseInterval)
        return;

    m_pauseInterval = newPauseInterval;
    emit pauseIntervalChanged();
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::SET_PAUSE_INTERVAL, m_pauseInterval);
}
