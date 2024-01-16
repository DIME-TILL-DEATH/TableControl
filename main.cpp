#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "uitransport.h"

#include "filemanager.h"
#include "netmanager.h"
#include "playlistmodel.h"

#include "threadcontroller.h"

FileManager* fileManager;
UiTransport* uiTransport;
NetManager* netManager;
PlaylistModel* playlistModel;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setOrganizationName("FMone");
    app.setOrganizationDomain("fmone.ru");
    app.setApplicationName("Kinetic table");

    netManager = new NetManager();
    uiTransport = new UiTransport();
    playlistModel = new PlaylistModel();
    fileManager = new FileManager();

    ThreadController threadController(QThread::currentThread());
    netManager->moveToThread(threadController.backendThread());
    fileManager->moveToThread(threadController.backendThread());

    QObject::connect(uiTransport, &UiTransport::sgRequest, netManager, &NetManager::sendRequest);
    QObject::connect(netManager, &NetManager::sgTransportDataUpdated, uiTransport, &UiTransport::slTransportUpdate);

    QObject::connect(netManager, &NetManager::sgPlaylistDataUpdated, playlistModel, &PlaylistModel::slPlaylistDataUpdate);
    QObject::connect(netManager, &NetManager::sgFileDataUpdated, fileManager, &FileManager::processDownloadedFile);

    QObject::connect(playlistModel, &PlaylistModel::sgRequest, netManager, &NetManager::sendRequest);
    QObject::connect(playlistModel, &PlaylistModel::sgUpdateData, netManager, &NetManager::updateData);
    QObject::connect(playlistModel, &PlaylistModel::sgRequestFileData, fileManager, &FileManager::processFileLoadRequest);

    QObject::connect(fileManager, &FileManager::sgFileDataReady, playlistModel, &PlaylistModel::slFileDataReady);
    QObject::connect(fileManager, &FileManager::sgUpdateData, netManager, &NetManager::updateData);

    qmlRegisterSingletonInstance("UiObjects", 1, 0, "TransportCore", uiTransport);
    qmlRegisterSingletonInstance("UiObjects", 1, 0, "PlaylistModel", playlistModel);

    QQmlApplicationEngine engine;
    engine.addImportPath(":/qml/");
    const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
