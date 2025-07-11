import QtQuick


Rectangle {
    id: _root

    border.color: "black"
    border.width: 3

    color: "white"

//    property var dataPoints

    function update(data)
    {
        _basicPreview.dataPoints = data;
        _basicPreview.update();
    }

    radius: width/2

    Preview{
        id: _basicPreview

        lineColor: "black"

        dataPoints: _root.dataPoints
        lineWidth: 1
    }
}
