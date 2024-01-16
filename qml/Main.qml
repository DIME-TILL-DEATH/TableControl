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


    SwipeView
    {
        id: _swipeView

        width: parent.width
        height: parent.height*0.98

        anchors.horizontalCenter: parent.horizontalCenter

        currentIndex: _bar.currentIndex
        interactive: false

        Column{
            id: _playlistPage
            height: parent.height
            anchors.horizontalCenter: parent.horizontalCenter

            PlaylistWindow{
                height: parent.height - _transport.height //- _preview.height
                width: parent.width
            }

            TransportPanel{
                id: _transport

                height: parent.height * 0.1
                width: parent.width

                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        Column{
            height: parent.height
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Column{
            height: parent.height
            anchors.horizontalCenter: parent.horizontalCenter
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
