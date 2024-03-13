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
        break;
    }
    default:
        break;
    }
}

void Hardware::slDeviceAvaliable()
{
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::REQUEST_PROGRESS);
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_PRINT_SPEED);
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_LED_BRIGHTNESS);
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_SCALE_COEFFICIENT);
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_ROTATION);
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_CORRECTION);
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_PAUSE_INTERVAL);
}

void Hardware::sendFloatRequest(Requests::Hardware requestType, float data)
{
    union { float f; uint32_t i; } u;
    u.f = data;
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)requestType, u.i);
}

float Hardware::printSpeed() const
{
    return m_printSpeed;
}

void Hardware::setPrintSpeed(float newPrintSpeed)
{
    m_printSpeed = newPrintSpeed;
    emit sgPrintSpeedChanged();

    sendFloatRequest(Requests::Hardware::SET_PRINT_SPEED, m_printSpeed);
}

float Hardware::ledBrightness() const
{
    return m_ledBrightness;
}

void Hardware::setLedBrightness(float newLedBrightness)
{
    m_ledBrightness = newLedBrightness;
    emit sgLedBrightnessChanged();

    sendFloatRequest(Requests::Hardware::SET_LED_BRIGHTNESS, m_ledBrightness);
}

float Hardware::scaleCoefficient() const
{
    return m_scaleCoefficient;
}

void Hardware::setScaleCoefficient(float newScaleCoefficient)
{
    m_scaleCoefficient = newScaleCoefficient;
    emit scaleCoefficientChanged();
}

float Hardware::rotation() const
{
    return m_rotation;
}

void Hardware::setRotation(float newRotation)
{
    m_rotation = newRotation;
    emit rotationChanged();
}

float Hardware::correction() const
{
    return m_correction;
}

void Hardware::setCorrection(float newCorrection)
{
    m_correction = newCorrection;
    emit correctionChanged();
}
