#include <QPointF>

#include "playlistmodel.h"
#include "filemanager.h"

PlaylistModel::PlaylistModel(QObject *parent)
    : QAbstractListModel{parent}
{
    updateDataTimer = new QTimer(this);
    updateDataTimer->setInterval(1000);
    updateDataTimer->start();

    connect(updateDataTimer, &QTimer::timeout, this, &PlaylistModel::checkDataUpdate);
}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ListRoles::FileNameRole] = "fileName";
    roles[ListRoles::PreviewDataRole] = "previewData";

    return roles;
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
        setCurrentPlsPos(plsPos);
        if(plsPos < m_playlist.size())
        {            
            setCurPrintFileName(m_playlist.at(plsPos));
        }
        break;
    }
    }
}

void PlaylistModel::slFileDataReady(QString fileName, QList<QVariant> fileData)
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

int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_playlist.size();
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() > rowCount(index))
    {
        return {};
    }

    switch(role)
    {
        case ListRoles::FileNameRole:
        {
            return QVariant::fromValue(m_playlist.at(index.row()));
        }

        case ListRoles::PreviewDataRole:
        {
            QString fileName = m_playlist.at(index.row());
            return QVariant(m_previewData.value(fileName));
        }

        default:
        {
            qWarning() << __FUNCTION__ << "no role";
            return true;
        }
    }
}

void PlaylistModel::refreshModel(QList<QString> newPlayList)
{
    for(int pos=0; pos < newPlayList.size()-1; pos++)
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
    setCurPrintFileName(m_playlist.at(m_currentPlsPos));
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

        QVariantList resultData;
        foreach (QString name, m_playlist)
        {
            resultData.append(QVariant(name));
        }

        auto it = std::find(m_playlist.begin(), m_playlist.end(), m_curPrintFileName);
        setCurrentPlsPos(std::distance(m_playlist.begin(), it));

        emit sgUpdateData(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::CHANGE_PLAYLIST, resultData);
        resultData.clear();
        resultData.append(m_currentPlsPos);
        emit sgUpdateData(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::CHANGE_PLAYLIST_POSITION, resultData);
    }
}

quint16 PlaylistModel::currentPlsPos() const
{
    return m_currentPlsPos;
}

void PlaylistModel::setCurrentPlsPos(quint16 newCurrentPlsPos)
{
    if (m_currentPlsPos == newCurrentPlsPos)
        return;
    m_currentPlsPos = newCurrentPlsPos;
    emit currentPlsPosChanged();
}

QString PlaylistModel::curPrintFileName() const
{
    return m_curPrintFileName;
}

void PlaylistModel::setCurPrintFileName(const QString &newCurPrintFileName)
{
    if (m_curPrintFileName == newCurPrintFileName)
        return;
    m_curPrintFileName = newCurPrintFileName;
    emit curPrintFileNameChanged();
}

void PlaylistModel::checkDataUpdate()
{
    emit sgRequest(FrameType::TRANSPORT_ACTIONS, (uint8_t)Requests::Transport::REQUEST_PROGRESS);
    emit sgRequest(FrameType::PLAYLIST_ACTIONS, (uint8_t)Requests::Playlist::REQUEST_PLAYLIST_POSITION);
}
