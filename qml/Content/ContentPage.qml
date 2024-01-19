import QtQuick
import QtQuick.Controls

import Elements

import UiObjects

Item{
    TreeView{
        id: _treeView

        width: parent.width
        height: parent.height

        model: DeviceContentModel

        boundsBehavior: Flickable.StopAtBounds

        clip: true

        delegate: ContentDelegate{

        }
    }
}
