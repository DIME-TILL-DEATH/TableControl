import QtQuick
import QtQuick.Controls

import UiObjects

SwipeDelegate {
    id: swipeDelegate

    signal showLargePreview(var dataPoints)
    signal hideLargePreview()

    contentItem: DelegateItem{
        width: swipeDelegate.width
        height: swipeDelegate.height

        Component.onCompleted: {
            showLargePreview.connect(swipeDelegate.showLargePreview)
            hideLargePreview.connect(swipeDelegate.hideLargePreview)
        }
    }


    swipe.right: Label {
        id: deleteLabel
        text: qsTr("Delete")
        color: "white"
        verticalAlignment: Label.AlignVCenter
        padding: 12
        height: parent.height
        anchors.right: parent.right

        SwipeDelegate.onClicked: PlaylistModel.remove(index)

        background: Rectangle {
            color: deleteLabel.SwipeDelegate.pressed ? Qt.darker("tomato", 1.1) : "tomato"
        }
    }

    background: Rectangle{
        //color: "black"
        id: _mainRec

        property color startColor: "black"
        property color stopColor: model.playlistElement.isCurrentPrintingElement ? "slategray" : "black"

        color: startColor

        SequentialAnimation on color{
            id: _colorAnim

            running: model.playlistElement.isCurrentPrintingElement
            loops: Animation.Infinite
            alwaysRunToEnd: true
            ColorAnimation {from: _mainRec.startColor; to:  _mainRec.stopColor ; duration: 2000}
            ColorAnimation {from: _mainRec.stopColor; to: _mainRec.startColor; duration: 2000}
        }
    }

    onDoubleClicked: {
        PlaylistModel.changePrint(model.index);
    }
}
