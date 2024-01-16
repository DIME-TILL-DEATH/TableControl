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
    Q_PROPERTY(qint16 currentPlsPos READ currentPlsPos WRITE setCurrentPlsPos NOTIFY currentPlsPosChanged FINAL)
    Q_PROPERTY(QString curPrintFileName READ curPrintFileName WRITE setCurPrintFileName NOTIFY curPrintFileNameChanged FINAL)
public:
    explicit PlaylistModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE void move(int from, int to);

    quint16 currentPlsPos() const;
    void setCurrentPlsPos(quint16 newCurrentPlsPos);

    QString curPrintFileName() const;
    void setCurPrintFileName(const QString &newCurPrintFileName);

signals:
    void sgUpdateData(FrameType frameType, uint8_t dataType, QVariantList data);
    void sgRequest(FrameType frameType, uint8_t requestType,
                   uint32_t data0 = 0,
                   uint32_t data1 = 0,
                   uint32_t parameters = 0);


    void currentPlsPosChanged();
    void curPrintFileNameChanged();

    void sgRequestFileData(QString fileName);

public slots:
    void slPlaylistDataUpdate(Data::Playlist dataType, QVariantList dataList);
    void slFileDataReady(QString fileName, QList<QVariant> fileData);

private:
    QList<QString> m_playlist;
    QMap<QString, QList<QVariant>> m_previewData;

    void refreshModel(QList<QString> playList);
    enum ListRoles{
        FileNameRole = Qt::UserRole + 1,
        PreviewDataRole
    };

    void checkDataUpdate();

    qint16 m_currentPlsPos{0};
    QString m_curPrintFileName;

    QTimer* updateDataTimer;
};

#endif // PLAYLISTMODEL_H
