import QtQuick
import QtQuick.Controls

import Elements
import UiObjects

DropArea{
    id: _delegateRoot

    property int modelIndex
    property int visualIndex: DelegateModel.itemsIndex

    signal showLargePreview(var dataPoints)
    signal hideLargePreview()

    onEntered: function (drag) {
        var from = drag.source.visualIndex;
        var to = _thing.visualIndex;
        _visualModel.items.move(from, to);
    }

    onDropped: function (drag) {
        var from = modelIndex;
        var to = (drag.source as Item).visualIndex;
        PlaylistModel.move(from, to);
    }

    Item{
        id: _thing

        property Item dragParent: _delegateRoot

        width: _delegateRoot.width
        height: _delegateRoot.height

        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }

        Drag.active: _mouseAreaDrag.drag.active
        Drag.source: _thing
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: height / 2

        property int visualIndex: _delegateRoot.visualIndex

        Row{
            width: parent.width
            height: parent.height

            anchors.verticalCenter: parent.verticalCenter

            spacing: _previewIcon.width/10

            Item{
                width: height
                height: parent.height

                Rectangle{
                    id: _playngPoint

                    color: "yellow"

                    z: _previewIcon.z + 5

                    anchors.centerIn: parent

                    visible: model.playlistElement.isCurrentPrintingElement

                    property int baseWidth: parent.width/5

                    width: baseWidth
                    height: baseWidth

                    radius: baseWidth/2

                    transformOrigin: Item.Center

                    SequentialAnimation on scale{
                        loops: Animation.Infinite
                        running: true

                        NumberAnimation {from: 1; to: 0.5 ; duration: 250}
                        NumberAnimation {from: 0.5; to: 1; duration: 250}
                    }
                }

                Preview
                {
                    id: _previewIcon

                    anchors.fill: parent

                    dataPoints: model.previewData

                    MouseArea{
                        id: _showLargePreviewMA

                        anchors.fill: parent

                        onPressed:
                        {
                            _delegateRoot.showLargePreview(previewData)
                        }

                        onReleased:
                        {
                            _delegateRoot.hideLargePreview()
                        }
                    }
                }
            }

            Item{
                width: parent.width - _previewIcon.width - _itemHandler.width - parent.spacing*2
                height: parent.height*0.5

                anchors.verticalCenter: parent.verticalCenter
                Column{
                    anchors.fill: parent
                    spacing: height/20

                    DefaultText{
                       text: model.playlistElement.fileName

                       color: (model.isFileAvaliable) ? "lightgrey" : "red";
                    }
                    DefaultText{
                       text: model.playlistElement.filePath
                       font.bold: false

                       color: "lightgrey"
                    }
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
                    id: _mouseAreaDrag
                    anchors.fill: parent
                    drag.target: _thing
                    drag.axis: Drag.YAxis

                    onPressed: _delegateRoot.modelIndex = visualIndex


                    onReleased: _thing.Drag.drop()
                }
            }
        }

        Connections{
            target: PlaylistModel

            function onDataChanged(firstModelIndex, lastModelIndex)
            {
                for(var i = firstModelIndex.row; i<=lastModelIndex.row; i++)
                {
                    if(i === _delegateRoot.visualIndex)
                    {
                        //console.log("request update for index:", i);
                        _previewIcon.update();
                    }
                }
            }
        }

        states: State {
                when: _mouseAreaDrag.drag.active
                ParentChange {
                    target: _thing
                    parent: _listView
                }

                AnchorChanges {
                    target: _thing
                    anchors.horizontalCenter: undefined
                    anchors.verticalCenter: undefined
                }
            }
    }
}
