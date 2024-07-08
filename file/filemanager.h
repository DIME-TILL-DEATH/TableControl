#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QVariant>
#include <QPointF>

#include "requestactions.h"

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
    void savePreviewFile(QString filePath, const QByteArray& fileData);

signals:
    void sgUpdateData(FrameType frameType, uint8_t dataType, QVariantList data);

    void sgFileDataReady(QString fileName, QList<QVariant> fileData);

public slots:
    void processFileLoadRequest(QString fileName);

    void slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList dataList);

private:
    QMap<QString, QList<QVariant> > m_loadedData;

    QString deviceSerialId;
};

#endif // FILEMANAGER_H
