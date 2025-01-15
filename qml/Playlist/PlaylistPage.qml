import QtQuick
import QtQuick.Controls

import Elements

import UiObjects

Rectangle{
    Busy{
        height: parent.height
        width: parent.width

        busy: !Hardware.deviceAvaliable
    }

    Column{
        id: _playlistPage
        width: parent.width
        height: parent.height

        Rectangle{
            id: _plsHeader

            height: parent.height * 0.065
            width: parent.width

            color: "black"

            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                width: parent.width * 0.75
                height: parent.height * 0.95
                spacing: parent.width * 0.1

                RoundIconBtn{
                    id: _btnPlay

                    text: "Play"
                    iconSource: "qrc:/images/play.svg"

                    onClicked: PlaylistModel.changePrint(_listView.currentIndex);
                }

                // RoundIconBtn{
                //     id: _control

                //     text: "Shuffle"

                //     iconSource: "qrc:/images/shuffle.svg"
                // }
            }
        }

        PlaylistView{
            id: _listView

            height: parent.height * 0.975 - _plsHeader.height
            width: parent.width

            currentIndex: 0
        }
    }
}
