import QtQuick 2.15
import Elements
import UiObjects

Rectangle{
    color: "lightsteelblue"
    border.width: 1

    radius: width/20

    signal delegateClicked(int index)

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
            delegateClicked(index)
        }
    }
}
