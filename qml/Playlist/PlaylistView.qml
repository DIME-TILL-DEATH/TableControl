import QtQuick 2.15

import UiObjects

ListView
{
    id: _listView

    spacing: height/100

    currentIndex: 3
    clip: true

    model: DelegateModel{
        id: _visualModel
        model:PlaylistModel

        delegate: DelegateSwipable{
            width:  _listView.width
            height: _listView.height/6
        }

    }

    //==================Animations=================================
    displaced: Transition {
        NumberAnimation {
            properties: "x,y";
            easing.type: Easing.OutQuad;}
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
