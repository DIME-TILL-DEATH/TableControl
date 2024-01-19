import QtQuick
import QtQuick.Controls

import Elements

import UiObjects

Item {
    id: treeDelegate

    implicitWidth: _treeView.width//padding + label.x + label.implicitWidth + padding + _button.width
    implicitHeight: label.implicitHeight * 2

    readonly property real indent: 20
    readonly property real padding: 5

    // Assigned to by TreeView:
    required property TreeView treeView
    required property bool isTreeNode
    required property bool expanded
    required property int hasChildren
    required property int depth

    Row{
        id: _content

        anchors.fill: parent

        Item{
            id: _thing

            width: parent.width - _button.width
            height: parent.height

            TapHandler {
                onTapped: treeView.toggleExpanded(row)
            }

            DefaultText {
                id: indicator
                visible: treeDelegate.isTreeNode && treeDelegate.hasChildren
                x: padding + (treeDelegate.depth * treeDelegate.indent)
                anchors.verticalCenter: label.verticalCenter
                text: "▸"
                rotation: treeDelegate.expanded ? 90 : 0
            }

            DefaultText {
                id: label
                x: padding + (treeDelegate.isTreeNode ? (treeDelegate.depth + 1) * treeDelegate.indent : 0)
                width: treeDelegate.width - treeDelegate.padding - x
                clip: true
                text: model.path

                anchors.verticalCenter: _thing.verticalCenter

                color: (model.nodeType) ? "white" : "lightgreen"
            }
        }

        Button{
            id: _button

            width: parent.height
            height: parent.height

            text: (model.nodeType) ? ">>" : "+"

            onClicked: function(){
                if(model.nodeType)
                {
                    console.log("append to playlist, size: ", PlaylistModel.rowCount());
                    PlaylistModel.insert(PlaylistModel.rowCount(), model.path);
                }
            }
        }
    }
}
