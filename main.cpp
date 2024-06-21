#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "requestmanager.h"
#include "hardware.h"

#include "filemanager.h"
#include "netmanager.h"
#include "playlistmodel.h"
#include "devicecontentmodel.h"
#include "progressmanager.h"
#include "firmwaremanager.h"

#include "threadcontroller.h"

RequestManager* requestManager;
FileManager* fileManager;
Hardware* hardware;
NetManager* netManager;
PlaylistModel* playlistModel;
DeviceContentModel* deviceContentModel;
ProgressManager* progressManager;
FirmwareManager* firmwareManager;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setOrganizationName("FMone");
    app.setOrganizationDomain("fmone.ru");
    app.setApplicationName("Kinetic table");

    netManager = new NetManager();
    requestManager = new RequestManager(); // TODO: backend thread?

    fileManager = new FileManager(netManager);
    
    // TODO: Answer manager?
    hardware = new Hardware(netManager, requestManager);
    playlistModel = new PlaylistModel(netManager, requestManager);
    deviceContentModel = new DeviceContentModel(netManager, requestManager);
    progressManager = new ProgressManager(netManager, requestManager);
    firmwareManager = new FirmwareManager(netManager);

    ThreadController threadController(QThread::currentThread());
    netManager->moveToThread(threadController.backendThread());
    fileManager->moveToThread(threadController.backendThread());

    QObject::connect(requestManager, &RequestManager::sgNetRequest, netManager, &NetManager::sendNetRequest);
    QObject::connect(requestManager, &RequestManager::sgUpdateData, netManager, &NetManager::slUpdateData);

    QObject::connect(netManager, &NetManager::sgDataUpdated, requestManager, &RequestManager::slDataUpdated);
    QObject::connect(netManager, &NetManager::sgDeviceConnected, requestManager, &RequestManager::slDeviceConnected);
    QObject::connect(netManager, &NetManager::sgDeviceDisconnected, requestManager, &RequestManager::slDeviceDisconnected);

    QObject::connect(fileManager, &FileManager::sgFileDataReady, playlistModel, &PlaylistModel::slFileDataReady);
    QObject::connect(playlistModel, &PlaylistModel::sgRequestFileData, fileManager, &FileManager::processFileLoadRequest);

    QObject::connect(deviceContentModel, &DeviceContentModel::sgRequestFileData, fileManager, &FileManager::processFileLoadRequest, Qt::QueuedConnection);

    qmlRegisterUncreatableType<ContentNode>("UiObjects", 1, 0, "ContentNode", "Cannot create ContentNode in QML");

    //qRegisterMetaType<PlaylistElement>("PlaylistElement");

    qmlRegisterSingletonInstance("UiObjects", 1, 0, "Hardware", hardware);
    qmlRegisterSingletonInstance("UiObjects", 1, 0, "PlaylistModel", playlistModel);
    qmlRegisterSingletonInstance("UiObjects", 1, 0, "DeviceContentModel", deviceContentModel);
    qmlRegisterSingletonInstance("UiObjects", 1, 0, "ProgressManager", progressManager);
    qmlRegisterSingletonInstance("UiObjects", 1, 0, "FirmwareManager", firmwareManager);

    QQmlApplicationEngine engine;
    engine.addImportPath(":/qml/");
    const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
