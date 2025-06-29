#include "requestmanager.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>

#include "firmware.h"
#include "devicecontentmodel.h"

#include "intvaluemessage.h"
#include "filepartmessage.h"

RequestManager::RequestManager(QObject *parent)
    : QObject{parent}
{
    updateDataTimer = new QTimer(this);
    updateDataTimer->setInterval(500);

    connect(updateDataTimer, &QTimer::timeout, this, &RequestManager::checkDataUpdate);
}

void RequestManager::slDeviceConnected()
{
    requestParameter(Requests::Firmware::FIRMWARE_VERSION);
}

void RequestManager::slDeviceDisconnected()
{
    qDebug() << "Device unavaliable";

    updateDataTimer->stop();
    emit sgTableUnavaliable();
}


void RequestManager::slFirmwareVerisonRecieved(QString firmwareVersion)
{
    currentFw = firmwareVersion;
    if(Firmware::isVerisonSufficient(firmwareVersion))
    {
        requestParameter(Requests::Hardware::GET_SERIAL_ID);
    }
}

void RequestManager::slSerialIdRecieved(QString serialId)
{
    updateDataTimer->start();
    getTableBaseData();
}

void RequestManager::getTableBaseData()
{   
    requestParameter(Requests::Hardware::REQUEST_PROGRESS);
    requestParameter(Requests::Hardware::GET_PRINT_SPEED);
    requestParameter(Requests::Hardware::GET_LED_BRIGHTNESS);
    requestParameter(Requests::Hardware::GET_SCALE_COEFFICIENT);
    requestParameter(Requests::Hardware::GET_ROTATION);
    requestParameter(Requests::Hardware::GET_CORRECTION);
    requestParameter(Requests::Hardware::GET_PAUSE_INTERVAL);
    requestParameter(Requests::Hardware::GET_FI_GEAR2_TEETH_COUNT);
    requestParameter(Requests::Hardware::GET_MACHINE_MINUTES);

    requestParameter(Requests::Hardware::GET_FIRST_MOTOR_INVERSION);
    requestParameter(Requests::Hardware::GET_SECOND_MOTOR_INVERSION);

    requestParameter(Requests::Playlist::GET_CURRENT_GALLERY);

    requestParameter(Requests::Playlist::REQUEST_PLAYLIST);
    requestParameter(Requests::Playlist::REQUEST_PLAYLIST_POSITION);
    requestParameter(Requests::Hardware::REQUEST_PROGRESS);

    requestFolder(DeviceContentModel::librarySdcardPath);

    updateDataTimer->start();
    emit sgTableAvaliable();
}

void RequestManager::checkDataUpdate()
{
    requestParameter(Requests::Hardware::REQUEST_PROGRESS);
    requestParameter(Requests::Hardware::GET_MACHINE_MINUTES);
    requestParameter(Requests::Playlist::REQUEST_PLAYLIST_POSITION);
}

//==================Requests/Setters/Overloads==================================
void RequestManager::requestParameter(Requests::Hardware request)
{
    emit sgSendMessage(std::shared_ptr<AbstractMessage>(new IntValueMessage(FrameType::HARDWARE_ACTIONS, (uint8_t)request)));
}

void RequestManager::setParameter(Requests::Hardware request, uint32_t value)
{
    IntValueMessage* msg = new IntValueMessage(FrameType::HARDWARE_ACTIONS, (uint8_t)request, value);
    emit sgSendMessage(std::shared_ptr<AbstractMessage>(msg));
}

void RequestManager::setParameter(Requests::Hardware request, float_t value)
{
    FloatValueMessage* msg = new FloatValueMessage(FrameType::HARDWARE_ACTIONS, (uint8_t)request, value);
    emit sgSendMessage(std::shared_ptr<AbstractMessage>(msg));
}

void RequestManager::setParameter(Requests::Playlist request, uint32_t value)
{
    IntValueMessage* msg = new IntValueMessage(FrameType::PLAYLIST_ACTIONS, (uint8_t)request, value);
    emit sgSendMessage(std::shared_ptr<AbstractMessage>(msg));
}

void RequestManager::setParameter(Requests::Playlist request, QString string)
{
    StringMessage* msg = new StringMessage(FrameType::PLAYLIST_ACTIONS, (uint8_t)request, string);
    emit sgSendMessage(std::shared_ptr<AbstractMessage>(msg));
}

void RequestManager::requestParameter(Requests::Playlist request)
{
    emit sgSendMessage(std::shared_ptr<AbstractMessage>(new IntValueMessage(FrameType::PLAYLIST_ACTIONS, (uint8_t)request)));
}

void RequestManager::requestParameter(Requests::Firmware request)
{
    emit sgSendMessage(std::shared_ptr<AbstractMessage>(new IntValueMessage(FrameType::FIRMWARE_ACTIONS, (uint8_t)request)));
}

void RequestManager::requestFile(QString filePath)
{
    qDebug() << "Requesting file: " << filePath;
    StringMessage* msg = new StringMessage(FrameType::FILE_ACTIONS, (uint8_t)Requests::File::GET_FILE, filePath);
    emit sgSendMessage(std::shared_ptr<AbstractMessage>(msg));
}

void RequestManager::requestFolder(QString folderPath)
{
    qDebug() << "Requesting folder: " << folderPath;
    StringMessage* msg = new StringMessage(FrameType::FILE_ACTIONS, (uint8_t)Requests::File::GET_FOLDER_CONTENT, folderPath);
    emit sgSendMessage(std::shared_ptr<AbstractMessage>(msg));
}

void RequestManager::uploadFile(QString dstPath, QString srcPath)
{
    qDebug() << "Upload file " << srcPath  << " to" << dstPath;

    QFile file(srcPath);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << __FUNCTION__ << "Can't open file " << srcPath;
        return;
    }

    FilePartMessage* msg = new FilePartMessage(FilePartMessage::ActionType::CREATE_TEXT, dstPath, srcPath);
    emit sgSendMessage(std::shared_ptr<AbstractMessage>(msg));

    updateFileUploadProgress(NetEvents::StartUploadData, dstPath, 0, file.size());

    while(!file.atEnd())
    {
        int partPos = file.pos();
        FilePartMessage* msg = new FilePartMessage(FilePartMessage::ActionType::APPEND_TEXT, dstPath, srcPath, file.read(FilePartMessage::defaultPartSize), partPos);
        emit sgSendMessage(std::shared_ptr<AbstractMessage>(msg));

        updateFileUploadProgress(NetEvents::StartUploadData, dstPath, 0, file.size());
    }
    file.close();

    QStringList pathElements = dstPath.split("/", Qt::SkipEmptyParts);
    pathElements.pop_back();
    QString path = pathElements.join("/");
    requestFolder("/" + path + "/");
}

void RequestManager::uploadFolder(QString dstPath, QString srcPath)
{
    QDir dir(srcPath);
    QFileInfoList fileList = dir.entryInfoList();

    dstPath += dir.dirName() + "/";

    StringMessage* msg = new StringMessage(FrameType::FILE_ACTIONS, (uint8_t)Requests::File::FOLDER_CREATE, dstPath);
    emit sgSendMessage(std::shared_ptr<AbstractMessage>(msg));

    foreach(QFileInfo fileInfo, fileList)
    {
        QFile currentFile(fileInfo.absoluteFilePath());
        if(!currentFile.open(QIODevice::ReadOnly))
        {
            qDebug() << __FUNCTION__ << "Can't open file " << fileInfo.absoluteFilePath();
            continue;
        }
        qDebug() << "File opened" << fileInfo.fileName();
        QString uploadPath = dstPath + fileInfo.fileName();
        updateFileUploadProgress(NetEvents::StartUploadData, uploadPath, 0, currentFile.size());
        while(!currentFile.atEnd())
        {
            int partPos = currentFile.pos();
            FilePartMessage* msg = new FilePartMessage(FilePartMessage::ActionType::APPEND_TEXT, uploadPath,
                                                       srcPath, currentFile.read(FilePartMessage::defaultPartSize), partPos);
            emit sgSendMessage(std::shared_ptr<AbstractMessage>(msg));
        }
        currentFile.close();
    }
    requestFolder(DeviceContentModel::librarySdcardPath);
}

void RequestManager::updateFirmware(QString firmwarePath)
{
    qDebug() << "Upload firmware from " << firmwarePath;

    QFile file(firmwarePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << __FUNCTION__ << "Can't open firmware file " << firmwarePath;
        return;
    }

    int64_t partPos = file.pos();
    QString firmwareDstPath = Firmware::dstPath;

    if(!Firmware::isVerisonSufficient(currentFw, "0.80"))
    {
        partPos = FilePartMessage::defaultPartSize;
        firmwareDstPath = "";
    }

    FilePartMessage* msg = new FilePartMessage(FilePartMessage::ActionType::CREATE_FIRMWARE, firmwareDstPath, firmwarePath, file.read(FilePartMessage::defaultPartSize), partPos);
    emit sgSendMessage(std::shared_ptr<AbstractMessage>(msg));

    while(!file.atEnd())
    {
        partPos = file.pos();

        if(!Firmware::isVerisonSufficient(currentFw, "0.80")) partPos = FilePartMessage::defaultPartSize; //legacy support

        FilePartMessage* msg = new FilePartMessage(FilePartMessage::ActionType::APPEND_FIRMWARE, firmwareDstPath, firmwarePath, file.read(FilePartMessage::defaultPartSize), partPos);
        emit sgSendMessage(std::shared_ptr<AbstractMessage>(msg));

    }
    updateFileUploadProgress(NetEvents::UploadFirmwareStart, Firmware::dstPath, 0, file.size());
    file.close();
}

void RequestManager::updateFileUploadProgress(NetEvents type, QString filePath, quint64 currentPartSize, quint64 totalSize)
{
    qDebug() << Q_FUNC_INFO << filePath << currentPartSize << totalSize;
    QVariantList netEventData;
    netEventData.append(currentPartSize);
    netEventData.append(totalSize);
    emit sgNetEvent(type, filePath, netEventData);
}
