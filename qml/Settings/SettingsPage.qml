import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

import Elements
import UiObjects

Rectangle{
    gradient: Gradient{
        GradientStop { position: 0.0; color: "lightgrey" }
        GradientStop { position: 1.0; color: "#303030" }
    }

    ProgressScreen{
        id: _progressScreen

        text: Progress.updatingState ? "Updating firmware... Please wait." : "Uploading firmware file..."

        visible: (Progress.firmwareUploadProgress < 1.0) || (Progress.updatingState)
        progress: Progress.firmwareUploadProgress

        anchors.fill: parent
    }

    Column{
        height: parent.height*0.95
        width: parent.width*0.75

        anchors.centerIn: parent

        spacing: height/45

        SettingsHeader{
            text: "Application ver.: " + Qt.application.version + "\n"
                  + "Firmware ver.: " + Firmware.currentFwVersion
        }

        MButton{
            id: _btnFirmwareUpdate

            anchors.horizontalCenter: parent.horizontalCenter

            width: parent.width*0.5
            height: parent.height/20

            text: "Update firmware"

            onClicked: {
                Firmware.selectFile();

            }
        }

        SettingsHeader{
            text: "Print speed"
        }

        MSlider{
            id: _speedSlider

            width: parent.width
            height: parent.height/17

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

        MSlider{
            id: _ledSlider

            width: parent.width
            height: parent.height/17

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
            height: parent.height/18

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

            Firmware.updateFirmware(cleanPathFile);
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
        target: Firmware

        function onSgOpenPlatformFileDialog()
        {
            _firmwareFileDialog.open();
        }
    }

    Connections{
        target: Progress

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
