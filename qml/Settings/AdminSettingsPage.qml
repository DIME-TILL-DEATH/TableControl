import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

import Elements
import UiObjects

Rectangle{
    Column{
        height: parent.height*0.95
        width: parent.width*0.75

        anchors.centerIn: parent

        spacing: height/45

        SettingsHeader{
            text: "Serial ID: " + Hardware.serialId
        }

        SettingsHeader{
            text: "Printing properties:"
        }

        SettingTextEdit{
            label: "Scale coefficient: "
            valueChecker: DoubleValidator{locale: "en"; bottom: 0.1; top: 5.0}
            visibleText: Hardware.scaleCoefficient.toFixed(3)

            onValueEntered: function(text){
                Hardware.scaleCoefficient = parseFloat(text);
            }
        }

        SettingTextEdit{
            label: "Print rotation, deg: "
            valueChecker: DoubleValidator{locale: "en"; bottom: 0; top: 360.0}
            visibleText: Hardware.rotation.toFixed(1)

            onValueEntered: function(text){
                Hardware.rotation = parseFloat(text);
            }
        }

        SettingTextEdit{
            label: "R correction, mm: "
            valueChecker: DoubleValidator{locale: "en"; bottom: -50.0; top: 50.0}
            visibleText: Hardware.correction.toFixed(1)

            onValueEntered: function(text){
                Hardware.correction = parseFloat(text);
            }
        }

        Button{
            id: _btnSetPrintProperties

            anchors.horizontalCenter: parent.horizontalCenter

            width: parent.width*0.5
            height: parent.height/20

            text: "Set"

            onClicked: {
                Hardware.setPrintProperties();

            }
        }

        SettingsHeader{
            text: "Fi gear 2 teeths: " + Hardware.fiGear2Teeths
        }

        SettingsHeader{
            text: "Running time: " + (Math.floor(Hardware.machineMinutes/60)) + ":" +  Hardware.machineMinutes%60
        }
    }
}
