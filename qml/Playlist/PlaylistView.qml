import QtQuick 2.15

import Elements
import UiObjects

ListView
{
    id: _listView

    //spacing: height/100

    boundsBehavior: Flickable.StopAtBounds

    currentIndex: 3
    clip: true

    model: DelegateModel{
        id: _visualModel
        model: PlaylistModel

        delegate: DelegateSwipable{
            width:  _listView.width
            height: _listView.height/6

            Component.onCompleted: {
                showLargePreview.connect(_listView.showLargePreview)
                hideLargePreview.connect(_listView.hideLargePreview)
            }
        }

    }

    // highlight: Rectangle{
    //     border.width: 2
    //     radius: 5
    //     color: "transparent"

    //     z: 5
    // }

    function showLargePreview(dataPoints)
    {
        _largePreview.update(dataPoints);
        _largePreview.visible = true;

        _listView.interactive = false
    }

    function hideLargePreview()
    {
        _largePreview.visible = false;
        _largePreview.update(0);

        _listView.interactive = true
    }

    LargePreview{
        id: _largePreview

        width: parent.width * 0.95
        height: width

        visible: false

        anchors.centerIn: parent
    }

    //==================Animations=================================
    displaced: Transition {
        NumberAnimation {
            properties: "x,y";
            easing.type: Easing.OutQuad;}
    }

    moveDisplaced: Transition {
             NumberAnimation { properties: "y"; duration: 150 }
    }

    remove: Transition{
        PropertyAction {
            property: "ListView.delayRemove"
            value: true
        }
        NumberAnimation {
            property: "height"
            to: 0
            easing.type: Easing.InOutQuad
        }
        PropertyAction {
            property: "ListView.delayRemove"
            value: false
        }
    }
}
