import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

import QtQuick.Layouts
import QtQuick.Window

Dialog
{
    id: _root

    property alias text: _text.text
    property alias wrapMode: _text.wrapMode

    property alias headerText: _titleLabel.text

    property alias buttons: _dialogButtonBox.standardButtons

    modal: true
    closePolicy: Popup.NoAutoClose

    width: parent.width * 0.9
    height: (_text.textHeight+3)*_text.font.pixelSize*0.6
            +_title.height+_dialogButtonBox.height

    property int parentHeight: parent.height*0.34
    property int headerHeight: parentHeight*0.2
    property int footerHeight: parentHeight*0.25

    property bool closeOnDisconnect: false

    anchors.centerIn: Overlay.overlay
//    anchors.centerIn: parent

    background: Rectangle {
       id: _backRect

        width: _root.width
        height: _root.height

        color: "white"

        radius: _root.width/200
        border.color: "black"
        border.width: 1
    }

    header: Rectangle{
               id: _title

               width: _root.width
               height: _root.height * 0.075

               color: "black"

               Rectangle{
                   id: _headerDivider
                   width: parent.width
                   height: 1
                   anchors.bottom: _title.bottom
                   color: "black"
               }


               DefaultText{
                   id: _titleLabel

                   color: "white"

                   font.pointSize: 16

                   anchors.horizontalCenter: parent.horizontalCenter
                   anchors.verticalCenter: parent.verticalCenter
               }
        }


    contentItem:  Item {
           id: _textLabel

           width: _root.width
           height: _root.height-headerHeight-footerHeight

           DefaultText{
               id: _text

               wrapMode: Text.WordWrap
               width: parent.width

               padding: 5

               anchors.verticalCenter: parent.verticalCenter
           }
    }

    footer: DialogButtonBox
    {
        id: _dialogButtonBox

        width: _root.width
        height: _title.height*1.5 //footerHeight

        alignment: Qt.AlignHCenter

        background: Rectangle{
            color: "black"
            radius: _backRect.radius
            border.color: "black"
            border.width: 1
        }
    }
}
