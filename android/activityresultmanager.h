#ifndef ACTIVITYRESULTMANAGER_H
#define ACTIVITYRESULTMANAGER_H

#include <qglobal.h>

#ifdef Q_OS_ANDROID

#include <QtCore/private/qandroidextras_p.h>

enum class ActivityType
{
    PICK_FILE
};

class ActivityResultManager : public QObject, public QAndroidActivityResultReceiver
{
    Q_OBJECT
public:
    ActivityResultManager();
    void handleActivityResult(int receiverRequestCode, int resultCode, const QJniObject &data) override;

    // static QString getFileNameFromUri(QString uri);
    static void takeReadUriPermission(QJniObject uriObject);
    static void takeWriteUriPermission(QJniObject uriObject);
private:
    QString m_fileName;
    QString m_filePath;

    void processUri(QJniObject uriObject);

signals:
    void sgFilePicked(QString fullFilePath, QString fileName);
};

#endif // ACTIVITYRESULTMANAGER_H
#endif
