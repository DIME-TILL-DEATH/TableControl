#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "netmanager.h"
#include "requestmanager.h"
#include "answermanager.h"

#include "filemanager.h"

#include "hardware.h"
#include "playlistmodel.h"
#include "devicecontentmodel.h"
#include "progress.h"
#include "firmware.h"

#include "threadcontroller.h"

NetManager* netManager;
AnswerManager* answerManager;
RequestManager* requestManager;

FileManager* fileManager;

Hardware* hardware;
PlaylistModel* playlistModel;
DeviceContentModel* deviceContentModel;
Progress* progress;
Firmware* firmware;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setOrganizationName("FMone");
    app.setOrganizationDomain("fmone.ru");
    app.setApplicationName("Kinetic table");

    netManager = new NetManager(fileManager);
    requestManager = new RequestManager();
    answerManager = new AnswerManager();
    fileManager = new FileManager(answerManager, requestManager);
    
    hardware = new Hardware(answerManager, requestManager);
    playlistModel = new PlaylistModel(answerManager, requestManager);
    deviceContentModel = new DeviceContentModel(answerManager, requestManager);
    progress = new Progress(answerManager, requestManager);
    firmware = new Firmware(answerManager, requestManager);

    ThreadController threadController(QThread::currentThread());
    netManager->moveToThread(threadController.backendThread());
    fileManager->moveToThread(threadController.backendThread());
    // requestManager->moveToThread(threadController.backendThread());
    // answerManager->moveToThread(threadController.backendThread());

    // QObject::connect(requestManager, &RequestManager::sgNetRequest, netManager, &NetManager::sendNetRequest);
    QObject::connect(requestManager, &RequestManager::sgUpdateData, netManager, &NetManager::slUpdateData);

    QObject::connect(netManager, &NetManager::sgDataUpdated, requestManager, &RequestManager::slDataUpdated);
    QObject::connect(netManager, &NetManager::sgDeviceConnected, requestManager, &RequestManager::slDeviceConnected);
    QObject::connect(netManager, &NetManager::sgDeviceDisconnected, requestManager, &RequestManager::slDeviceDisconnected);

    QObject::connect(requestManager, &RequestManager::sgSendMessage, netManager, &NetManager::slSendMessage);

    QObject::connect(netManager, &NetManager::sgDataUpdated, answerManager, &AnswerManager::slDataUpdated);
    QObject::connect(netManager, &NetManager::sgNetEvent, answerManager, &AnswerManager::slNetEvent);
    QObject::connect(netManager, &NetManager::sgRecievingMessage, answerManager, &AnswerManager::slRecievingMessage);

    QObject::connect(fileManager, &FileManager::sgFileDataReady, playlistModel, &PlaylistModel::slFileDataReady);
    QObject::connect(playlistModel, &PlaylistModel::sgRequestFileData, fileManager, &FileManager::processFileLoadRequest);

    QObject::connect(deviceContentModel, &DeviceContentModel::sgRequestFileData, fileManager, &FileManager::processFileLoadRequest, Qt::QueuedConnection);

    qmlRegisterUncreatableType<ContentNode>("UiObjects", 1, 0, "ContentNode", "Cannot create ContentNode in QML");

    qmlRegisterSingletonInstance("UiObjects", 1, 0, "Hardware", hardware);
    qmlRegisterSingletonInstance("UiObjects", 1, 0, "PlaylistModel", playlistModel);
    qmlRegisterSingletonInstance("UiObjects", 1, 0, "DeviceContentModel", deviceContentModel);
    qmlRegisterSingletonInstance("UiObjects", 1, 0, "Progress", progress);
    qmlRegisterSingletonInstance("UiObjects", 1, 0, "Firmware", firmware);

    QQmlApplicationEngine engine;
    engine.addImportPath(":/qml/");
    const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
