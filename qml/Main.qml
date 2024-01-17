import QtQuick
import QtQuick.Controls

import UiObjects
import Transport
import Playlist
import Elements

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

//                TransportPanel{
//                    id: _transport

//                    height: parent.height * 0.1
//                    width: parent.width

//                    anchors.horizontalCenter: parent.horizontalCenter
//                }
            }
        }

        Item{
            Column{
                height: parent.height
                width: parent.width
            }
        }

        Item{
            Column{
                height: parent.height
                width: parent.width
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
