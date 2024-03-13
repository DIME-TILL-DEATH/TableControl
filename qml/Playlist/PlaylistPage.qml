import QtQuick
import QtQuick.Controls

import Elements

import UiObjects

Item{
    Busy{
        height: parent.height
        width: parent.width

        busy: !PlaylistModel.deviceAvaliable
    }

    Column{
        id: _playlistPage
        width: parent.width
        height: parent.height

        PlaylistView{
            height: parent.height * 0.975// - _transport.height //- _preview.height
            width: parent.width
        }

        ProgressBar{
            id: _progressBar

            visible: PlaylistModel.deviceAvaliable

            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            height: parent.height * 0.025

            value: Hardware.progress;

            Text{
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: (Hardware.progress * 100).toFixed(2) + "%"

                z: 2
            }

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
                     width: _progressBar.visualPosition * parent.width
                     height: parent.height
                     radius: 2
                     z:-2

                     color: "blue"
                     border.width: 1
                }
            }
        }
    }
}
