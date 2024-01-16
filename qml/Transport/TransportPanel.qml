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

            value: TransportCore.progress;

            Text{
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: TransportCore.progress
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
                    TransportCore.previous();
                    TransportCore.requestPlaylist();
                }
            }

            Button{
                width: parent.width/3
                height: parent.height

                text: "P/P"

                onClicked: {
                    TransportCore.pause();
                }
            }

            Button{
                width: parent.width/3
                height: parent.height

                text: "FF"

                onClicked: {
                    TransportCore.next();
                }
            }
        }
    }
}
