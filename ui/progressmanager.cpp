#include "progressmanager.h"
#include "qforeach.h"

ProgressManager::ProgressManager(NetManager *netManager, QObject *parent)
    : QObject{parent}
{
    QObject::connect(netManager, &NetManager::sgNetEvent, this, &ProgressManager::slUpdate);
}

qreal ProgressManager::currentProgress() const
{
    return m_currentProgress;
}

void ProgressManager::setCurrentProgress(qreal newCurrentProgress)
{
    if (m_currentProgress == newCurrentProgress)
        return;
    m_currentProgress = newCurrentProgress;
    emit currentProgressChanged();
}

void ProgressManager::slUpdate(NetEvents eventType, QString target, QVariantList data)
{
    switch (eventType)
    {
    case NetEvents::UploadData:
    {
        qint64 partSize = data.at(0).toInt();
        qint64 fileSize = data.at(1).toInt();

        if(!m_activeProcesses.contains(target))
            m_activeProcesses.insert(target, {partSize, fileSize});
        break;
    }
    case NetEvents::UploadDataCompleted:
    {
        qint64 uploadedPartSize = data.at(0).toInt();

        if(!m_activeProcesses.contains(target))
        {
            qDebug() << __FUNCTION__ << "Some error, complete action without request! Target" << target;
        }
        qint64 fileSize = m_activeProcesses.value(target).second;
        qint64 targetProgress = m_activeProcesses.value(target).first + uploadedPartSize;
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
