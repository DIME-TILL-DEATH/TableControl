import QtQuick
import QtQuick.Controls

import UiObjects

SwipeDelegate {
    id: swipeDelegate

    signal showLargePreview(var dataPoints)
    signal hideLargePreview()

    contentItem: DelegateContent{
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
        color: "black"

        SequentialAnimation on color{
            id: _colorAnim

            running: (fileName === PlaylistModel.curPrintFileName)
            loops: Animation.Infinite
            alwaysRunToEnd: true
            ColorAnimation {from: "black"; to: "slategray"; duration: 2000}
            ColorAnimation {from: "slategray"; to: "black"; duration: 2000}
        }
    }
}
