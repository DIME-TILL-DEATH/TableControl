#include "devicecontentmodel.h"

#ifdef __ANDROID__
#include <jni.h>
#include "androidutils.h"
#endif

DeviceContentModel::DeviceContentModel(NetManager *netManager, QObject *parent)
    : QAbstractItemModel{parent}
{
    QObject::connect(this, &DeviceContentModel::sgRequest, netManager, &NetManager::sendRequest);
    QObject::connect(this, &DeviceContentModel::sgUpdateData, netManager, &NetManager::slUpdateData);

    QObject::connect(netManager, &NetManager::sgDeviceConnected, this, &DeviceContentModel::slDeviceAvaliable);
    QObject::connect(netManager, &NetManager::sgDataUpdated, this, &DeviceContentModel::slDataUpdated);

#ifdef Q_OS_ANDROID
    QObject::connect(&activityResultHandler, &ActivityResultManager::sgFilePicked, this, &DeviceContentModel::slAndroidFilePicked);
#endif

    rootNode = new ContentNode("", ContentNode::NodeType::Root);
    sdCardNode = new ContentNode("SD card", ContentNode::NodeType::Root);
    rootNode->appendChild(sdCardNode);
}

DeviceContentModel::~DeviceContentModel()
{
    delete rootNode;
}

QModelIndex DeviceContentModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    ContentNode *parentItem;

    if (!parent.isValid())
        parentItem = rootNode;
    else
        parentItem = static_cast<ContentNode*>(parent.internalPointer());

    ContentNode *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

int DeviceContentModel::rowCount(const QModelIndex &parent) const
{
    ContentNode *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootNode;
    else
        parentItem = static_cast<ContentNode*>(parent.internalPointer());

    return parentItem->childCount();
}

int DeviceContentModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QModelIndex DeviceContentModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    ContentNode *childItem = static_cast<ContentNode*>(index.internalPointer());
    ContentNode *parentItem = childItem->parentItem();

    if (parentItem == rootNode)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

Qt::ItemFlags DeviceContentModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant DeviceContentModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
    {
        return QVariant();
    }

    ContentNode *item = static_cast<ContentNode*>(index.internalPointer());
    return item->data(role);

    return QVariant();
}

QHash<int, QByteArray> DeviceContentModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[ContentNode::ListRoles::NodeNameRole] = "name";
    roles[ContentNode::ListRoles::NodeTypeRole] = "type";
    roles[ContentNode::ListRoles::NodePathRole] = "path";
    roles[ContentNode::ListRoles::PreviewDataRole] = "previewData";
    return roles;
}

void DeviceContentModel::uploadFileToDevice(QString dstPath, QString srcPath)
{
    QVariantList data;
    data.append(librarySdcardPath + dstPath);
    data.append(srcPath);
    emit sgUpdateData(FrameType::FILE_ACTIONS, (uint8_t)(Requests::File::FILE_CREATE), data);
}

void DeviceContentModel::slDeviceAvaliable()
{
    QVariantList data;
    data.append(librarySdcardPath);
    emit sgUpdateData(FrameType::FILE_ACTIONS, (uint8_t)(Requests::File::GET_FOLDER_CONTENT), data);
}

void DeviceContentModel::selectFile()
{
#ifdef Q_OS_ANDROID
    AndroidUtils::pickFile(ActivityType::PICK_FILE, "*/*", &activityResultHandler);
#else
    emit sgOpenPlatformFileDialog();
#endif
}

void DeviceContentModel::slAndroidFilePicked(QString filePath, QString fileName)
{
    uploadFileToDevice(m_currentDstPath + "/" + fileName, filePath);
}

void DeviceContentModel::slDataUpdated(FrameType frameType, uint8_t dataType, QVariantList dataList)
{
    if(frameType != FrameType::FILE_ACTIONS) return;

    switch ((Data::File)dataType)
    {
    case Data::File::REQUESTED_FOLDER:
    {
        QString fullFolderPath = dataList.at(0).toString();
        fullFolderPath.remove(librarySdcardPath);

        QStringList pathList = fullFolderPath.split("/", Qt::SkipEmptyParts);
        ContentNode* currentPathNode = sdCardNode;
        foreach(auto pathElement, pathList)
        {
            ContentNode* nextPathNode = currentPathNode->childByName(pathElement);
            if(nextPathNode)
            {
                currentPathNode = nextPathNode;
            }
            else
            {
                ContentNode* newPathElement = new ContentNode(pathElement, ContentNode::NodeType::Folder);
                appendNode(currentPathNode, newPathElement);
                currentPathNode = newPathElement;
            }
        }

        int row = currentPathNode->childCount();
        if(row>0)
        {
            QModelIndex index = createIndex(currentPathNode->row(), 0, currentPathNode);

            beginRemoveRows(index, 0, row-1);
            currentPathNode->deleteAllChilds();
            endRemoveRows();
        }

        dataList.pop_front();
        foreach (auto contentNameVariant, dataList)
        {
            QString contentName = contentNameVariant.toString();
            if(contentName.indexOf("DIR|") == 0)
            {
                contentName.remove(0, 4);

                appendNode(currentPathNode, new ContentNode(contentName, ContentNode::NodeType::Folder));

                QVariantList data;
                data.append(librarySdcardPath + fullFolderPath + contentName + "/");
                emit sgUpdateData(FrameType::FILE_ACTIONS, (uint8_t)(Requests::File::GET_FOLDER_CONTENT), data);
            }
            else
            {
                appendNode(currentPathNode, new ContentNode(contentName, ContentNode::NodeType::File));
            }
        }
        if(currentPathNode->childCount() >= 2)
        {
            currentPathNode->sortChilds();
            emit dataChanged(createIndex(0, 0, currentPathNode), createIndex(currentPathNode->childCount()-1, 0, currentPathNode));
        }
        break;
    }
    default:
        break;
    }
}

void DeviceContentModel::appendNode(ContentNode *destNode, ContentNode *newNode)
{
    int row = destNode->childCount();
    QModelIndex index = createIndex(row, 0, destNode);

    beginInsertRows(index, row, row);
    destNode->appendChild(newNode);
    endInsertRows();
}

void DeviceContentModel::slFileDataReady(QString fileName, QList<QVariant> fileData)
{

}

QString DeviceContentModel::currentDstPath() const
{
    return m_currentDstPath;
}

void DeviceContentModel::setCurrentDstPath(const QString &newCurrentDstPath)
{
    if (m_currentDstPath == newCurrentDstPath)
        return;
    m_currentDstPath = newCurrentDstPath;
    emit currentDstPathChanged();
}
