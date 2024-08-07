import QtQuick 2.15
import QtQuick.Controls 2.15

import Elements
import UiObjects

Rectangle{
    id: _libraryPage

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

            delegate: LibraryItem{
                width: _gridView.cellWidth*0.95
                height: _gridView.cellHeight*0.95

                onDelegateClicked: function click(delegateIndex){
                    LibraryModel.setPreviewPlaylist(delegateIndex);

                    elementPreviewWindow.choosedElement = model.libraryElement.systemName
                    elementPreviewWindow.headerText = model.libraryElement.name
                    elementPreviewWindow.open();
                }
            }
        }
    }

    ElementPreviewWindow{
        id: elementPreviewWindow

        width: _libraryPage.width*0.9
        height: _libraryPage.height*0.95
    }

}
