#include <QPointF>

#include "playlistmodel.h"

PlaylistModel::PlaylistModel(AnswerManager* answerManager, RequestManager *requestManager, QObject *parent)
    : QAbstractListModel{parent}
{
    m_requestManager = requestManager;

    connect(answerManager, &AnswerManager::sgPlaylist, this, &PlaylistModel::slPlaylistUpdated);
    connect(answerManager, &AnswerManager::sgPlaylistPosition, this, &PlaylistModel::slPlaylistPositionUpdated);

    connect(answerManager, &AnswerManager::sgCurrentGalleryName, this, &PlaylistModel::setGalleryName);

    connect(requestManager, &RequestManager::sgTableUnavaliable, this, &PlaylistModel::slDeviceUnavaliable);
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_playlist.size();
}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ListRoles::PreviewDataRole] = "previewData";
    roles[ListRoles::FileAvaliableRole] = "isFileAvaliable";
    roles[ListRoles::PlaylistElementRole] = "playlistElement";
    return roles;
}

void PlaylistModel::changePrint(int plsPos)
{
    if(plsPos != curPlaylistPosition())
    {
        m_requestManager->setParameter(Requests::Playlist::CHANGE_PRINTNG_FILE, plsPos);
    }
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() > rowCount(index))
    {
        return {-1};
    }

    QString fullFileName = m_galleryName + "/" + m_playlist.at(index.row()).fullFilePath();

    switch(role)
    {
    case ListRoles::PreviewDataRole:
    {
        if(!m_previewData.contains(fullFileName))
        {
            qDebug() << "Model. Requesting file data: " << fullFileName;
            emit sgRequestFileData(fullFileName);
            return QVariantList();
        }
        else
        {
            if(m_previewData.value(fullFileName).size() == 0) return QVariantList();

            if(m_previewData.value(fullFileName).at(0).toString() == "FILE NOT FOUND") return QVariantList();

            return QVariant(m_previewData.value(fullFileName));
        }
    }

    case ListRoles::FileAvaliableRole:
    {
        if(!m_previewData.contains(fullFileName))
        {
            emit sgRequestFileData(fullFileName);
            return 1;
        }

        if(m_previewData.value(fullFileName).size() == 0)
        {
            qDebug() << "File found, but empty data";
            return 1;
        }

        return !(m_previewData.value(fullFileName).at(0).toString() == "FILE NOT FOUND");
    }

    case ListRoles::PlaylistElementRole:
    {
        return  QVariant::fromValue(m_playlist.at(index.row()));
    }

    default:
    {
        qWarning() << __FUNCTION__ << "no role";
        return true;
    }
    }
}

void PlaylistModel::slDeviceUnavaliable()
{
    if(m_playlist.size()>0)
    {
        beginRemoveRows(QModelIndex(), 0, m_playlist.size()-1);
        m_playlist.clear();
        endRemoveRows();
    }
}

void PlaylistModel::slPlaylistUpdated(QStringList newPlaylist)
{
    if(curPlaylistPosition() > newPlaylist.size()) slPlaylistPositionUpdated(newPlaylist.size());

    refreshModel(newPlaylist);
}

void PlaylistModel::slPlaylistPositionUpdated(qint32 newPlsPos)
{
    if(newPlsPos == curPlaylistPosition()) return;

    if(newPlsPos > m_playlist.size())
    {
        qWarning() << "Recieved playlist position more than elements in playlist, pos: " << newPlsPos << ", size: " <<  m_playlist.size();
        return;
    }

    if(newPlsPos == -1)
    {
        qWarning() << "Playlist is empty";
        return;
    }

    for(auto it = m_playlist.begin(); it != m_playlist.end(); ++it)
    {
        (*it).setIsCurrentPrintingElement(false);
    }
    (m_playlist.begin() + newPlsPos)->setIsCurrentPrintingElement(true);

    emit dataChanged(createIndex(0, 0), createIndex(m_playlist.size()-1, 0));
    emit printNameChanged();
}

void PlaylistModel::slFileDataReady(QString fileName, QList<QVariant> fileData)
{
    if(m_previewData.value(fileName) != fileData)
    {
        m_previewData.insert(fileName, fileData);
        for(int pos=0; pos<m_playlist.size(); pos++)
        {
            if((m_galleryName + "/" + m_playlist.at(pos).fullFilePath()) == fileName)
            {
                emit dataChanged(createIndex(pos, 0), createIndex(pos, 0));
            }
        }
    }
}

void PlaylistModel::refreshModel(QList<QString> newPlayList)
{
    //qDebug() << "Refreshing playlist model";

    for(int pos=0; pos < newPlayList.size(); pos++)
    {
        QString newFilePath = newPlayList.at(pos);

        if(pos < m_playlist.size())
        {
            if(m_playlist.at(pos).fullFilePath() != newFilePath)
            {
                m_playlist.replace(pos, PlaylistElement(newFilePath));
                if(!m_previewData.contains(m_galleryName + "/" + newFilePath))
                {
                   emit sgRequestFileData(m_galleryName + "/" + newFilePath);
                }
                emit dataChanged(createIndex(pos, 0), createIndex(pos, 0));
            }
        }
        else
        {
            beginInsertRows(QModelIndex(), pos, pos);
            m_playlist.append(PlaylistElement(newFilePath));
            if(!m_previewData.contains(m_galleryName + "/" + newFilePath))
            {
               emit sgRequestFileData(m_galleryName + "/" + newFilePath);
            }
            endInsertRows();
        }

        // if(!m_previewData.contains(m_galleryName + "/" + newFilePath))
        // {
        //     emit sgRequestFileData(m_galleryName + "/" + newFilePath);
        // }
    }

    if(newPlayList.size() < m_playlist.size())
    {
        int rowsToDelete = m_playlist.size() - newPlayList.size();
        beginRemoveRows(QModelIndex(), m_playlist.size(), m_playlist.size()+rowsToDelete);
        m_playlist.remove(m_playlist.size(), rowsToDelete);
        endRemoveRows();
    }
}

void PlaylistModel::move(int from, int to)
{
    if(from >= 0 && from < rowCount() && to >= 0 && to < rowCount() && from != to)
    {
        if(from == to - 1)
        { // Allow item moving to the bottom
            to = from++;
        }

        int dest = to;
        if(to > from) dest++;

        beginMoveRows(QModelIndex(), from, from, QModelIndex(), dest);
        m_playlist.move(from, to);
        endMoveRows();

        sendUpdatedPlaylist();
    }
}


void PlaylistModel::insert(int pos, QString fileName)
{
    beginInsertRows(QModelIndex(), pos, pos);
    m_playlist.insert(pos, PlaylistElement(fileName));
    endInsertRows();

    sendUpdatedPlaylist();
}

void PlaylistModel::remove(int pos)
{
    if(pos == curPlaylistPosition())
    {
        // TODO оповестить пользователя
        qDebug() << "Can't remove currently printing file";
        return;
    }

    beginRemoveRows(QModelIndex(), pos, pos);
    m_playlist.removeAt(pos);
    endRemoveRows();

    sendUpdatedPlaylist();
}

void PlaylistModel::sendUpdatedPlaylist()
{
    QString resultDataList;
    foreach (PlaylistElement item, m_playlist)
    {
        resultDataList.append(item.fullFilePath());
        resultDataList.append("\r\n");
    }
    m_requestManager->setParameter(Requests::Playlist::CHANGE_PLAYLIST, resultDataList);
    m_requestManager->setParameter(Requests::Playlist::CHANGE_PLAYLIST_POSITION, curPlaylistPosition());
}

qint32 PlaylistModel::curPlaylistPosition()
{
    auto playlistElement = std::find_if(m_playlist.begin(), m_playlist.end(), [](PlaylistElement element) {return element.isCurrentPrintingElement();});
    return std::distance(m_playlist.begin(), playlistElement);
}

QString PlaylistModel::printName() const
{
    auto playlistElement = std::find_if(m_playlist.begin(), m_playlist.end(), [](PlaylistElement element) {return element.isCurrentPrintingElement();});
    if(playlistElement != nullptr) return playlistElement->fileName();
    else return "--";
}

QString PlaylistModel::galleryName() const
{
    if(m_galleryName == "") return "default";

    return m_galleryName;
}

void PlaylistModel::setGalleryName(const QString &galleryName)
{
    if(galleryName == m_galleryName) return;

    m_galleryName = galleryName;
    emit galleryNameChanged();

    beginResetModel();
    m_playlist.clear();
    m_previewData.clear();
    endResetModel();
}
