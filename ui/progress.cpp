#include "progress.h"
#include "qforeach.h"

Progress::Progress(AnswerManager *answerManager, RequestManager* requestManager, QObject *parent)
    : QObject{parent},
    m_requestManager(requestManager)
{
    QObject::connect(answerManager, &AnswerManager::sgNetEvent, this, &Progress::slUpdate);
    QObject::connect(requestManager, &RequestManager::sgNetEvent, this, &Progress::slUpdate);
    QObject::connect(requestManager, &RequestManager::sgTableAvaliable, this, &Progress::slDeviceAvalible);
}

void Progress::slUpdate(NetEvents eventType, QString target, QVariantList data)
{
    switch (eventType)
    {
    case NetEvents::StartUploadData:
    {
        qint64 partSize = data.at(0).toInt();
        qint64 fileSize = data.at(1).toInt();

        if(!m_activeProcesses.contains(target))
            m_activeProcesses.insert(target, {partSize, fileSize});
        break;
    }
    case NetEvents::UploadDataCompleted:
    {
        if(!m_activeProcesses.contains(target))
        {
            qDebug() << __FUNCTION__ << "Some error, complete action without request! Target" << target;
        }
        qint64 targetProgress = data.at(0).toInt();
        qint64 fileSize = m_activeProcesses.value(target).second;

        m_activeProcesses.insert(target, {targetProgress, fileSize});
        break;
    }
    case NetEvents::UploadDataError:
    {
        qDebug() << "Failed to upload file " << target;
        m_activeProcesses.remove(target);
        emit errorOccured(Error::UploadFailed, target);
        break;
    }
    case NetEvents::UploadFirmwareStart:
    {
        qint64 firmwareUploadedBytes = data.at(0).toInt();
        qint64 fileSize = data.at(1).toInt();

        if(!m_activeProcesses.contains(target))
            m_activeProcesses.insert(target, {firmwareUploadedBytes, fileSize});

        setFirmwareUploadProgress((qreal)firmwareUploadedBytes/(qreal)fileSize);
        break;
    }
    case NetEvents::UploadFirmware:
    {
        qint32 firmwareUploadedBytes = data.at(0).toInt();
        qint32 fileSize = m_activeProcesses.value(target).second; //data.at(1).toInt();

        if(fileSize>0) setFirmwareUploadProgress((qreal)firmwareUploadedBytes/(qreal)fileSize);

        qDebug() << "Upload, processed: " << firmwareUploadedBytes << " file size: " << fileSize;

        if(firmwareUploadedBytes >= fileSize)
        {
            qDebug() << "Firmware uploaded, update";
            setUpdatingState(true);

            m_activeProcesses.remove(target);
            m_requestManager->requestParameter(Requests::Firmware::FIRMWARE_UPDATE);
        }
        break;
    }
    case NetEvents::UploadFirmwareError:
    {
        qDebug() << "Failed to upload file " << target;
        setFirmwareUploadProgress(1.0);
        emit errorOccured(Error::FirmwareUploadFailed, target);
        break;
    }

    case NetEvents::UpdatingFirmwareFinished:
    {
        setUpdatingState(false);
        emit firmwareUpdateComplete();
        break;
    }

    case NetEvents::UpdatingFirmwareError:
    {
        qDebug() << "Update firmware failed " << target;
        setFirmwareUploadProgress(1.0);
        emit errorOccured(Error::FirmwareUpdateFailed, target);
        break;
    }
    default:
        break;
    }


    qreal overallTask = 0;
    qreal overallProgress = 0;
    foreach(QString key, m_activeProcesses.keys())
    {
        qint64 currentProgress = m_activeProcesses.value(key).first;
        qint64 currentTask = m_activeProcesses.value(key).second;
        if(currentProgress >= currentTask)
        {
            m_activeProcesses.remove(key);
        }
        else
        {
            overallProgress += currentProgress;
            overallTask += currentTask;
        }
    }

    if(m_activeProcesses.size() == 0)
    {
        setCurrentProgress(1.0);
        return;
    }

    if(overallTask == 0)
    {
        setCurrentProgress(1.0);
    }
    else
    {
        setCurrentProgress(overallProgress/overallTask);
    }
}

void Progress::slDeviceAvalible()
{
    setFirmwareUploadProgress(1.0);
    setUpdatingState(false);
}

qreal Progress::currentProgress() const
{
    return m_currentProgress;
}

void Progress::setCurrentProgress(qreal newCurrentProgress)
{
    if (m_currentProgress == newCurrentProgress)
        return;
    m_currentProgress = newCurrentProgress;
    emit currentProgressChanged();
}

qreal Progress::firmwareUploadProgress() const
{
    return m_firmwareUploadProgress;
}

void Progress::setFirmwareUploadProgress(qreal newFirmwareUploadProgress)
{
    if (qFuzzyCompare(m_firmwareUploadProgress, newFirmwareUploadProgress))
        return;
    m_firmwareUploadProgress = newFirmwareUploadProgress;
    emit firmwareUploadProgressChanged();
}

bool Progress::updatingState() const
{
    return m_updatingState;
}

void Progress::setUpdatingState(bool newUpdatingState)
{
    m_updatingState = newUpdatingState;
    emit updatingStateChanged();
}
