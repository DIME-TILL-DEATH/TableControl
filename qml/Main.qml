import QtQuick
import QtQuick.Controls

import UiObjects

import Playlist
import Elements
import Content
import Settings

ApplicationWindow {
    id: _main

    width: 400
    height: 900
    visible: true
    title: qsTr("Kinetic table control")

    Rectangle
    {
        anchors.fill: parent
        color: "black"
    }

    SwipeView
    {
        id: _swipeView

        width: parent.width * 0.95
        height: parent.height

        spacing: parent.width * 0.025

        anchors.horizontalCenter: parent.horizontalCenter

        currentIndex: _bar.currentIndex
        interactive: false

        PlaylistPage{
            id: _playlistPage
        }

        ContentPage{
            id: _contentPage
        }

        SettingsPage{
            id: _settingsPage
        }

        AdminSettingsPage{
            id: _adminSettingsPage
        }
    }

    footer: Rectangle
    {
        width: _main.width
        height: _main.height*0.125
        color: "black"

        Column{
            anchors.fill: parent

            Row{
                width: parent.width*0.8
                height: parent.height*0.4

                anchors.horizontalCenter: parent.horizontalCenter

                Column{
                    width: parent.width - _pauseResumeBtn.width
                    height: parent.height

                    anchors.verticalCenter: parent.verticalCenter

                    DefaultText{
                        height: parent.height * 0.5

                        text: "Playing from " + PlaylistModel.playlistName + " playlist"

                        color: "white"
                        font.pointSize: 8
                    }
                    DefaultText{
                        height: parent.height * 0.5

                        text: PlaylistModel.printName

                        color: "white"
                        font.pointSize: 12
                    }
                }

                IconImage{
                    id: _pauseResumeBtn
                    source: "qrc:/images/play-pause.svg"

                    width: parent.height * 0.95
                    height: width

                    fillMode: Image.PreserveAspectFit

                    color: _maPauseResume.containsPress ? "red" : "white"
                    MouseArea{
                        id: _maPauseResume

                        anchors.fill: parent

                        onClicked: {
                            Hardware.pause();
                        }
                    }
                }

            }


            ProgressBar{
                id: _progressBar

                visible: Hardware.deviceAvaliable

                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width
                height: parent.height * 0.05

                value: Hardware.progress;

                // Text{
                //     anchors.horizontalCenter: parent.horizontalCenter
                //     anchors.verticalCenter: parent.verticalCenter
                //     text: (Hardware.progress * 100).toFixed(2) + "%"

                //     z: 2
                // }

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

            TabBar{
                id: _bar

                width: parent.width
                height: parent.height*0.55
                currentIndex: _swipeView.currentIndex

                background: Rectangle {
                         color: "#eeeeee"
                }

                FooterMenuBtn{
                    text: qsTr("Home")

                    imageSource: "qrc:/images/home1.svg"
                    isActive: TabBar.index === _bar.currentIndex

                    onClicked: {
                        _swipeView.currentIndex=0
                    }
                }

                FooterMenuBtn {
                    imageSource: "qrc:/images/library.svg"
                    isActive: TabBar.index === _bar.currentIndex

                    text: qsTr("Library")
                    onClicked: {
                        _swipeView.currentIndex=1
                    }
                }

                FooterMenuBtn {
                    imageSource: "qrc:/images/settings1.svg"
                    isActive: TabBar.index === _bar.currentIndex

                    text: qsTr("Settings")
                    onClicked: {
                        _swipeView.currentIndex=2
                    }
                }

                FooterMenuBtn {
                    imageSource: "qrc:/images/key.svg"
                    isActive: TabBar.index === _bar.currentIndex

                    text: qsTr("Admin")
                    onClicked: {
                        _swipeView.currentIndex=3
                    }
                }
            }
        }
    }

    Dialog{
        id: _dialog

        title: "Error"

        anchors.centerIn: parent

        standardButtons: Dialog.Ok

        Label {
            text: "Firmware version insufficient!\nPlease, update firmware"
        }
    }

    Connections{
        target: Firmware

        function onSgFirmwareVersionInsufficient()
        {
            _dialog.open();
            _swipeView.currentIndex=2
        }
    }
}
