#ifndef CONTENTNODE_H
#define CONTENTNODE_H

#include <QVariant>

enum class NodeType
{
    Folder = 0,
    File
};


class ContentNode : public QObject
{
    Q_OBJECT
public:
    ContentNode(const QString& nodeName, NodeType nodeType, ContentNode *parentItem = nullptr);
    ~ContentNode();

    enum ListRoles{
        NodeNameRole = Qt::UserRole + 1,
        NodeTypeRole,
        PreviewDataRole
    };

    void appendChild(ContentNode *child);

    ContentNode* child(int row);
    ContentNode* childByName(const QString& childName);
    int childCount() const;
    void deleteAllChilds();

    QVariant data(int role) const;
    int row() const;

    ContentNode *parentItem();

    QString nodeName() const;

private:
    QVector<ContentNode*> m_childNodes;

    QString m_nodeName;
    NodeType m_nodeType;
    QList<QVariant> m_previewData;

    ContentNode *m_parentItem;

    void setParent(ContentNode* parent);
};

#endif // CONTENTNODE_H
