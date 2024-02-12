import QtQuick 2.15
import QtQuick.Controls 2.15

import UiObjects

Rectangle {

    color: "gray"

    Column{
        anchors.fill: parent
        ProgressBar{
            id: _progressBar

            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width * 0.95

            value: Transport.progress;

            Text{
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: Transport.progress
            }
        }

        Row{
            width: parent.width
            height: parent.height - _progressBar.height

            Button{
                width: parent.width/3
                height: parent.height

                text: "RR"

                onClicked: {

                }
            }

            Button{
                width: parent.width/3
                height: parent.height

                text: "P/P"

                onClicked: {
                    Transport.pause();
                }
            }

            Button{
                width: parent.width/3
                height: parent.height

                text: "FF"

                onClicked: {

                }
            }
        }
    }
}
