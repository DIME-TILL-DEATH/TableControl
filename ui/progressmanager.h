#ifndef PROGRESSMANAGER_H
#define PROGRESSMANAGER_H

#include <QObject>

#include "netmanager.h"

class ProgressManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal currentProgress READ currentProgress WRITE setCurrentProgress NOTIFY currentProgressChanged FINAL)
    Q_PROPERTY(qreal firmwareUploadProgress READ firmwareUploadProgress WRITE setFirmwareUploadProgress NOTIFY firmwareUploadProgressChanged FINAL)
    Q_PROPERTY(bool updatingAndReboot READ updatingAndReboot WRITE setUpdatingAndReboot NOTIFY updatingAndRebootChanged FINAL)
public:
    explicit ProgressManager(NetManager* netManager, QObject *parent = nullptr);

    qreal currentProgress() const;
    void setCurrentProgress(qreal newCurrentProgress);

    enum class Error
    {
        UploadFailed,
        FirmwareUploadFailed
    };

    qreal firmwareUploadProgress() const;
    void setFirmwareUploadProgress(qreal newFirmwareUploadProgress);

    bool updatingAndReboot() const;
    void setUpdatingAndReboot(bool newUpdatingAndReboot);

signals:

    void currentProgressChanged();

    void errorOccured(Error errorType, QVariant data);

    void firmwareUploadProgressChanged();

    void updatingAndRebootChanged();

public slots:
    void slUpdate(NetEvents eventType, QString target, QVariantList data);
    void slDeviceAvalible();

private:
    qreal m_currentProgress{1.0};
    qreal m_firmwareUploadProgress{1.0};
    bool m_updatingAndReboot{false};

    QMap<QString, QPair<qint64, qint64> > m_activeProcesses;

    qint32 firmwareUploadedBytes{0};
};

#endif // PROGRESSMANAGER_H
