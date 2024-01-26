import QtQuick
import QtQuick.Controls

import UiObjects
import Playlist
import Elements
import Content

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

        width: parent.width
        height: parent.height

        anchors.horizontalCenter: parent.horizontalCenter

        currentIndex: _bar.currentIndex
        interactive: false

        PlaylistPage{
            id: _playlistPage
        }

        ContentPage{
            id: _contentPage
        }

        Item{
            Column{
                height: parent.height*0.8
                width: parent.width

                anchors.centerIn: parent

                spacing: height/20

                Rectangle{
                    anchors.horizontalCenter: parent.horizontalCenter

                    width: parent.width*0.75
                    height: parent.height/12

                    color: "transparent"

                    border.width: 2
                    border.color: "white"

                    DefaultText{
                        anchors.centerIn: parent

                        text: "Application ver.: " + Qt.application.version + "\n"
                              + "Firmware ver.: " + FirmwareManager.currentFwVersion
                    }
                }

                Button{
                    id: _btnFirmwareUpdate

                    anchors.horizontalCenter: parent.horizontalCenter

                    width: parent.width*0.75
                    height: parent.height/12

                    text: "Update firmware"

                    onClicked: {
                        FirmwareManager.updateFirmware("firmware.bin");
                    }
                }
            }
        }
    }

    footer: TabBar{
        id: _bar

        height: _main.height/20
        currentIndex: _swipeView.currentIndex

        TabButton {
            height: parent.height
            anchors.top: parent.top
            text: qsTr("Playlist")
            onClicked: {
                _swipeView.currentIndex=0
            }
         }
         TabButton {
             height: parent.height
             anchors.top: parent.top
             text: qsTr("Library")
             onClicked: {
                 _swipeView.currentIndex=1
             }
         }
         TabButton {
             height: parent.height
             anchors.top: parent.top
             text: qsTr("Settings")
             onClicked: {
                 _swipeView.currentIndex=2
             }
         }
    }
}
