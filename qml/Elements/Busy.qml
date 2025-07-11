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

        // BusyIndicator
        // {
        //     id : bI
        //     width: height
        //     height: parent.height*5/10
        //     anchors.horizontalCenter: parent.horizontalCenter
        //     palette.dark: "red"

        //     running: true
        // }

        Image{
            id: _coverImage

            fillMode: Image.PreserveAspectFit
            source: "qrc:/images/figure.png"

            width: parent.width * 0.5
            height: width

            anchors.horizontalCenter: parent.horizontalCenter

            SequentialAnimation{
                running: true
                loops: Animation.Infinite


                NumberAnimation {
                    target: _coverImage
                    property: "scale"
                    duration: 1000
                    to: 0.8
                }

                NumberAnimation {
                    target: _coverImage
                    property: "scale"
                    duration: 1000
                    to: 1.0
                }
            }
        }

        Item{
            width: parent.width
            height: parent.height*1/10
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

                font.pixelSize: height*2.5
                // font.family: ""

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

        }
    }
}
