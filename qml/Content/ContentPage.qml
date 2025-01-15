import QtQuick
import QtQuick.Controls

import QtQuick.Dialogs

import Elements

import UiObjects

Item{
    id: _contentPage

    // ProgressScreen{
    //     id: _progressScreen

    //     anchors.fill: parent
    // }

    TreeView{
        id: _treeView

        width: parent.width
        height: parent.height

        model: DeviceContentModel

        boundsBehavior: Flickable.StopAtBounds

        clip: true

        delegate: ContentDelegate{
        }
    }



    FileDialog{
        id: _fileDialog

        property string dstPath: DeviceContentModel.currentDstPath

        nameFilters: ["GCode files (*.gcode)"]

        onAccepted:
        {
            var cleanPathFile = selectedFile.toString();
            cleanPathFile = (Qt.platform.os==="windows") ? decodeURIComponent(cleanPathFile.replace(/^(file:\/{3})|(qrc:\/{2})|(http:\/{2})/,""))
                                                         : decodeURIComponent(cleanPathFile.replace(/^(file:\/{2})|(qrc:\/{2})|(http:\/{2})/,""));

            var cleanPathFolder = currentFolder.toString();
            var fileName = selectedFile.toString().replace(cleanPathFolder + "/", "");
            DeviceContentModel.uploadFileToDevice(dstPath + "/" + fileName, cleanPathFile);
        }
    }

    Connections{
        target: DeviceContentModel

        function onSgOpenPlatformFileDialog(dstPath)
        {
            _fileDialog.open();
        }
    }
}
