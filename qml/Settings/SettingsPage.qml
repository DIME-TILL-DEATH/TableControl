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
        height: parent.height*0.8
        width: parent.width*0.75

        anchors.centerIn: parent

        spacing: height/20

        Rectangle{
            anchors.horizontalCenter: parent.horizontalCenter

            width: parent.width
            height: parent.height/12

            color: "transparent"

            border.width: 2
            border.color: "white"

            DefaultText{
                anchors.centerIn: parent

                text: "Print speed"
            }
        }

        Slider{
            width: parent.width

            from: 10
            to: 60
            stepSize: 5

            snapMode: Slider.SnapAlways

            value: Transport.printSpeed

            onMoved: {
                Transport.printSpeed = value
            }
        }


        Rectangle{
            anchors.horizontalCenter: parent.horizontalCenter

            width: parent.width
            height: parent.height/12

            color: "transparent"

            border.width: 2
            border.color: "white"

            DefaultText{
                anchors.centerIn: parent

                text: "Application ver.: " + Qt.application.version + "\n"
                      + "Firmware ver.: " + FirmwareManager.currentFwVersion
            }
        }

        Button{
            id: _btnFirmwareUpdate

            anchors.horizontalCenter: parent.horizontalCenter

            width: parent.width
            height: parent.height/12

            text: "Update firmware"

            onClicked: {
                FirmwareManager.selectFile();

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
