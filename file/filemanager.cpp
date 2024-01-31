#include <QFile>
#include <QDir>
#include <QDebug>

#include "filemanager.h"
#include "devicecontentmodel.h"

FileManager::FileManager(NetManager *netManager, QObject *parent)
    : QObject{parent}
{
    QObject::connect(netManager, &NetManager::sgDataUpdated, this, &FileManager::slDataUpdated);
    QObject::connect(this, &FileManager::sgUpdateData, netManager, &NetManager::slUpdateData);
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
    QFileInfo fileInfo("preview/" + filePath);

    QDir folder = fileInfo.dir();

//    QString folder("preview/");
//    if(!QDir(folder).exists())
//    {
//        QDir().mkpath(folder);
//    }
    if(!folder.exists())
    {
        folder.mkpath(fileInfo.absolutePath());
    }

    QFile file(filePath);
    file.setFileName(fileInfo.absoluteFilePath());

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
    //qDebug() << "Processing content load request" << fileName;
    QList<QVariant> dataFromFile;

    if(m_loadedData.contains(fileName))
    {
        //qDebug() << "send pointer to already loaded data";
        emit sgFileDataReady(fileName, m_loadedData.value(fileName));
    }
    else
    {
        if(getPointsFromFile(fileName, dataFromFile))
        {
            //qDebug() << "loading file";
            m_loadedData.insert(fileName, dataFromFile);
            emit sgFileDataReady(fileName, dataFromFile);
        }
        else
        {
            // TODO двойные запросы на скачку от разных модулей. Загружают сеть ненужной работой
            m_loadedData.insert(fileName, QVariantList());
            //qDebug() << "try to download file";
            QVariantList data;
            data.append(DeviceContentModel::librarySdcardPath + fileName);
            emit sgUpdateData(FrameType::FILE_ACTIONS, (uint8_t)(Requests::File::GET_FILE), data);
        }
    }
}

void FileManager::slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList dataList)
{
    if(frameType != FrameType::FILE_ACTIONS) return;

    switch((Data::File)dataType)
    {
    case Data::File::REQUESTED_FILE:
    {
        QString fileName = dataList.at(0).toString();
        QList<QVariant> dataFromFile;
        getPointsFromFile(fileName, dataFromFile);
        emit sgFileDataReady(fileName, dataFromFile);
        break;
    }

    case Data::File::REQUESTED_FILE_NOT_FOUND:
    {
        QString fileName = dataList.at(0).toString();
        QList<QVariant> dummyData;
        dummyData.append("FILE NOT FOUND");
        emit sgFileDataReady(fileName, dummyData);
        break;
    }

    default: {}
    }
}
