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
        color: "black"
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
        id: _mainRec

        color: "transparent"//model.playlistElement.isCurrentPrintingElement ? "lightgray" : "transparent"
    }

    onClicked: {
        _listView.currentIndex = model.index
    }

    onDoubleClicked: {
        PlaylistModel.changePrint(model.index);
    }
}
