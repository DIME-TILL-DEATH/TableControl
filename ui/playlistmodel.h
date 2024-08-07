#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>

#include "playlistelement.h"

#include "requestmanager.h"
#include "answermanager.h"
#include "filemanager.h"


class PlaylistModel : public QAbstractListModel
{
    Q_OBJECT

    QML_ELEMENT
    QML_UNCREATABLE("LibraryElement is for C++ instantiation only")

    Q_PROPERTY(QString printName READ printName NOTIFY printNameChanged FINAL)
    Q_PROPERTY(QString galleryName READ galleryName NOTIFY galleryNameChanged FINAL)
public:
    explicit PlaylistModel(AnswerManager* answerManager, RequestManager* requestManager, FileManager* fileManager, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE void changePrint(int plsPos);

    Q_INVOKABLE void insert(int pos, QString fileName);
    Q_INVOKABLE void move(int from, int to);
    Q_INVOKABLE void remove(int pos);

    float progress() const;
    void setProgress(float newProgress);

    qint32 curPlaylistPosition() const;

    QString printName() const;

    QString galleryName() const;
    void setGalleryName(const QString& galleryName);

signals:
    void sgRequestFileData(QString fileName);

    void sgProgressChanged();
    void printNameChanged();
    void galleryNameChanged();

public slots:
    void slPlaylistUpdated(QStringList newPlaylist);
    void slPlaylistPositionUpdated(qint32 newPlsPos);

    void slFileDataReady(QString fileName, QList<QVariant> fileData);
    void slDeviceUnavaliable();

private:
    RequestManager* m_requestManager;

    QString m_galleryName;

    QList<PlaylistElement> m_playlist;
    QMap<QString, QList<QVariant>> m_previewData;

    void requestFileData(const QString& dataPath);
    void refreshModel(QList<QString> playList);
    enum ListRoles{
        FileNameRole = Qt::UserRole + 1,
        FilePathRole,
        PreviewDataRole,
        FileAvaliableRole,
        PlaylistElementRole
    };

    void checkDataUpdate();
    void sendUpdatedPlaylist();

    qint32 curPlaylistPosition();

    float m_progress;
};

#endif // PLAYLISTMODEL_H
