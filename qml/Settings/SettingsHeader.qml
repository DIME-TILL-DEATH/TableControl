import QtQuick
import QtQuick.Controls

import Elements
import UiObjects

Rectangle{
    id: _settingHeaderRoot

    property string text: "Setting name"

    anchors.horizontalCenter: parent.horizontalCenter

    width: parent.width
    height: parent.height/14
    radius: parent.width/50

    color: "transparent"

    border.width: 2
    border.color: "black"

    DefaultText{
        anchors.centerIn: parent

        text: _settingHeaderRoot.text
    }
}
