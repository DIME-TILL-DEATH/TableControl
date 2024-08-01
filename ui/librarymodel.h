#ifndef LIBRARYMODEL_H
#define LIBRARYMODEL_H

#include <QAbstractListModel>

#include "playlistmodel.h"

#include "answermanager.h"
#include "requestmanager.h"
#include "filemanager.h"

#include "libraryelement.h"

class LibraryModel : public QAbstractListModel
{
    Q_OBJECT

    // Q_PROPERTY(PlaylistModel previewPlaylistModel READ previewPlaylistModel NOTIFY previewPlaylistModelChanged FINAL)
public:
    explicit LibraryModel(AnswerManager* answerManager, RequestManager* requestManager, FileManager* fileManager, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE void setGallery(QString galleryName);

    Q_INVOKABLE void setPreviewPlaylist(int64_t index);

    // PlaylistModel& previewPlaylistModel() {return *m_previewPlaylistModel;};

signals:
    void sgFileDownloadRequest(QString filePath);

    // void previewPlaylistModelChanged();

public slots:
    void slFolderDataUpdated(QString path, QStringList contentList);
    void slFileDataUpdated(QString filePath);
    void slSerialIdUpdated(QString serialId);

private:
    RequestManager* m_requestManager;
    PlaylistModel* m_previewPlaylistModel;

    QList<LibraryElement> m_library;
    QString m_deviceSerialId;

    enum ListRoles
    {
        LibraryElementRole = Qt::UserRole + 1
    };
};

#endif // LIBRARYMODEL_H
