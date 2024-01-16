#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QVariant>
#include <QPointF>

#include "frames.h"
#include "requestactions.h"

class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(QObject *parent = nullptr);

    static bool getPointsFromFile(QString fileName, QList<QVariant>& result);
    static void savePreviewFile(QString filePath, const QByteArray& fileData);

signals:
    void sgUpdateData(FrameType frameType, uint8_t dataType, QVariantList data);

    void sgFileDataReady(QString fileName, QList<QVariant> fileData);

public slots:
    void processFileLoadRequest(QString fileName);
    void processDownloadedFile(Data::File dataType, QVariantList dataList);

private:
};

#endif // FILEMANAGER_H
