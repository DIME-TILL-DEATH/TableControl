import QtQuick
import QtQuick.Controls

TabButton {
    id: control

    anchors.top: parent.top
    height: parent.height

    property string imageSource
    property bool isActive

    text: qsTr("Home")



    contentItem: Column{
        Image
        {
            source: control.imageSource

            width: parent.width
            height: parent.height*0.7

            fillMode: Image.PreserveAspectFit
            // color: control.isActive ? "white" : "grey"
        }

        Text {
            width: parent.width
            height: parent.height*0.3

            font.bold: true
            font.pointSize: 12

            color: control.isActive ? "white" : "grey"

            text: control.text
            horizontalAlignment: Text.AlignHCenter
         }
    }

    background: Rectangle {
        color: "black"
    }
}
