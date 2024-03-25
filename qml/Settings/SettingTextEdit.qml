import QtQuick
import QtQuick.Controls

import Elements

Row{
    property QtObject valueChecker: DoubleValidator{locale: "en"}
    property alias visibleText: _textField.text
    property string label: "Setting"

    width: parent.width
    height: parent.height/22
    spacing: width/20

    anchors.horizontalCenter: parent.horizontalCenter

    signal valueEntered(var text)

    DefaultText{
        text: label
        anchors.verticalCenter: parent.verticalCenter
    }

    TextField{
        id: _textField

        height: parent.height
        width: parent.width/4
        anchors.verticalCenter: parent.verticalCenter

        horizontalAlignment: TextInput.AlignHCenter

        text: "1.0"
        font.bold: true
        font.pointSize: 14
        color: "white"

        validator: valueChecker


        selectByMouse: true
        inputMethodHints: Qt.ImhPreferNumbers

        background: Rectangle {
                 id: _backgroundRectangle

                 implicitWidth: _textField.font.pointSize * 15
                 implicitHeight: _textField.font.pointSize

                 color: "transparent"

                 border.color: _textField.activeFocus ? "blue" : "white"
             }


        onEditingFinished: {
            valueEntered(_textField.text)
            _textField.focus = false
        }

        onTextEdited: {
            _backgroundRectangle.color = (parseFloat(text) <= _textField.validator.top &&
                                          parseFloat(text) >= _textField.validator.bottom) ? "transparent" : "red"
        }
    }
}
