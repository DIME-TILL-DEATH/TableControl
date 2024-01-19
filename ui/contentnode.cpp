#include "contentnode.h"

ContentNode::ContentNode(const QString& nodeName, NodeType nodeType, ContentNode *parentItem)
    : m_nodeName(nodeName), m_nodeType(nodeType) ,m_parentItem(parentItem)
{

}

ContentNode::~ContentNode()
{
    deleteAllChilds();
}


void ContentNode::deleteAllChilds()
{
    qDeleteAll(m_childNodes);
    m_childNodes.clear();
}

void ContentNode::appendChild(ContentNode *child)
{
    m_childNodes.append(child);
    child->setParent(this);
}

ContentNode *ContentNode::child(int row)
{
    if (row < 0 || row >= m_childNodes.size())
        return nullptr;
    return m_childNodes.at(row);
}

ContentNode *ContentNode::childByName(const QString &childName)
{
    foreach(ContentNode* node, m_childNodes)
    {
        if(node->nodeName() == childName)
        {
            return node;
        }
    }
    return nullptr;
}

int ContentNode::childCount() const
{
    return m_childNodes.count();
}

int ContentNode::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childNodes.indexOf(const_cast<ContentNode*>(this));

    return 0;
}

QVariant ContentNode::data(int role) const
{
    switch(role)
    {
    case ContentNode::ListRoles::NodeNameRole:
    {
        return m_nodeName;
        break;
    }
    case ContentNode::ListRoles::NodeTypeRole:
    {
        //TODO enum class to qml
        if(m_nodeType == NodeType::File) return 1;
        else return 0;
    }
    default: return QVariant();
    }
}

ContentNode *ContentNode::parentItem()
{
    return m_parentItem;
}

QString ContentNode::nodeName() const
{
    return m_nodeName;
}

void ContentNode::setParent(ContentNode *parent)
{
    m_parentItem = parent;
}
