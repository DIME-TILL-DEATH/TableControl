import QtQuick
import QtQuick.Controls

Button{
    id: _control

    width: parent.width * 0.45
    height: parent.height

    text: "Shuffle"

    focusPolicy: Qt.NoFocus

    property string iconSource

    contentItem: Row{

        spacing: width/15
        IconImage{
            id: _icon
            source: _control.iconSource

            width: parent.height
            height: width

            fillMode: Image.PreserveAspectFit

            color: _control.down ? "grey" : "white"
        }

        DefaultText {
            height: parent.height

            text: _control.text
            color: _control.down ? "grey" : "white"

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }

    background: Rectangle {
        color: "black"
        border.color: _control.down ? "grey" : "white"
        border.width: 2
        radius: _control.width / 10
    }
}
