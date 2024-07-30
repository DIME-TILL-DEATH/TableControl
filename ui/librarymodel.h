#ifndef LIBRARYMODEL_H
#define LIBRARYMODEL_H

#include <QAbstractListModel>


#include "answermanager.h"
#include "requestmanager.h"

#include "libraryelement.h"

class LibraryModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit LibraryModel(AnswerManager* answerManager, RequestManager* requestManager, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE void setGallery(QString galleryName);

signals:
    void sgFileDownloadRequest(QString filePath);

public slots:
    void slFolderDataUpdated(QString path, QStringList contentList);
    void slFileDataUpdated(QString filePath);
    void slSerialIdUpdated(QString serialId);

private:
    RequestManager* m_requestManager;

    QList<LibraryElement> m_library;
    QString m_deviceSerialId;

    enum ListRoles
    {
        LibraryElementRole = Qt::UserRole + 1
    };
};

#endif // LIBRARYMODEL_H
