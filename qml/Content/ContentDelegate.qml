import QtQuick
import QtQuick.Controls

import Elements

import UiObjects

Item {
    id: treeDelegate

    implicitWidth: (_treeView.width === 0) ? 30 : _treeView.width
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
                text: model.name

                anchors.verticalCenter: _thing.verticalCenter

                color: (model.type === ContentNode.File) ? "white" : "lightgreen"
            }
        }

        Button{
            id: _button

            width: parent.height
            height: parent.height

            text: (model.type === ContentNode.File) ? ">>" : "+"

            onClicked: function(){
                if(model.type === ContentNode.File)
                {
                    console.log("append to playlist, size: ", model.path + model.name, PlaylistModel.rowCount());
                    PlaylistModel.insert(PlaylistModel.rowCount(), model.path + model.name);
                }

                if(model.type === ContentNode.Folder)
                {
                    DeviceContentModel.currentDstPath = model.path + model.name;
                    DeviceContentModel.selectFile();
                }

                if(model.type === ContentNode.Root)
                {
                    DeviceContentModel.currentDstPath = "/";
                    DeviceContentModel.selectFile();
                }
            }
        }
    }
}
