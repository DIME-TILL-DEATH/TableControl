#include "devicecontentmodel.h"

DeviceContentModel::DeviceContentModel(QObject *parent)
    : QAbstractItemModel{parent}
{
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

void DeviceContentModel::slDeviceAvaliable()
{
    QVariantList data;
    data.append(librarySdcardPath);
    emit sgUpdateData(FrameType::FILE_ACTIONS, (uint8_t)(Requests::File::GET_FOLDER_CONTENT), data);
}

void DeviceContentModel::slContentDataUpdate(Data::File dataType, QVariantList dataList)
{
    switch (dataType)
    {
    case Data::File::REQUESTED_FOLDER:
    {
        QString fullFolderPath = dataList.at(0).toString();
        fullFolderPath.remove(librarySdcardPath);

        QStringList pathList = fullFolderPath.split("/", Qt::SkipEmptyParts);
//        ContentNode* currentPathNode = rootNode;
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
        currentPathNode->deleteAllChilds();

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
