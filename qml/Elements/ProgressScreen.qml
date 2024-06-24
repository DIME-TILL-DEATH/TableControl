import QtQuick
import QtQuick.Controls

import UiObjects

Item {
    id: _progressRoot

    z: 10

    property string text: "Uploading file..."
    property real progress: Progress.currentProgress

    visible: (Progress.currentProgress < 1.0)

    Column{
        width: parent.width
        height: parent.height*0.1

        anchors.centerIn: parent

        spacing: height/20

        ProgressBar
        {
            id: progressBar

            anchors.horizontalCenter: parent.horizontalCenter
            value: _progressRoot.progress

            width: parent.width*0.6
            height: parent.height*0.4

            background: Rectangle {
                     implicitWidth: 200
                     implicitHeight: 6
                     radius: 3

                     border.width: 1
                 }

             contentItem: Item {
                 implicitWidth: 200
                 implicitHeight: 4

                 Rectangle {
                     width: progressBar.visualPosition * parent.width
                     height: parent.height
                     radius: 2
                     color: "blue"
                     border.width: 1
                }
            }
        }

        DefaultText{
            text: _progressRoot.text
            anchors.horizontalCenter: parent.horizontalCenter

            color: "black"
        }


    }

    Rectangle{
        anchors.fill: parent
        opacity: 0.6
        z:-2

        color: "wheat"
    }

    MouseArea
    {
        //enabled: _progressRoot.visible
        anchors.fill: parent

        width: _progressRoot.width
        height: _progressRoot.height

        onClicked: {
        }
    }
}
