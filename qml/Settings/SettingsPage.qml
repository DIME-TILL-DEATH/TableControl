import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

import Elements
import UiObjects

Item{
    ProgressScreen{
        id: _progressScreen

        text: ProgressManager.updatingState ? "Updating firmware... Please wait." : "Uploading firmware file..."

        visible: (ProgressManager.firmwareUploadProgress < 1.0) || (ProgressManager.updatingState)
        progress: ProgressManager.firmwareUploadProgress

        anchors.fill: parent
    }

    Column{
        height: parent.height*0.95
        width: parent.width*0.75

        anchors.centerIn: parent

        spacing: height/35

        SettingsHeader{
            text: "Application ver.: " + Qt.application.version + "\n"
                  + "Firmware ver.: " + FirmwareManager.currentFwVersion
        }

        Button{
            id: _btnFirmwareUpdate

            anchors.horizontalCenter: parent.horizontalCenter

            width: parent.width*0.75
            height: parent.height/20

            text: "Update firmware"

            onClicked: {
                FirmwareManager.selectFile();

            }
        }

        SettingsHeader{
            text: "Print speed"
        }

        Slider{
            id: _speedSlider

            width: parent.width

            from: 10
            to: 60
            stepSize: 5

            snapMode: Slider.SnapAlways

            value: Hardware.printSpeed

            onMoved: {
                Hardware.printSpeed = _speedSlider.value
            }
        }

        SettingsHeader{
            text: "LED brightness"
        }

        Slider{
            id: _ledSlider

            width: parent.width

            from: 0
            to: 1
            stepSize: 0.05

            snapMode: Slider.SnapAlways

            value: Hardware.ledBrightness

            onMoved: {
                Hardware.ledBrightness = _ledSlider.value
            }
        }

        SettingsHeader{
            text: "Pause between prints"
        }

        ComboBox{
            id: _comboPause

            width: parent.width*0.5
            height: parent.height/20

            anchors.horizontalCenter: parent.horizontalCenter

            textRole: "key"
            valueRole: "value"

            model: ListModel{
                id: comboModel

                ListElement { key: "10 s"; value: 10000 }
                ListElement { key: "30 s"; value: 30000 }
                ListElement { key: "1 min"; value: 60000 }
                ListElement { key: "5 min"; value: 300000 }
                ListElement { key: "10 min"; value: 600000 }
            }

            onActivated: {
                Hardware.pauseInterval = currentValue
            }
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
            valueChecker: DoubleValidator{locale: "en"; bottom: 0; top: 100.0}
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
    }

    FileDialog{
        id: _firmwareFileDialog

        property string dstPath

        nameFilters: ["Firmware files (*.bin)"]

        onAccepted:
        {
            var cleanPathFile = selectedFile.toString();
            cleanPathFile = (Qt.platform.os==="windows") ? decodeURIComponent(cleanPathFile.replace(/^(file:\/{3})|(qrc:\/{2})|(http:\/{2})/,""))
                                                         : decodeURIComponent(cleanPathFile.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,""));

            FirmwareManager.updateFirmware(cleanPathFile);
        }
    }

    MessageDialog{
        id: _updateStatusDialog
    }

    Connections{
        target: Hardware

        function onPauseIntervalChanged()
        {
            _comboPause.currentIndex = _comboPause.indexOfValue(Hardware.pauseInterval)
        }
    }

    Connections{
        target: FirmwareManager

        function onSgOpenPlatformFileDialog()
        {
            _firmwareFileDialog.open();
        }
    }

    Connections{
        target: ProgressManager

        function oneErorOccured(errorType, data)
        {
            _updateStatusDialog.text = "Firmware updating failed..."
            _updateStatusDialog.open();
        }

        function onFirmwareUpdateComplete()
        {
            _updateStatusDialog.text = "Firmware updating completed. Please reconnect to Wi-Fi and restart application"
            _updateStatusDialog.open();
        }
    }
}
