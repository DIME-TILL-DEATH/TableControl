#ifndef PROGRESS_H
#define PROGRESS_H

#include <QObject>

#include "answermanager.h"
#include "requestmanager.h"

class Progress : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal currentProgress READ currentProgress WRITE setCurrentProgress NOTIFY currentProgressChanged FINAL)
    Q_PROPERTY(qreal firmwareUploadProgress READ firmwareUploadProgress WRITE setFirmwareUploadProgress NOTIFY firmwareUploadProgressChanged FINAL)
    Q_PROPERTY(bool updatingState READ updatingState WRITE setUpdatingState NOTIFY updatingStateChanged FINAL)
public:
    explicit Progress(AnswerManager* answerManager, RequestManager* requestManager, QObject *parent = nullptr);

    qreal currentProgress() const;
    void setCurrentProgress(qreal newCurrentProgress);

    enum class Error
    {
        UploadFailed,
        FirmwareUploadFailed,
        FirmwareUpdateFailed
    };

    qreal firmwareUploadProgress() const;
    void setFirmwareUploadProgress(qreal newFirmwareUploadProgress);

    bool updatingState() const;
    void setUpdatingState(bool newUpdatingState);

signals:

    void currentProgressChanged();

    void errorOccured(Error errorType, QVariant data);
    void firmwareUpdateComplete();

    void firmwareUploadProgressChanged();
    void updatingStateChanged();


public slots:
    void slUpdate(NetEvents eventType, QString target, QVariantList data);
    void slDeviceAvalible();

private:
    qreal m_currentProgress{1.0};
    qreal m_firmwareUploadProgress{1.0};
    bool m_updatingState{false};

    RequestManager* m_requestManager;

    QMap<QString, QPair<qint64, qint64> > m_activeProcesses;
};

#endif // PROGRESS_H
