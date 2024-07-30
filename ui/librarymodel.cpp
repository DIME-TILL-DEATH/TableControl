#include "QDir"

#include "librarymodel.h"

#include "devicecontentmodel.h"

LibraryModel::LibraryModel(AnswerManager* answerManager, RequestManager* requestManager, QObject *parent)
    : QAbstractListModel{parent}
{
    m_requestManager = requestManager;

    connect(answerManager, &AnswerManager::sgFolderContent, this, &LibraryModel::slFolderDataUpdated);
    connect(answerManager, &AnswerManager::sgSerialId, this, &LibraryModel::slSerialIdUpdated);
}

int LibraryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_library.size();
}

QVariant LibraryModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() > rowCount(index))
    {
        return {-1};
    }

    switch(role)
    {
        case ListRoles::LibraryElementRole:
        {
            return  QVariant::fromValue(m_library.at(index.row()));
        }

        default:
        {
            qWarning() << __FUNCTION__ << "no role";
            return true;
        }
    }
}

QHash<int, QByteArray> LibraryModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ListRoles::LibraryElementRole] = "libraryElement";
    return roles;
}

void LibraryModel::setGallery(QString contentElementName)
{
    contentElementName.remove(m_deviceSerialId + "/");
    m_requestManager->setParameter(Requests::Playlist::SET_CURRENT_GALLERY, contentElementName);

    m_requestManager->requestParameter(Requests::Playlist::REQUEST_PLAYLIST);
    m_requestManager->requestParameter(Requests::Playlist::REQUEST_PLAYLIST_POSITION);
    m_requestManager->requestParameter(Requests::Hardware::REQUEST_PROGRESS);
}

void LibraryModel::slFolderDataUpdated(QString path, QStringList contentList)
{
    path.remove(DeviceContentModel::librarySdcardPath);

    QString dataPath;
#ifdef Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + '/';
#endif

    if(path.size() == 0)
    {
        beginResetModel();
        m_library.clear();

        foreach (QString contentNameVariant, contentList)
        {
            QString contentName = contentNameVariant;
            if(contentName.indexOf("DIR|") == 0)
            {
                contentName.remove(0, 4);
                QString elementPath(m_deviceSerialId + "/" + contentName);

                QDir elementDir(dataPath + "/preview/" + elementPath);
                if(!elementDir.exists())
                {
                    elementDir.mkpath(elementDir.absolutePath());
                }

                LibraryElement newElement(elementPath);

                if(!newElement.loadInfo())
                {
                    emit sgFileDownloadRequest(contentName + "/info.ini");
                }

                if(!newElement.loadCover())
                {
                    emit sgFileDownloadRequest(contentName + "/cover.jpg");
                }

                if(!newElement.loadPlaylist())
                {
                    emit sgFileDownloadRequest(contentName + "/playlist.pls");
                }

                m_library.append(newElement);
            }
        }
        endResetModel();
    }
}

void LibraryModel::slFileDataUpdated(QString filePath)
{
    qDebug() << "Ready to update" << filePath;
    filePath.prepend(m_deviceSerialId + "/");

    if(filePath.contains("info.ini"))
    {
        filePath.remove("/info.ini");
        for(quint16 pos=0; pos < m_library.size(); pos++)
        {
            if(m_library.at(pos).systemName().indexOf(filePath) != -1)
            {
                m_library[pos].loadInfo();
                emit dataChanged(createIndex(pos, 0), createIndex(pos, 0));
            }
        }
    }

    if(filePath.contains("cover.jpg"))
    {
        filePath.remove("/cover.jpg");
        for(quint16 pos=0; pos < m_library.size(); pos++)
        {
            if(m_library.at(pos).systemName().indexOf(filePath) != -1)
            {
                m_library[pos].loadCover();
                emit dataChanged(createIndex(pos, 0), createIndex(pos, 0));
            }
        }
    }

    if(filePath.contains("playlist.pls"))
    {
        filePath.remove("/playlist.pls");
        for(quint16 pos=0; pos < m_library.size(); pos++)
        {
            if(m_library.at(pos).systemName().indexOf(filePath) != -1)
            {
                m_library[pos].loadPlaylist();
                emit dataChanged(createIndex(pos, 0), createIndex(pos, 0));
            }
        }
    }
}

void LibraryModel::slSerialIdUpdated(QString serialId)
{
    m_deviceSerialId = serialId;
}
