#include <QPointF>

#include "playlistmodel.h"

PlaylistModel::PlaylistModel(NetManager *netManager, QObject *parent)
    : QAbstractListModel{parent}
{
    QObject::connect(netManager, &NetManager::sgPlaylistDataUpdated, this, &PlaylistModel::slPlaylistDataUpdate);
    QObject::connect(netManager, &NetManager::sgDeviceConnected, this, &PlaylistModel::slDeviceAvaliable);
    QObject::connect(netManager, &NetManager::sgDeviceDisconnected, this, &PlaylistModel::slDeviceUnavaliable);

    QObject::connect(this, &PlaylistModel::sgRequest, netManager, &NetManager::sendRequest);
    QObject::connect(this, &PlaylistModel::sgUpdateData, netManager, &NetManager::slUpdateData);

    updateDataTimer = new QTimer(this);
    updateDataTimer->setInterval(1000);
    m_deviceAvaliable = false;

    connect(updateDataTimer, &QTimer::timeout, this, &PlaylistModel::checkDataUpdate);
}

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_playlist.size();
}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ListRoles::FileNameRole] = "fileName";
    roles[ListRoles::FilePathRole] = "filePath";
    roles[ListRoles::PreviewDataRole] = "previewData";
    roles[ListRoles::FileAvaliableRole] = "isFileAvaliable";
    return roles;
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() > rowCount(index))
    {
        return {-1};
    }

    switch(role)
    {
    case ListRoles::FileNameRole:
    {
        QString fullPath = m_playlist.at(index.row());
        QStringList pathElements = fullPath.split("/", Qt::SkipEmptyParts);
        return pathElements.last();
//        return QVariant::fromValue(m_playlist.at(index.row()));
    }

    case ListRoles::FilePathRole:
    {
        QString fullPath = m_playlist.at(index.row());
        QStringList pathElements = fullPath.split("/", Qt::SkipEmptyParts);
        pathElements.pop_back();
        QString path = pathElements.join("/");
        return path;
    }

    case ListRoles::PreviewDataRole:
    {
        QString fileName = m_playlist.at(index.row());
        if(!m_previewData.contains(fileName))
        {
            qDebug() << "Model. Requesting file data: " << fileName;
            emit sgRequestFileData(fileName);
            return QVariantList();
        }
        else
        {
            if(m_previewData.value(fileName).at(0).toString() == "FILE NOT FOUND") return QVariantList();

            return QVariant(m_previewData.value(fileName));
        }
    }

    case ListRoles::FileAvaliableRole:
    {
        QString fileName = m_playlist.at(index.row());
        if(!m_previewData.contains(fileName))
        {
            emit sgRequestFileData(fileName);
            return 1;
        }

        return !(m_previewData.value(fileName).at(0).toString() == "FILE NOT FOUND");
    }

    default:
    {
        qWarning() << __FUNCTION__ << "no role";
        return true;
    }
    }
}

void PlaylistModel::slDeviceAvaliable()
{
    emit sgRequest(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::REQUEST_PLAYLIST);
    emit sgRequest(FrameType::TRANSPORT_ACTIONS, (uint8_t)Requests::Transport::REQUEST_PROGRESS);
    emit sgRequest(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::REQUEST_PLAYLIST_POSITION);

    setDeviceAvaliable(true);
    updateDataTimer->start();
}

void PlaylistModel::slDeviceUnavaliable()
{
    updateDataTimer->stop();

    if(m_playlist.size()>0)
    {
        beginRemoveRows(QModelIndex(), 0, m_playlist.size()-1);
        m_playlist.clear();
        setCurPlaylistPosition(-1);
        endRemoveRows();
    }

    setDeviceAvaliable(false);
    qDebug() << "Device unavaliable";
}

void PlaylistModel::slPlaylistDataUpdate(Data::Playlist dataType, QVariantList dataList)
{
    switch(dataType)
    {
    case Data::Playlist::PLAYLIST:
    {
        QList<QString> newPlaylist;
        foreach (QVariant varName, dataList)
        {
            newPlaylist.append(varName.toString());
        }
        refreshModel(newPlaylist);
        break;
    }
    case Data::Playlist::PLAYLIST_POSITION:
    {
        qint16 plsPos = dataList.at(0).toInt();
        setCurPlaylistPosition(plsPos);
        break;
    }
    }
}

void PlaylistModel::slFileDataReady(QString fileName, QList<QVariant> fileData)
{
    //qDebug() << "Updating file data: " << fileName;
    if(m_previewData.value(fileName) != fileData)
    {
        m_previewData.insert(fileName, fileData);
        for(int pos=0; pos<m_playlist.size();pos++)
        {
            if(m_playlist.at(pos) == fileName)
            {
                emit dataChanged(createIndex(pos, 0), createIndex(pos, 0));
            }
        }
    }
}

void PlaylistModel::refreshModel(QList<QString> newPlayList)
{
    for(int pos=0; pos < newPlayList.size(); pos++)
    {
        QString newFileName = newPlayList.at(pos);

        if(pos < m_playlist.size())
        {
            if(m_playlist.at(pos) != newFileName)
            {
                m_playlist.replace(pos, newFileName);
                if(!m_previewData.contains(newFileName))
                {
                    emit sgRequestFileData(newFileName);
                }
                emit dataChanged(createIndex(pos, 0), createIndex(pos, 0));
            }
        }
        else
        {
            beginInsertRows(QModelIndex(), pos, pos);
            m_playlist.append(newFileName);
            if(!m_previewData.contains(newFileName))
            {
                emit sgRequestFileData(newFileName);
            }
            endInsertRows();
        }
    }

    if(newPlayList.size() < m_playlist.size())
    {
        int rowsToDelete = m_playlist.size() - newPlayList.size();
        beginRemoveRows(QModelIndex(), m_playlist.size(), m_playlist.size()+rowsToDelete);
        m_playlist.remove(m_playlist.size(), rowsToDelete);
        endRemoveRows();
    }
    //setCurPrintFileName(m_playlist.at(m_currentPlsPos));
}

void PlaylistModel::move(int from, int to)
{
    if(from >= 0 && from < rowCount() && to >= 0 && to < rowCount() && from != to)
    {
        if(from == to - 1)
        { // Allow item moving to the bottom
            to = from++;
        }

        m_playlist.move(from, to);
        emit dataChanged(createIndex(from, 0), createIndex(to, 0));

        sendUpdatedPlaylist();
    }
}


void PlaylistModel::insert(int pos, QString fileName)
{
    beginInsertRows(QModelIndex(), pos, pos);
    m_playlist.insert(pos, fileName);
    endInsertRows();

    sendUpdatedPlaylist();
}

void PlaylistModel::remove(int pos)
{
    beginRemoveRows(QModelIndex(), pos, pos);
    m_playlist.removeAt(pos);
    endRemoveRows();

    sendUpdatedPlaylist();
}

void PlaylistModel::checkDataUpdate()
{
    emit sgRequest(FrameType::TRANSPORT_ACTIONS, (uint8_t)Requests::Transport::REQUEST_PROGRESS);
    emit sgRequest(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::REQUEST_PLAYLIST_POSITION);
}

void PlaylistModel::sendUpdatedPlaylist()
{
    QVariantList resultData;
    foreach (QString name, m_playlist)
    {
        resultData.append(QVariant(name));
    }

    emit sgUpdateData(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::CHANGE_PLAYLIST, resultData);
    resultData.clear();
    resultData.append(m_curPlaylistPosition);
    emit sgUpdateData(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::CHANGE_PLAYLIST_POSITION, resultData);
    qDebug() << "Updating playlist";
}

bool PlaylistModel::deviceAvaliable() const
{
    return m_deviceAvaliable;
}

void PlaylistModel::setDeviceAvaliable(bool newDeviceAvaliable)
{
    m_deviceAvaliable = newDeviceAvaliable;
    emit deviceAvaliableChanged();
}

qint32 PlaylistModel::curPlaylistPosition() const
{
    return m_curPlaylistPosition;
}

void PlaylistModel::setCurPlaylistPosition(qint32 newCurPlaylistPosition)
{
    m_curPlaylistPosition = newCurPlaylistPosition;
    emit curPlaylistPositionChanged();
}
