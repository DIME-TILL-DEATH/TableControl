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

    connect(answerManager, &AnswerManager::sgFilePartDownloaded, this, &FileManager::saveFilePart);
    connect(answerManager, &AnswerManager::sgFileNotFound, this, &FileManager::requesteFileUnavaliable);
}


bool FileManager::getPointsFromFile(QString fileName, QList<QVariant>& result)
{
    QString dataPath;
#ifdef Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + '/';
#endif
    QString folder(dataPath + "preview/");
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
        qDebug() << "Can't open file" << fileName;
        return false;
    }
}

void FileManager::saveFilePart(QString filePath, const QByteArray &fileData, int32_t partPosition, int32_t fileSize)
{
    // qDebug() << "Save file part" << filePath << partPosition;
    if(partPosition == -1)
    {
        qDebug() << "Attempt to save NOT FOUND file";
        return;
    }

    QString dataPath;

    filePath.remove(DeviceContentModel::librarySdcardPath);
#ifdef Q_OS_IOS
    dataPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + '/';
#endif
    QFileInfo fileInfo(dataPath + "preview/" + filePath);

    QDir folder = fileInfo.dir();
    if(!folder.exists())
    {
        folder.mkpath(fileInfo.absolutePath());
    }

    QFile file(filePath);
    file.setFileName(fileInfo.absoluteFilePath());

    QAbstractSocket::OpenMode flags;
    if(partPosition == 0)
    {
        flags = QIODevice::WriteOnly | QIODevice::Text;
    }
    else
    {
        flags = QIODevice::Append | QIODevice::Text;
    }

    if(file.open(flags))
    {
        file.write(fileData);

        // TODO: append new data to preview and update
        // if((partPosition + fileData.size()) >= fileSize)
        if((file.size()) >= fileSize)
        {
            qDebug() << "File " << filePath << " downloaded.";
            QList<QVariant> dataFromFile;
            getPointsFromFile(filePath, dataFromFile);

            emit sgFileDataReady(filePath, dataFromFile);
        }
        file.close();
    }
    else
    {
        qDebug() << "Can't open file" << filePath;
    }
}

void FileManager::processFileLoadRequest(QString fileName)
{
    QList<QVariant> dataFromFile;

    if(m_loadedData.contains(fileName))
    {
        emit sgFileDataReady(fileName, m_loadedData.value(fileName));
    }
    else
    {
        if(getPointsFromFile(fileName, dataFromFile))
        {
            m_loadedData.insert(fileName, dataFromFile);
            emit sgFileDataReady(fileName, dataFromFile);
        }
        else
        {
            m_loadedData.insert(fileName, QVariantList());
            m_requestManager->requestFile(DeviceContentModel::librarySdcardPath + fileName);
        }
    }
}


void FileManager::requesteFileUnavaliable(QString filePath)
{
    // TODO: signal sgRequestedFileUnavaliable
    filePath.remove(DeviceContentModel::librarySdcardPath);
    qDebug() << "Requested file not found" << filePath;
    QList<QVariant> dummyData;
    dummyData.append("FILE NOT FOUND");
    emit sgFileDataReady(filePath, dummyData);
}
