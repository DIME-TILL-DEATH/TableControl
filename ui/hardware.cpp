#include "hardware.h"

Hardware::Hardware(AnswerManager *answerManager, RequestManager *requestManager, QObject *parent)
    :   m_requestManager{requestManager},
        QObject{parent}
{
    QObject::connect(this, &Hardware::sgRequest, requestManager, &RequestManager::sgNetRequest);
    QObject::connect(answerManager, &AnswerManager::sgDataUpdated, this, &Hardware::slDataUpdated);

    connect(answerManager, &AnswerManager::sgFiGear2Teeths, this, &Hardware::setFiGear2Teehts);
    connect(answerManager, &AnswerManager::sgMachineMinutes, this, &Hardware::setMachineMinutes);
    connect(answerManager, &AnswerManager::sgPauseInterval, this, &Hardware::setPauseInterval);

    connect(answerManager, &AnswerManager::sgPrintSpeed, this, &Hardware::setPrintSpeed);
    connect(answerManager, &AnswerManager::sgLedBrightness, this, &Hardware::setLedBrightness);
    connect(answerManager, &AnswerManager::sgScaleCoefficient, this, &Hardware::setScaleCoefficient);
    connect(answerManager, &AnswerManager::sgRotation, this, &Hardware::setRotation);
    connect(answerManager, &AnswerManager::sgCorrection, this, &Hardware::setCorrection);

    connect(answerManager, &AnswerManager::sgProgressUpdated, this, &Hardware::setProgress);

    QObject::connect(requestManager, &RequestManager::sgTableAvaliable, this, &Hardware::slTableAvalible);
    QObject::connect(requestManager, &RequestManager::sgTableUnavaliable, this, &Hardware::slTableUnavalible);
}

void Hardware::setPrintProperties()
{
    setScaleCoefficient(m_scaleCoefficient, true);
    setRotation(m_rotation, true);
    setCorrection(m_correction, true);
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
    // case Data::Hardware::PROGRESS:
    // {
    //     quint16 currentPoint = data.at(0).toInt();
    //     quint16 pointsCount = data.at(1).toInt();
    //     setProgress((float)currentPoint/(float)pointsCount);
    //     break;
    // }
    default:
        break;
    }
}

void Hardware::slTableAvalible()
{
    m_deviceAvaliable = true;
    emit deviceAvaliableChanged();
}

void Hardware::slTableUnavalible()
{
    m_deviceAvaliable = false;
    emit deviceAvaliableChanged();
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

void Hardware::pause()
{
    m_requestManager->requestParameter(Requests::Hardware::PAUSE_PRINTING);
}

void Hardware::setPrintSpeed(float newPrintSpeed, bool sendRequest)
{
    m_printSpeed = newPrintSpeed;
    emit sgPrintSpeedChanged();

    if(sendRequest)
        m_requestManager->setHardwareParameter(Requests::Hardware::SET_PRINT_SPEED, m_printSpeed);
}

void Hardware::setLedBrightness(float newLedBrightness, bool sendRequest)
{
    m_ledBrightness = newLedBrightness;
    emit sgLedBrightnessChanged();

    if(sendRequest)
        m_requestManager->setHardwareParameter(Requests::Hardware::SET_LED_BRIGHTNESS, m_ledBrightness);
}

void Hardware::setPauseInterval(quint32 newPauseInterval, bool sendRequest)
{
    if (m_pauseInterval == newPauseInterval)
        return;

    m_pauseInterval = newPauseInterval;
    emit pauseIntervalChanged();

    if(sendRequest)
        m_requestManager->setHardwareParameter(Requests::Hardware::SET_PAUSE_INTERVAL, m_pauseInterval);
}

void Hardware::setScaleCoefficient(float newScaleCoefficient, bool sendRequest)
{
    m_scaleCoefficient = newScaleCoefficient;
    emit scaleCoefficientChanged();

    if(sendRequest)
        m_requestManager->setHardwareParameter(Requests::Hardware::SET_SCALE_COEFFICIENT, m_scaleCoefficient);
}

void Hardware::setRotation(float newRotation, bool sendRequest)
{
    m_rotation = newRotation;
    emit rotationChanged();

    if(sendRequest)
        m_requestManager->setHardwareParameter(Requests::Hardware::SET_ROTATION, m_rotation);

}

void Hardware::setCorrection(float newCorrection, bool sendRequest)
{
    m_correction = newCorrection;
    emit correctionChanged();

    if(sendRequest)
        m_requestManager->setHardwareParameter(Requests::Hardware::SET_CORRECTION, m_correction);
}


void Hardware::setFiGear2Teehts(quint16 fiGear2Teeths)
{
    m_fiGear2Teeths = fiGear2Teeths;
    emit fiGear2TeethsChanged();
}

void Hardware::setMachineMinutes(quint32 machineMinutes)
{
    m_machineMinutes = machineMinutes;
    emit machineMinutesChanged();
}
