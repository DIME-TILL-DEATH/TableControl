#include <QFile>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>

#include "libraryelement.h"

LibraryElement::LibraryElement(QString systemName)
    : m_systemName(systemName)
{

}

LibraryElement::LibraryElement(QString name, QString description, QString coverPath)
    : m_name(name),
    m_description(description),
    m_coverPath(coverPath)
{

}

void LibraryElement::setName(const QString &name)
{
    if(name != m_name)
    {
        m_name = name;
    }
}

void LibraryElement::setDescription(const QString &description)
{
    if(description != description)
    {
        m_description = description;
    }

}

void LibraryElement::setCoverPath(const QString &coverPath)
{
    if(coverPath != m_coverPath)
    {
        m_coverPath = coverPath;
    }
}

bool LibraryElement::loadInfo()
{
    QFile infoFile(path() + "info.ini");
    if(infoFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&infoFile);
        while (!in.atEnd())
        {
            QString line = in.readLine();

            if(line.indexOf("NAME=") == 0)
            {
                line.remove(0, 5);
                setName(line);
            }

            if(line.indexOf("DESCRIPTION=") == 0)
            {
                line.remove(0, 12);
                setDescription(line);
            }

        }
        return true;
    }
    else
    {
        qDebug() << "Can't open info file for library element " << m_systemName;
        return false;
    }
}

bool LibraryElement::loadCover()
{
    QFile coverFile(path() + "cover.jpg");
    QFileInfo info(coverFile);

    if(coverFile.exists())
    {
        m_coverPath = info.absoluteFilePath();
        return true;
    }
    else
    {
        qDebug() << "Can't open cover file for library element " << m_systemName;
        return false;
    }
}

bool LibraryElement::loadPlaylist()
{
    QFile playlistFile(path() + "playlist.pls");
    if(playlistFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&playlistFile);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            m_playlist.append(line);
        }
        return true;
    }
    else
    {
        qDebug() << "Can't open info file for library element " << m_systemName;
        return false;
    }
}

QString LibraryElement::path()
{
    QString dataPath;
#ifdef Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + '/';
#endif
    return dataPath + "preview/" + m_systemName + "/";
}
