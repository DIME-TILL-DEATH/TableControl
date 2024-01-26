import QtQuick
import QtQuick.Controls

import UiObjects

Item {
    id: _progressRoot

    z: 10

    visible: (ProgressManager.currentProgress < 1.0)

    Column{
        width: parent.width
        height: parent.height*0.1

        anchors.centerIn: parent

        spacing: height/20

        ProgressBar
        {
            id: progressBar

            anchors.horizontalCenter: parent.horizontalCenter
            value: ProgressManager.currentProgress

            width: parent.width*0.6
            height: parent.height*0.4

        }

        DefaultText{
            text: "Uploading file..."
            anchors.horizontalCenter: parent.horizontalCenter

            color: "blue"
        }


    }

    Rectangle{
        anchors.fill: parent
        opacity: 0.4
        z:-2
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
