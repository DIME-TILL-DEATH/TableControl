#ifndef PLAYLISTELEMENT_H
#define PLAYLISTELEMENT_H

#include "qqmlintegration.h"
#include <QObject>

class PlaylistElement
{
    Q_GADGET
    QML_ELEMENT
    QML_UNCREATABLE("AppManager is for C++ instantiation only")

    Q_PROPERTY(QString fileName READ fileName)
    Q_PROPERTY(QString filePath READ filePath)
    Q_PROPERTY(bool isCurrentPrintingElement READ isCurrentPrintingElement WRITE setIsCurrentPrintingElement)
public:
    explicit PlaylistElement(QObject *parent = nullptr);
    PlaylistElement(QString filePath, QObject *parent = nullptr);

    QString fileName() const;

    QString filePath() const;

    QString fullFilePath() const;

    bool isCurrentPrintingElement() const;
    void setIsCurrentPrintingElement(bool newIsCurrentPrintingElement);

private:
    QString m_fileName;
    QString m_filePath;
    QString m_fullFilePath;
    bool m_isCurrentPrintingElement{false};
};

#endif // PLAYLISTELEMENT_H
