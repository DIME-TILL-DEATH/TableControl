import QtQuick 2.15
import QtQuick.Controls 2.15

import Elements
import UiObjects

CustomDialog{
    id: _root

    property alias headerText: _titleLabel.text

    property var choosedElement

    header: Rectangle{
               id: _title

               width: _root.width
               height: _root.height * 0.125

               color: "black"

               Rectangle{
                   id: _headerDivider
                   width: parent.width
                   height: 1
                   anchors.bottom: _title.bottom
                   color: "black"
               }

               Column{
                   width: parent.width*0.4
                   height: parent.height*0.9

                   anchors.centerIn: parent

                   spacing: parent.height*0.05

                   DefaultText{
                       id: _titleLabel

                       text: "odnfbison"
                       height: parent.height*0.4

                       color: "white"

                       font.pointSize: 16

                       anchors.horizontalCenter: parent.horizontalCenter
                   }

                   RoundIconBtn{
                       id: _btnPlay

                       width: parent.width
                       height: parent.height*0.55

                       text: "Play"
                       iconSource: "qrc:/images/play.svg"

                       onClicked: {
                            LibraryModel.setGallery(_root.choosedElement);
                       }
                   }
               }
        }

    footer: Rectangle {
        id: _dialogButtonBox

        width: _root.width
        height: _root.height*0.075
        color: "black"

        border.color: "black"
        border.width: 1

        RoundIconBtn{
            text: "BACK"

            width: parent.width*0.4
            height: parent.height*0.9

            anchors.centerIn: parent

            onClicked: {
                _root.close()
            }
        }
    }

    ListView{
        id: _listView

        width: parent.width
        height: parent.height

        boundsBehavior: Flickable.StopAtBounds
        clip: true

        model: PreviewPlaylistModel

        delegate: Item{
            id: _delegate

            width: _listView.width
            height: _listView.height/8

            Row{
                width: parent.width
                height: parent.height

                anchors.verticalCenter: parent.verticalCenter

                spacing: _previewIcon.width/10

                Preview
                {
                    id: _previewIcon

                    width: height
                    height: parent.height

                    dataPoints: model.previewData

                    MouseArea{
                        id: _showLargePreviewMA

                        anchors.fill: parent

                        onPressed:
                        {
                            // _delegateRoot.showLargePreview(previewData)

                            _largePreview.update(model.previewData);
                            _largePreview.visible = true;

                            _listView.interactive = false
                        }

                        onReleased:
                        {
                            // _delegateRoot.hideLargePreview()

                            _largePreview.visible = false;
                            _largePreview.update(0);

                            _listView.interactive = true
                        }
                    }

                    Connections{
                        target: PreviewPlaylistModel

                        function onDataChanged(firstModelIndex, lastModelIndex)
                        {

                            for(var i = firstModelIndex.row; i<=lastModelIndex.row; i++)
                            {
                                if(i === index)
                                {
                                    _previewIcon.update();
                                }
                            }
                        }
                    }
                }


                Item{
                    width: parent.width - _previewIcon.width - parent.spacing*2
                    height: parent.height*0.5

                    anchors.verticalCenter: parent.verticalCenter

                    DefaultText{
                       text: model.playlistElement.fileName

                       color: (model.isFileAvaliable) ? "black" : "red";
                    }
                }

            }
        }
    }

    LargePreview{
        id: _largePreview

        width: parent.width * 0.95
        height: width

        visible: false

        anchors.centerIn: parent
    }
}
