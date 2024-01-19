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
