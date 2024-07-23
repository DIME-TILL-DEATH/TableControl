#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QVariant>
#include <QPointF>


#include "answermanager.h"
#include "requestmanager.h"

// TODO: save data only here. Send others itterotros or pointers
typedef QMapIterator<QString, QList<QVariant> > PreviewFileData;

class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(AnswerManager *answerManager, RequestManager* requestManager, QObject *parent = nullptr);

    bool getPointsFromFile(QString fileName, QList<QVariant>& result);
    void saveFilePart(QString filePath, const QByteArray& fileData, int32_t partPosition = -1, int32_t fileSize = -1);
    void requesteFileUnavaliable(QString filePath);

signals:
    void sgFileDataReady(QString fileName, QList<QVariant> fileData);

public slots:
    void processFileLoadRequest(QString fileName);

private:
    RequestManager* m_requestManager;

    QMap<QString, QList<QVariant> > m_loadedData;

    QString deviceSerialId;
};

#endif // FILEMANAGER_H
