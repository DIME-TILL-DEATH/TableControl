#include <QFile>
#include <QDir>
#include <QDebug>

#include "filemanager.h"

FileManager::FileManager(QObject *parent)
    : QObject{parent}
{

}

bool FileManager::getPointsFromFile(QString fileName, QList<QVariant>& result)
{
    QString folder("preview/");
    QFile gcodeFile(folder+fileName);
    if(gcodeFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&gcodeFile);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList params = line.split(" ");
            if(params.at(0) != "G1") continue;
            else
            {
                QString xstr = params.at(1);
                QString ystr = params.at(2);
                xstr.remove("X");
                ystr.remove("Y");

                result.append(QPointF(xstr.toFloat(), ystr.toFloat()));
            }
        }
        return true;
    }
    else
    {
        qDebug() << "Can't open file" << fileName;
        return false;
    }
}

void FileManager::savePreviewFile(QString filePath, const QByteArray &fileData)
{
    QString folder("preview/");
    if(!QDir(folder).exists())
    {
        QDir().mkpath(folder);
    }

    QFile file(filePath);
    file.setFileName(folder+filePath);

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        file.write(fileData);
        file.close();
        qDebug() << "File saved";
    }
    else
    {
        qDebug() << "Can't open file" << filePath;
    }
}

void FileManager::processFileLoadRequest(QString fileName)
{
    QList<QVariant> dataFromFile;
    if(getPointsFromFile(fileName, dataFromFile))
    {
        emit sgFileDataReady(fileName, dataFromFile);
    }
    else
    {
        QVariantList data;
        data.append(fileName);
        emit sgUpdateData(FrameType::FILE_ACTIONS, (uint8_t)(Requests::File::GET_FILE), data);
    }
}

void FileManager::processDownloadedFile(Data::File dataType, QVariantList dataList)
{
    switch(dataType)
    {
    case Data::File::REQUESTED_FILE:
    {
        QString fileName = dataList.at(0).toString();
        QList<QVariant> dataFromFile;
        getPointsFromFile(fileName, dataFromFile);
        emit sgFileDataReady(fileName, dataFromFile);
        break;
    }
    }
}
