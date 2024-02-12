#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "transport.h"

#include "filemanager.h"
#include "netmanager.h"
#include "playlistmodel.h"
#include "devicecontentmodel.h"
#include "progressmanager.h"
#include "firmwaremanager.h"

#include "threadcontroller.h"

FileManager* fileManager;
Transport* uiTransport;
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
    fileManager = new FileManager(netManager);

    uiTransport = new Transport(netManager);
    playlistModel = new PlaylistModel(netManager);
    deviceContentModel = new DeviceContentModel(netManager);
    progressManager = new ProgressManager(netManager);
    firmwareManager = new FirmwareManager(netManager);

    ThreadController threadController(QThread::currentThread());
    netManager->moveToThread(threadController.backendThread());
    fileManager->moveToThread(threadController.backendThread());

    QObject::connect(fileManager, &FileManager::sgFileDataReady, playlistModel, &PlaylistModel::slFileDataReady);
    QObject::connect(playlistModel, &PlaylistModel::sgRequestFileData, fileManager, &FileManager::processFileLoadRequest);

    QObject::connect(deviceContentModel, &DeviceContentModel::sgRequestFileData, fileManager, &FileManager::processFileLoadRequest, Qt::QueuedConnection);

    qmlRegisterUncreatableType<ContentNode>("UiObjects", 1, 0, "ContentNode", "Cannot create ContentNode in QML");

    //qRegisterMetaType<PlaylistElement>("PlaylistElement");

    qmlRegisterSingletonInstance("UiObjects", 1, 0, "Transport", uiTransport);
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
