import QtQuick
import QtQuick.Controls

import Elements

Rectangle
{
    id: _root

    property string contentText

    signal clicked()
    signal released()

    implicitWidth: 100
    implicitHeight: 40

    color: _ma.containsPress ? "darkgray" : "lightgrey"

    opacity: enabled ? 1 : 0.3
    border.color: _ma.containsPress ? "green" : "lightgreen"
    border.width: 2
    radius: 5

    DefaultText {
         text: contentText
         opacity: enabled ? 1.0 : 0.3
         color: _ma.containsPress ? "green" : "lightgreen"

         anchors.centerIn: parent

         horizontalAlignment: Text.AlignHCenter
         verticalAlignment: Text.AlignVCenter
         elide: Text.ElideRight
    }

    MouseArea{
        id: _ma

        anchors.fill: parent

        onClicked: {
            _root.clicked()
        }
    }
}
