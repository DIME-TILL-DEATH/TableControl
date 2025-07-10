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

    gradient: Gradient{
        GradientStop { position: 0.0; color: "lightgrey" }
        GradientStop { position: 1.0; color: "#303030" }
    }

    Column{
        id: _playlistPage
        width: parent.width
        height: parent.height

        Item{
            id: _plsHeader

            height: parent.height * 0.15
            width: parent.width

            // color: "black"

            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                width: parent.width * 0.75
                height: parent.height * 0.95
                spacing: parent.width * 0.1

                Image{
                    id: _icon
                    source: "qrc:/images/icon_pls_item.svg"

                    width: parent.height
                    height: width

                    fillMode: Image.PreserveAspectFit

                    // color: _control.down ? "grey" : "white"
                }

                Item{

                    width: parent.width - _icon.width
                    height: parent.height

                    Text{
                        text: "Kinetic control"

                        color: "black"
                        font.pointSize: 20
                        font.bold: true

                        anchors.centerIn: parent

                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                // RoundIconBtn{
                //     id: _btnPlay

                //     text: "Play"
                //     iconSource: "qrc:/images/play.svg"

                //     onClicked: PlaylistModel.changePrint(_listView.currentIndex);
                // }

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
