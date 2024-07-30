import QtQuick 2.15

import Elements
import UiObjects

Rectangle{

    Column{
        id: _column
        width: parent.width * 0.95
        height: parent.height * 0.95

        anchors.centerIn: parent

        GridView{
            id: _gridView

            width: parent.width
            height: parent.height

            cellWidth: width*0.5
            cellHeight: cellWidth*1.1

            model: LibraryModel

            delegate: Rectangle{
                width: _gridView.cellWidth*0.95
                height: _gridView.cellHeight*0.95

                color: "lightsteelblue"
                border.width: 1
                Column{
                    id: _delegateColumn

                    anchors.centerIn: parent
                    width: parent.width*0.9
                    height: parent.height*0.9

                    Image{
                        id: _coverImage

                        fillMode: Image.PreserveAspectFit
                        source: "file:///" + model.libraryElement.coverPath

                        width: parent.width
                        height: width
                    }

                    Item{
                        width: parent.width
                        height: _delegateColumn.height - _coverImage.height

                        DefaultText{
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: model.libraryElement.name
                        }
                    }
                }

                MouseArea{
                    id: _delegateMa

                    anchors.fill: parent

                    onClicked: {
                        LibraryModel.setGallery(model.libraryElement.systemName);
                    }
                }
            }
        }
    }

}
