#ifndef HARDWARE_H
#define HARDWARE_H

#include <QObject>
#include <QVariant>

#include "netmanager.h"

#include "requestactions.h"


class Hardware : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY sgProgressChanged FINAL)
    Q_PROPERTY(float printSpeed READ printSpeed WRITE setPrintSpeed NOTIFY sgPrintSpeedChanged FINAL)
    Q_PROPERTY(float ledBrightness READ ledBrightness WRITE setLedBrightness NOTIFY sgLedBrightnessChanged FINAL)
    Q_PROPERTY(quint32 pauseInterval READ pauseInterval WRITE setPauseInterval NOTIFY pauseIntervalChanged FINAL)

    Q_PROPERTY(float scaleCoefficient READ scaleCoefficient WRITE setScaleCoefficient NOTIFY scaleCoefficientChanged FINAL)
    Q_PROPERTY(float rotation READ rotation WRITE setRotation NOTIFY rotationChanged FINAL)
    Q_PROPERTY(float correction READ correction WRITE setCorrection NOTIFY correctionChanged FINAL)
public:
    explicit Hardware(NetManager* netManager, QObject *parent = nullptr);

    float progress() const;
    void setProgress(float newProgress);

    Q_INVOKABLE void pause() {emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::PAUSE_PRINTING);};
    Q_INVOKABLE void setPrintProperties();

    float printSpeed() const;
    void setPrintSpeed(float newPrintSpeed);

    float ledBrightness() const;
    void setLedBrightness(float newLedBrightness);

    float scaleCoefficient() const;
    void setScaleCoefficient(float newScaleCoefficient);

    float rotation() const;
    void setRotation(float newRotation);

    float correction() const;
    void setCorrection(float newCorrection);

    quint32 pauseInterval() const;
    void setPauseInterval(quint32 newPauseInterval);

signals:
    void sgRequest(FrameType frameType, uint8_t requestType, uint32_t data0 = 0, uint32_t data1 = 0, uint32_t parameters = 0);

    void sgProgressChanged();
    void sgPrintSpeedChanged();
    void sgLedBrightnessChanged();
    void scaleCoefficientChanged();
    void rotationChanged();
    void correctionChanged();


    void pauseIntervalChanged();

public slots:
    void slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList data);

    void slDeviceAvaliable();

private:
    float m_progress{0};
    float m_printSpeed{30};
    float m_ledBrightness{0.5};
    float m_scaleCoefficient{1.0};
    float m_rotation{0};
    float m_correction{0};
    quint32 m_pauseInterval{0};

    void sendFloatRequest(Requests::Hardware requestType, float data);
};

#endif // HARDWARE_H
