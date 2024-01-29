#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QVariant>
#include <QPointF>

#include "requestactions.h"

#include "netmanager.h"

// TODO: save data only here. Send others itterotros or pointers
typedef QMapIterator<QString, QList<QVariant> > PreviewFileData;

class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(NetManager* netManager, QObject *parent = nullptr);

    static bool getPointsFromFile(QString fileName, QList<QVariant>& result);
    static void savePreviewFile(QString filePath, const QByteArray& fileData);

signals:
    void sgUpdateData(FrameType frameType, uint8_t dataType, QVariantList data);

    void sgFileDataReady(QString fileName, QList<QVariant> fileData);

public slots:
    void processFileLoadRequest(QString fileName);

    void slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList dataList);

private:
    QMap<QString, QList<QVariant> > m_loadedData;
};

#endif // FILEMANAGER_H
