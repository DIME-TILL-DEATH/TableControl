#include "playlistelement.h"

PlaylistElement::PlaylistElement(QObject *parent)
{

}

PlaylistElement::PlaylistElement(QString filePath, QObject *parent)
{
    m_fullFilePath = filePath;

    QStringList pathElements = filePath.split("/", Qt::SkipEmptyParts);

    m_fileName = pathElements.last();

    pathElements.pop_back();
    m_filePath = pathElements.join("/");
}

QString PlaylistElement::fileName() const
{
    return m_fileName;
}

QString PlaylistElement::filePath() const
{
    return m_filePath;
}

QString PlaylistElement::fullFilePath() const
{
    return m_fullFilePath;
}

bool PlaylistElement::isCurrentPrintingElement() const
{
    return m_isCurrentPrintingElement;
}

void PlaylistElement::setIsCurrentPrintingElement(bool newIsCurrentPrintingElement)
{
    m_isCurrentPrintingElement = newIsCurrentPrintingElement;
}
