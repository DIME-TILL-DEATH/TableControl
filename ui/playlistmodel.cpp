#include <QPointF>

#include "playlistmodel.h"

PlaylistModel::PlaylistModel(NetManager *netManager, QObject *parent)
    : QAbstractListModel{parent}
{
    QObject::connect(netManager, &NetManager::sgDataUpdated, this, &PlaylistModel::slDataUpdated);
    QObject::connect(netManager, &NetManager::sgDeviceConnected, this, &PlaylistModel::slDeviceAvaliable);
    QObject::connect(netManager, &NetManager::sgDeviceDisconnected, this, &PlaylistModel::slDeviceUnavaliable);

    QObject::connect(this, &PlaylistModel::sgRequest, netManager, &NetManager::sendRequest);
    QObject::connect(this, &PlaylistModel::sgUpdateData, netManager, &NetManager::slUpdateData);

    updateDataTimer = new QTimer(this);
    updateDataTimer->setInterval(500);
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
    roles[ListRoles::PreviewDataRole] = "previewData";
    roles[ListRoles::FileAvaliableRole] = "isFileAvaliable";
    roles[ListRoles::PlaylistElementRole] = "playlistElement";
    return roles;
}

void PlaylistModel::changePrint(int plsPos)
{
    if(plsPos != curPlaylistPosition())
    {
        emit sgRequest(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::CHANGE_PRINTNG_FILE, plsPos);
        qDebug() << "Changing printing file to position: " << plsPos;
    }
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() > rowCount(index))
    {
        return {-1};
    }

    QString fileName = m_playlist.at(index.row()).fullFilePath();

    switch(role)
    {
    case ListRoles::PreviewDataRole:
    {
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
        if(!m_previewData.contains(fileName))
        {
            emit sgRequestFileData(fileName);
            return 1;
        }

        return !(m_previewData.value(fileName).at(0).toString() == "FILE NOT FOUND");
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

void PlaylistModel::slDeviceAvaliable()
{
    emit sgRequest(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::REQUEST_PLAYLIST);
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::REQUEST_PROGRESS);
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
        endRemoveRows();
    }

    setDeviceAvaliable(false);
    qDebug() << "Device unavaliable";
}


void PlaylistModel::slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList dataList)
{
    if(frameType != FrameType::PLAYLIST_ACTIONS) return;

    switch((Data::Playlist)dataType)
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

        if(plsPos == curPlaylistPosition()) return;

        if(plsPos > m_playlist.size())
        {
            qWarning() << "Recieved playlist position more than elements in playlist";
            return;
        }

        if(plsPos == -1)
        {
            qWarning() << "Playlist is empty";
            return;
        }

        for(auto it = m_playlist.begin(); it != m_playlist.end(); ++it)
        {
            (*it).setIsCurrentPrintingElement(false);
        }
        (m_playlist.begin() + plsPos)->setIsCurrentPrintingElement(true);

        emit dataChanged(createIndex(0, 0), createIndex(m_playlist.size()-1, 0));
        break;
    }
    }
}

void PlaylistModel::slFileDataReady(QString fileName, QList<QVariant> fileData)
{
    if(m_previewData.value(fileName) != fileData)
    {
        m_previewData.insert(fileName, fileData);
        for(int pos=0; pos<m_playlist.size();pos++)
        {
            if(m_playlist.at(pos).fullFilePath() == fileName)
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
        QString newFilePath = newPlayList.at(pos);

        if(pos < m_playlist.size())
        {
            if(m_playlist.at(pos).fullFilePath() != newFilePath)
            {
                m_playlist.replace(pos, PlaylistElement(newFilePath));
                if(!m_previewData.contains(newFilePath))
                {
                    emit sgRequestFileData(newFilePath);
                }
                emit dataChanged(createIndex(pos, 0), createIndex(pos, 0));
            }
        }
        else
        {
            beginInsertRows(QModelIndex(), pos, pos);
            m_playlist.append(PlaylistElement(newFilePath));
            if(!m_previewData.contains(newFilePath))
            {
                emit sgRequestFileData(newFilePath);
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

void PlaylistModel::checkDataUpdate()
{
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::REQUEST_PROGRESS);
    emit sgRequest(FrameType::HARDWARE_ACTIONS, (uint8_t)Requests::Hardware::GET_MACHINE_MINUTES);
    emit sgRequest(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::REQUEST_PLAYLIST_POSITION);
}

void PlaylistModel::sendUpdatedPlaylist()
{
    QVariantList resultData;
    foreach (const PlaylistElement& element, m_playlist)
    {
        resultData.append(QVariant(element.fullFilePath()));
    }

    emit sgUpdateData(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::CHANGE_PLAYLIST, resultData);
    emit sgRequest(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::CHANGE_PLAYLIST_POSITION, curPlaylistPosition());
    qDebug() << "Updating playlist, current printing position:" << curPlaylistPosition();
}

qint32 PlaylistModel::curPlaylistPosition()
{
    auto playlistElement = std::find_if(m_playlist.begin(), m_playlist.end(), [](PlaylistElement element) {return element.isCurrentPrintingElement();});
    return std::distance(m_playlist.begin(), playlistElement);
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
