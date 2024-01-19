import QtQuick
import QtQuick.Controls

import Elements

import UiObjects

Item{
    Busy{
        height: parent.height
        width: parent.width
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

            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            height: parent.height * 0.025

            value: TransportCore.progress;

            Text{
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                text: TransportCore.progress
            }
        }
    }
}
