#ifndef PROGRESSMANAGER_H
#define PROGRESSMANAGER_H

#include <QObject>

#include "netmanager.h"

class ProgressManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal currentProgress READ currentProgress WRITE setCurrentProgress NOTIFY currentProgressChanged FINAL)
public:
    explicit ProgressManager(NetManager* netManager, QObject *parent = nullptr);

    qreal currentProgress() const;
    void setCurrentProgress(qreal newCurrentProgress);

    enum class Error
    {
        UploadFailed
    };

signals:

    void currentProgressChanged();

    void errorOccured(Error errorType, QVariant data);

public slots:
    void slUpdate(NetEvents eventType, QString target, QVariantList data);

private:
    qreal m_currentProgress{1.0};

    QMap<QString, QPair<qint64, qint64> > m_activeProcesses;
};

#endif // PROGRESSMANAGER_H
