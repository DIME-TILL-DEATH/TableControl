#ifndef CONTENTNODE_H
#define CONTENTNODE_H

#include <QVariant>

class ContentNode : public QObject
{
    Q_OBJECT
public:
    enum ListRoles{
        NodeNameRole = Qt::UserRole + 1,
        NodeTypeRole,
        NodePathRole,
        PreviewDataRole,
    };

    enum class NodeType
    {
        Root,
        Folder,
        File
    };
    Q_ENUM(NodeType);

    ContentNode(const QString& nodeName, NodeType nodeType, ContentNode *parentItem = nullptr);
    ~ContentNode();

    void appendChild(ContentNode *child);

    ContentNode* child(int row);
    ContentNode* childByName(const QString& childName);
    int childCount() const;
    void deleteAllChilds();

    QVariant data(int role) const;
    int row() const;

    ContentNode *parentItem();

    QString nodeName() const;
    QString nodePath() const;

private:
    QVector<ContentNode*> m_childNodes;

    QString m_nodeName;
    NodeType m_nodeType;
    QList<QVariant> m_previewData;

    ContentNode *m_parentItem;

    void setParent(ContentNode* parent);
};

#endif // CONTENTNODE_H
