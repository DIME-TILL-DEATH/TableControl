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

        SettingsPage{
            id: _settingsPage
        }

        AdminSettingsPage{
            id: _adminSettingsPage
        }
    }

    footer: TabBar{
        id: _bar

        height: _main.height/20
        currentIndex: _swipeView.currentIndex

        background: Rectangle {
                 color: "#eeeeee"
             }

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
         TabButton {
             height: parent.height
             anchors.top: parent.top
             text: qsTr("Admin")
             onClicked: {
                 _swipeView.currentIndex=3
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
        target: FirmwareManager

        function onSgFirmwareVersionInsufficient()
        {
            _dialog.open();
            _swipeView.currentIndex=2
        }
    }
}
