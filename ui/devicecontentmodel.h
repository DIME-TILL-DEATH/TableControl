#ifndef DEVICECONTENTMODEL_H
#define DEVICECONTENTMODEL_H

#include <QAbstractListModel>
#include <QObject>

#include "netmanager.h"

#include "contentnode.h"

#include "frames.h"
#include "requestactions.h"

class DeviceContentModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit DeviceContentModel(NetManager *netManager, QObject *parent = nullptr);
    ~DeviceContentModel();

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QHash<int, QByteArray> roleNames() const override;

    constexpr static const char librarySdcardPath[] = "/sdcard/library/";

    Q_INVOKABLE void uploadFileToDevice(QString dstPath, QString srcPath);

signals:
    void sgUpdateData(FrameType frameType, uint8_t dataType, QVariantList data);
    void sgRequest(FrameType frameType, uint8_t requestType,
                   uint32_t data0 = 0,
                   uint32_t data1 = 0,
                   uint32_t parameters = 0);

    void sgRequestFileData(QString fileName);

public slots:
    void slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList dataList);

    void slFileDataReady(QString fileName, QList<QVariant> fileData);

    void slDeviceAvaliable();

private:
    ContentNode* rootNode;
    ContentNode* sdCardNode;

    void appendNode(ContentNode* destNode, ContentNode* newNode);
};

#endif // DEVICECONTENTMODEL_H
