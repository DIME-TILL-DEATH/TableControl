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

        Rectangle{
            visible: control.isActive

            width: control.width * 0.9
            height: 2

            anchors.horizontalCenter: parent.horizontalCenter

            gradient: Gradient{
                orientation: Gradient.Horizontal

                GradientStop { position: 0.0; color: "purple" }
                // GradientStop { position: 0.33; color: "yellow" }
                GradientStop { position: 1.0; color: "orange" }
            }
        }
    }

    background: Rectangle {
        color: "#202020"
    }
}
