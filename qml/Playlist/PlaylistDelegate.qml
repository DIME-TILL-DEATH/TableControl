import QtQuick 2.15

import Elements
import UiObjects

DropArea{
    id: _delegateRoot

    width: 80; height: 80

    property int modelIndex
    property int visualIndex: DelegateModel.itemsIndex

    function update()
    {
        _previewIcon.update();
    }

    onEntered: function (drag) {
        var from = (drag.source as Item).visualIndex;
        var to = _thing.visualIndex;
        _visualModel.items.move(from, to);
    }

    onDropped: function (drag) {
        var from = modelIndex;
        var to = (drag.source as Item).visualIndex;
        PlaylistModel.move(from, to);
    }

    Rectangle{
        id: _thing

        property Item dragParent: _delegateRoot

        z: -5

        color: "transparent"

        SequentialAnimation on color{
            id: _colorAnim

            running: (fileName === PlaylistModel.curPrintFileName)
            loops: Animation.Infinite
            alwaysRunToEnd: true
            ColorAnimation {from: "black"; to: "slategray"; duration: 2000}
            ColorAnimation {from: "slategray"; to: "black"; duration: 2000}
        }

        width: _delegateRoot.width
        height: _delegateRoot.height

        Drag.active: mouseArea.drag.active
        Drag.source: _thing
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: height / 2

        property int visualIndex: _delegateRoot.visualIndex

        Row{
            width: parent.width
            height: parent.height

            spacing: _previewIcon.width/10

            Preview
            {
                id: _previewIcon
                width: height
                height: parent.height

                dataPoints: previewData
            }

            Column{
                width: parent.width - _previewIcon.width - _itemHandler.width - parent.spacing*2
                height: parent.height*0.5

                anchors.verticalCenter: parent.verticalCenter

                spacing: height/20

                DefaultText{
                   text: fileName
                }
                DefaultText{
                   text: "default"
                   font.bold: false
                }
            }

            Item{
                id: _itemHandler
                width: height
                height: parent.height*0.5

                anchors.verticalCenter: parent.verticalCenter

                DefaultText{
                    anchors.centerIn: parent
                    font.pointSize: 25
                    text: "⋮"
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    drag.target: _thing
                    onClicked: {
                        _listView.currentIndex = index
                    }

                    onPressed: {
                        _delegateRoot.modelIndex = visualIndex
                    }

                    onReleased: {
                        _thing.Drag.drop()
                    }
                }
            }
        }


        states: [
            State {
                when: mouseArea.drag.active
                ParentChange {
                    target: _thing
                    parent: _thing.dragParent
                }

                AnchorChanges {
                    target: _thing
                    anchors.horizontalCenter: undefined
                    anchors.verticalCenter: undefined
                }
            }
        ]
    }
}
