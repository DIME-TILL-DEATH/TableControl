#ifndef LIBRARYELEMENT_H
#define LIBRARYELEMENT_H

#include "qqmlintegration.h"
#include <QObject>

class LibraryElement
{
    Q_GADGET
    QML_ELEMENT
    QML_UNCREATABLE("LibraryElement is for C++ instantiation only")

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString description READ description)
    Q_PROPERTY(QString coverPath READ coverPath)
    Q_PROPERTY(QString systemName READ systemName FINAL)
public:
    LibraryElement(QString systemName);
    LibraryElement(QString name, QString description, QString coverPath);

    QString systemName() const {return m_systemName;};
    QString name() const {return m_name;};
    QString description() const {return m_description;};
    QString coverPath() const {return m_coverPath;};
    QStringList playlist() const {return m_playlist;};

    void setName(const QString& name);
    void setDescription(const QString& description);
    void setCoverPath(const QString& coverPath);

    bool loadInfo();
    bool loadCover();
    bool loadPlaylist();

private:
    QString m_systemName;

    QString m_name{"undefined"};
    QString m_description{"undefined"};
    QString m_coverPath;

    QStringList m_playlist;

    QString path();
};


#endif // LIBRARYELEMENT_H
