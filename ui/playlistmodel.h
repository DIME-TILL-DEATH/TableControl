#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>

#include "frames.h"
#include "qtimer.h"
#include "requestactions.h"

class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString curPrintFileName READ curPrintFileName WRITE setCurPrintFileName NOTIFY curPrintFileNameChanged FINAL)
    Q_PROPERTY(bool deviceAvaliable READ deviceAvaliable WRITE setDeviceAvaliable NOTIFY deviceAvaliableChanged FINAL)
public:
    explicit PlaylistModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE void move(int from, int to);
    Q_INVOKABLE void remove(int pos);

    QString curPrintFileName() const;
    void setCurPrintFileName(const QString &newCurPrintFileName);

    float progress() const;
    void setProgress(float newProgress);

    bool deviceAvaliable() const;
    void setDeviceAvaliable(bool newDeviceAvaliable);

signals:
    void sgUpdateData(FrameType frameType, uint8_t dataType, QVariantList data);
    void sgRequest(FrameType frameType, uint8_t requestType,
                   uint32_t data0 = 0,
                   uint32_t data1 = 0,
                   uint32_t parameters = 0);

    void curPrintFileNameChanged();

    void sgRequestFileData(QString fileName);

    void sgProgressChanged();

    void deviceAvaliableChanged();

public slots:
    void slPlaylistDataUpdate(Data::Playlist dataType, QVariantList dataList);
    void slFileDataReady(QString fileName, QList<QVariant> fileData);

    void slDeviceAvaliable();
    void slDeviceUnavaliable();

private:
    QList<QString> m_playlist;
    QMap<QString, QList<QVariant>> m_previewData;

    void refreshModel(QList<QString> playList);
    enum ListRoles{
        FileNameRole = Qt::UserRole + 1,
        PreviewDataRole
    };

    void checkDataUpdate();
    void sendUpdatedPlaylist();

    qint16 m_currentPlsPos{0};
    QString m_curPrintFileName;

    QTimer* updateDataTimer;
    float m_progress;
    bool m_deviceAvaliable;
};

#endif // PLAYLISTMODEL_H
