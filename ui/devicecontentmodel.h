#ifndef DEVICECONTENTMODEL_H
#define DEVICECONTENTMODEL_H

#include <QAbstractListModel>
#include <QObject>

#include "answermanager.h"
#include "requestmanager.h"
#include "filemanager.h"

#include "contentnode.h"

#ifdef __ANDROID__
#include <jni.h>
#include "activityresultmanager.h"
#endif

class DeviceContentModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(QString currentDstPath READ currentDstPath WRITE setCurrentDstPath NOTIFY currentDstPathChanged FINAL)
public:
    explicit DeviceContentModel(AnswerManager *answerManager, RequestManager* requestManager, FileManager* fileManager, QObject *parent = nullptr);
    ~DeviceContentModel();

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QModelIndex parent(const QModelIndex &index) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QHash<int, QByteArray> roleNames() const override;

    constexpr static const char librarySdcardPath[] = "/sdcard/library/";

    Q_INVOKABLE void selectFile();
    Q_INVOKABLE void uploadFileToDevice(QString dstPath, QString srcPath);

    Q_INVOKABLE void uploadPlaylist(QString selectedFolder);
    Q_INVOKABLE void deletePlaylist(QString playlistName);

    QString currentDstPath() const;
    void setCurrentDstPath(const QString &newCurrentDstPath);

signals:
    void sgRequestFileData(QString fileName);

    void sgOpenPlatformFileDialog();

    void currentDstPathChanged();

    void sgUploadFolder(QString dstPath, QString srcPath);

public slots:
    void slContentUpdated(QString path, QStringList contentList);
    void slFileDataReady(QString fileName, QList<QVariant> fileData);

    void slAndroidFilePicked(QString filePath, QString fileName);

private:
    RequestManager* m_requestManager;

    ContentNode* rootNode;
    ContentNode* sdCardNode;

    void appendNode(ContentNode* destNode, ContentNode* newNode);


    #ifdef __ANDROID__
    ActivityResultManager activityResultHandler;
    #endif
    QString m_currentDstPath;
};

#endif // DEVICECONTENTMODEL_H
