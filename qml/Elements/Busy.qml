import QtQuick
import QtQuick.Controls

import UiObjects

Item
{
    id : rWait

    property bool busy: !PlaylistModel.deviceAvaliable

    property string extText: "Scanning for device"

    visible: busy
    anchors.centerIn: parent

    Column
    {
        width: parent.width
        height: parent.height/2

        anchors.verticalCenter: parent.verticalCenter

        spacing: bI.height/20

        BusyIndicator
        {
            id : bI
            width: height
            height: parent.height*5/10
            anchors.horizontalCenter: parent.horizontalCenter
            palette.dark: "red"

            running: true
        }

        Item
        {
            width: parent.width
            height: parent.height*1/10
            anchors.horizontalCenter: parent.horizontalCenter
            DefaultText
            {
                text: extText
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    Rectangle{
        anchors.fill: parent

        opacity: 0.4
        z:-2
    }

    MouseArea
    {
        id : mWait
        enabled: busy
        anchors.fill: parent

        onPressed: {
            console.log("clc")
        }
    }
}
