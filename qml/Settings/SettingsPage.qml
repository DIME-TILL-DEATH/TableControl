import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

import Elements
import UiObjects

Item{
    ProgressScreen{
        id: _progressScreen

        text: ProgressManager.updatingAndReboot ? "Updating and rebooting" : "Uploading firmware file..."

        visible: (ProgressManager.firmwareUploadProgress < 1.0) || (ProgressManager.updatingAndReboot)
        progress: ProgressManager.firmwareUploadProgress

        anchors.fill: parent
    }

    Column{
        height: parent.height*0.8
        width: parent.width

        anchors.centerIn: parent

        spacing: height/20

        Rectangle{
            anchors.horizontalCenter: parent.horizontalCenter

            width: parent.width*0.75
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

            width: parent.width*0.75
            height: parent.height/12

            text: "Update firmware"

            onClicked: {
                _firmwareFileDialog.open();

            }
        }
    }

    FileDialog{
        id: _firmwareFileDialog

        property string dstPath

        //nameFilters: ["Firmware files (*.bin)"]

        onAccepted:
        {
            var cleanPathFile = selectedFile.toString();
            cleanPathFile = (Qt.platform.os==="windows") ? decodeURIComponent(cleanPathFile.replace(/^(file:\/{3})|(qrc:\/{2})|(http:\/{2})/,""))
                                                         : decodeURIComponent(cleanPathFile.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,""));

            FirmwareManager.updateFirmware(cleanPathFile);
        }
    }
}
