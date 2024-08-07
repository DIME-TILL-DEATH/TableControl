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
#include "librarymodel.h"
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
LibraryModel* libraryModel;
Progress* progress;
Firmware* firmware;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setOrganizationName("FMone");
    app.setOrganizationDomain("fmone.ru");
    app.setApplicationName("Kinetic table");

    netManager = new NetManager();
    requestManager = new RequestManager();
    answerManager = new AnswerManager();
    fileManager = new FileManager(answerManager, requestManager);
    
    hardware = new Hardware(answerManager, requestManager);
    playlistModel = new PlaylistModel(answerManager, requestManager, fileManager);
    deviceContentModel = new DeviceContentModel(answerManager, requestManager, fileManager);
    libraryModel = new LibraryModel(answerManager, requestManager, fileManager);
    progress = new Progress(answerManager, requestManager);
    firmware = new Firmware(answerManager, requestManager);

    ThreadController threadController(QThread::currentThread());
    netManager->moveToThread(threadController.backendThread());
    fileManager->moveToThread(threadController.backendThread());
    requestManager->moveToThread(threadController.backendThread());
    answerManager->moveToThread(threadController.backendThread());

    QObject::connect(netManager, &NetManager::sgDeviceConnected, requestManager, &RequestManager::slDeviceConnected);
    QObject::connect(netManager, &NetManager::sgDeviceDisconnected, requestManager, &RequestManager::slDeviceDisconnected);
    QObject::connect(answerManager, &AnswerManager::sgFirmwareVersion, requestManager, &RequestManager::slFirmwareVerisonRecieved);
    QObject::connect(answerManager, &AnswerManager::sgSerialId, requestManager, &RequestManager::slSerialIdRecieved);

    QObject::connect(requestManager, &RequestManager::sgSendMessage, netManager, &NetManager::slSendMessage);
    QObject::connect(answerManager, &AnswerManager::sgSendMessage, netManager, &NetManager::slSendMessage);

    QObject::connect(netManager, &NetManager::sgNetEvent, answerManager, &AnswerManager::slNetEvent);
    QObject::connect(netManager, &NetManager::sgRecievingMessage, answerManager, &AnswerManager::slRecieveMessage);

    qmlRegisterUncreatableType<ContentNode>("UiObjects", 1, 0, "ContentNode", "Cannot create ContentNode in QML");

    qmlRegisterSingletonInstance("UiObjects", 1, 0, "Hardware", hardware);
    qmlRegisterSingletonInstance("UiObjects", 1, 0, "PlaylistModel", playlistModel);
    qmlRegisterSingletonInstance("UiObjects", 1, 0, "DeviceContentModel", deviceContentModel);
    qmlRegisterSingletonInstance("UiObjects", 1, 0, "LibraryModel", libraryModel);
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
