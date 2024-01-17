import QtQuick


Rectangle {
    id: _root

    border.color: "red"
    border.width: 3

    color: "blue"

//    property var dataPoints

    function update(data)
    {
        _basicPreview.dataPoints = data;
        _basicPreview.update();
    }

    radius: width/2

    Preview{
        id: _basicPreview

        dataPoints: _root.dataPoints
        lineWidth: 1
    }
}
