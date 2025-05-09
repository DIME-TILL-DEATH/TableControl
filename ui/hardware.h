#ifndef HARDWARE_H
#define HARDWARE_H

#include <QObject>
#include <QVariant>

#include "requestmanager.h"
#include "answermanager.h"

class Hardware : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool deviceAvaliable READ deviceAvaliable NOTIFY deviceAvaliableChanged FINAL)

    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY sgProgressChanged FINAL)
    Q_PROPERTY(float printSpeed READ printSpeed WRITE setPrintSpeed NOTIFY sgPrintSpeedChanged FINAL)
    Q_PROPERTY(float ledBrightness READ ledBrightness WRITE setLedBrightness NOTIFY sgLedBrightnessChanged FINAL)
    Q_PROPERTY(quint32 pauseInterval READ pauseInterval WRITE setPauseInterval NOTIFY pauseIntervalChanged FINAL)
    Q_PROPERTY(quint16 fiGear2Teeths READ fiGear2Teeths WRITE setFiGear2Teeths NOTIFY fiGear2TeethsChanged)
    Q_PROPERTY(quint32 machineMinutes READ machineMinutes NOTIFY machineMinutesChanged FINAL)
    Q_PROPERTY(QString serialId READ serialId NOTIFY serialIdChanged FINAL)

    Q_PROPERTY(float scaleCoefficient READ scaleCoefficient WRITE setScaleCoefficient NOTIFY scaleCoefficientChanged FINAL)
    Q_PROPERTY(float rotation READ rotation WRITE setRotation NOTIFY rotationChanged FINAL)
    Q_PROPERTY(float correction READ correction WRITE setCorrection NOTIFY correctionChanged FINAL)

    Q_PROPERTY(bool firstMotorInversion READ firstMotorInversion WRITE setFirstMotorInversion NOTIFY firstMotorInversionChanged FINAL)
    Q_PROPERTY(bool secondMotorInversion READ secondMotorInversion WRITE setSecondMotorInversion NOTIFY secondMotorInversionChanged FINAL)
public:
    explicit Hardware(AnswerManager* answerManager, RequestManager* requestManager, QObject *parent = nullptr);

    Q_INVOKABLE void pause();
    Q_INVOKABLE void setPrintProperties();

    QString serialId() const {return m_serialId;};
    void setSerialId(QString serialId);

    float progress() const {return m_progress;};
    void setProgress(float newProgress);

    bool deviceAvaliable() const {return m_deviceAvaliable;};

    float printSpeed() const {return m_printSpeed;};
    void setPrintSpeed(float newPrintSpeed, bool sendRequest = true);

    float ledBrightness() const {return m_ledBrightness;};
    void setLedBrightness(float newLedBrightness, bool sendRequest = true);

    float scaleCoefficient() const {return m_scaleCoefficient;};
    void setScaleCoefficient(float newScaleCoefficient, bool sendRequest = false);

    float rotation() const {return m_rotation;};
    void setRotation(float newRotation, bool sendRequest = false);

    float correction() const {return m_correction;};
    void setCorrection(float newCorrection, bool sendRequest = false);

    quint32 pauseInterval() const {return m_pauseInterval;};
    void setPauseInterval(quint32 newPauseInterval, bool sendRequest = true);

    quint16 fiGear2Teeths() const {return m_fiGear2Teeths;};
    void setFiGear2Teeths(quint32 fiGear2Teeths, bool sendRequest = true);

    quint32 machineMinutes() const {return m_machineMinutes;};
    void setMachineMinutes(quint32 machineMinutes);

    bool firstMotorInversion() const {return m_firstMotorInversion;};
    void setFirstMotorInversion(bool newFirstMotorInversion, bool sendRequest = true);

    bool secondMotorInversion() const {return m_secondMotorInversion;};
    void setSecondMotorInversion(bool newSecondMotorInversion, bool sendRequest = true);

signals:
    void deviceAvaliableChanged();
    void sgProgressChanged();
    void sgPrintSpeedChanged();
    void sgLedBrightnessChanged();
    void scaleCoefficientChanged();
    void rotationChanged();
    void correctionChanged();
    void pauseIntervalChanged();
    void fiGear2TeethsChanged();
    void machineMinutesChanged();
    void serialIdChanged();

    void firstMotorInversionChanged();
    void secondMotorInversionChanged();

public slots:
    void slTableAvalible();
    void slTableUnavalible();

private:
    float m_progress{0};
    float m_printSpeed{30};
    float m_ledBrightness{0.5};
    float m_scaleCoefficient{1.0};
    float m_rotation{0};
    float m_correction{0};
    quint32 m_pauseInterval{0};

    quint16 m_fiGear2Teeths{0};
    quint32 m_machineMinutes{0};
    QString m_serialId;
    bool m_deviceAvaliable;

    RequestManager* m_requestManager;
    bool m_firstMotorInversion;
    bool m_secondMotorInversion;
};

#endif // HARDWARE_H
