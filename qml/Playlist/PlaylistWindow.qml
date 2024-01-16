import QtQuick 2.15

import UiObjects

Rectangle{
    color: "black"

    ListView
    {
        id: _listView
        anchors.fill: parent

        spacing: height/100

        currentIndex: 3
        clip: true

        model: DelegateModel{
            id: _visualModel
            model:PlaylistModel

            delegate: PlaylistDelegate{
                width:  _listView.width
                height: _listView.height/6
            }

        }

        highlight: Rectangle{
            border.width: 4
            border.color: "blue"

            radius: 5

            color: "transparent"

            width: _listView.width
            height: _listView.height/20
        }

        Connections{
            target: PlaylistModel

            function onDataChanged(firstModelIndex, lastModelIndex)
            {
                for(var i = firstModelIndex.row; i<=lastModelIndex.row; i++)
                {
                    if(_listView.itemAtIndex(i) !== null)
                        _listView.itemAtIndex(i).update();
                }
            }
        }

        //==================Animations=================================
        displaced: Transition {
            NumberAnimation { properties: "x,y"; easing.type: Easing.OutQuad;}
        }
    }
}
