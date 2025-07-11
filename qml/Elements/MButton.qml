import QtQuick
import QtQuick.Controls

Button {

    id: control
    text: qsTr("Button")

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: control.down ? 0.6 : 1.0
        color: "black"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 40
        opacity: control.down ? 0.6 : 1.0

        // border.color: control.down ? "grey" : "black"
        // border.width: 1
        radius: 5
    }
}
