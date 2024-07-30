#include <QFile>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>

#include "filemanager.h"
#include "devicecontentmodel.h"

FileManager::FileManager(AnswerManager *answerManager, RequestManager* requestManager, QObject *parent)
    : QObject{parent}
{
    m_requestManager = requestManager;

    connect(answerManager, &AnswerManager::sgSerialId, this, &FileManager::setDeviceSerialId);

    connect(answerManager, &AnswerManager::sgFilePartDownloaded, this, &FileManager::saveFilePart);
    connect(answerManager, &AnswerManager::sgFileNotFound, this, &FileManager::requesteFileUnavaliable);
}


bool FileManager::getPointsFromFile(QString fileName, QList<QVariant>& result)
{
    QString dataPath;
#ifdef Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + '/';
#endif
    QString folder(dataPath + "preview/" + m_deviceSerialId + "/");
    QFile gcodeFile(folder+fileName);
    if(gcodeFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&gcodeFile);
        while (!in.atEnd())
        {
            QString line = in.readLine();
            QStringList params = line.split(" ");

            if(params.size() < 3) continue;

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
        qDebug() << __FUNCTION__ << "Can't open file" << fileName;
        return false;
    }
}

void FileManager::saveFilePart(QString filePath, const QByteArray &fileData, int32_t partPosition, int32_t fileSize)
{
    if(partPosition == -1)
    {
        qDebug() << "Attempt to save NOT FOUND file";
        return;
    }

    filePath.remove(DeviceContentModel::librarySdcardPath);

    QString dataPath;
#ifdef Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + '/';
#endif
    QFileInfo fileInfo(dataPath + "preview/" + m_deviceSerialId + "/" + filePath);

    QDir folder = fileInfo.dir();
    if(!folder.exists())
    {
        folder.mkpath(fileInfo.absolutePath());
    }

    QFile file(filePath);
    file.setFileName(fileInfo.absoluteFilePath());

    QStringList pathParts = filePath.split(".");

    QAbstractSocket::OpenMode flags;
    if(pathParts.last() == "jpg" || pathParts.last() == "jpeg")
    {

    }
    else
    {
        flags = QIODevice::Text;
    }

    if(partPosition == 0)
    {
        flags |= QIODevice::WriteOnly;
    }
    else
    {
        flags |= QIODevice::Append;
    }

    if(file.open(flags))
    {
        file.write(fileData);

        // TODO: append new data to preview and update
        // if((partPosition + fileData.size()) >= fileSize)

        // qDebug() << partPosition << fileData.size() << fileSize << file.size();
        if((file.size()) >= fileSize)
        {
            qDebug() << "File " << filePath << " downloaded.";

            if(pathParts.last() == "gcode")
            {
                // TODO: более универсальный алгорит, gCOde вынести куда-нибудь
                QList<QVariant> dataFromFile;
                getPointsFromFile(filePath, dataFromFile);

                emit sgGCodeDataReady(filePath, dataFromFile);
            }
            else
            {
                emit sgFileDownloaded(filePath);
            }
        }
        file.close();
    }
    else
    {
        qDebug() << __FUNCTION__ << "Can't open file" << filePath;
    }
}

void FileManager::loadGCodeFileRequest(QString fileName)
{
    QList<QVariant> dataFromFile;

    if(m_loadedGCodeData.contains(fileName))
    {
        emit sgGCodeDataReady(fileName, m_loadedGCodeData.value(fileName));
    }
    else
    {
        if(getPointsFromFile(fileName, dataFromFile))
        {
            m_loadedGCodeData.insert(fileName, dataFromFile);
            emit sgGCodeDataReady(fileName, dataFromFile);
        }
        else
        {
            m_loadedGCodeData.insert(fileName, QVariantList());
            downloadFileRequest(fileName);
        }
    }
}

void FileManager::downloadFileRequest(QString fileName)
{
    // if(!m_downloadRequests.contains(fileName))
    //     m_downloadRequests.append(fileName);


    m_requestManager->requestFile(DeviceContentModel::librarySdcardPath + fileName);
}

void FileManager::setDeviceSerialId(const QString &newDeviceSerialId)
{
    m_deviceSerialId = newDeviceSerialId;
}


void FileManager::requesteFileUnavaliable(QString filePath)
{
    // TODO: signal sgRequestedFileUnavaliable
    filePath.remove(DeviceContentModel::librarySdcardPath);
    qDebug() << "Requested file not found" << filePath;
    QList<QVariant> dummyData;
    dummyData.append("FILE NOT FOUND");
    emit sgGCodeDataReady(filePath, dummyData);
}
