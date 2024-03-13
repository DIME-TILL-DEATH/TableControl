import QtQuick
import QtQuick.Controls

import Elements
import UiObjects

Rectangle{
    id: _settingHeaderRoot

    property string text: "Setting name"

    anchors.horizontalCenter: parent.horizontalCenter

    width: parent.width
    height: parent.height/12

    color: "transparent"

    border.width: 2
    border.color: "white"

    DefaultText{
        anchors.centerIn: parent

        text: _settingHeaderRoot.text
    }
}
