import QtQuick
import QtQuick.Controls

import UiObjects

Item{
    width: parent.width
    height: width

    property var dataPoints
    property real lineWidth: 0.25

    function update()
    {
        _canvas.requestPaint();
    }

    Column{
        id: _loading

        anchors.fill: parent

        visible: (dataPoints.length === 0)

        BusyIndicator{
            anchors.horizontalCenter: parent.horizontalCenter

            height: parent.height * 0.5
            width: height

            palette.dark: "red"

            running: true
        }

        Text{
            text: "Loading"

            height: parent.height * 0.5

            anchors.horizontalCenter: parent.horizontalCenter

            horizontalAlignment: Text.AlignHCenter
        }
    }

    Canvas{
        id: _canvas

        //y: _chkRotate.height

        width: parent.width
        height: width

        transform: Scale {
            origin.x: width/2
            origin.y: height/2
            xScale: -1}

        renderStrategy: Canvas.Threaded

        property real coefX : width/ (400*2)
        property real coefY : width/ (400*2)

        onPaint: {
            if(dataPoints !== undefined)
            {
                // console.time("canvas paint")
                var ctx = getContext("2d")

                ctx.reset()

                ctx.lineWidth = lineWidth
                ctx.strokeStyle = "darkslategrey"

                ctx.translate(_canvas.width/2, _canvas.height/2)
                ctx.rotate(Math.PI)

                ctx.beginPath()

                for(var i=0; i<dataPoints.length; i++)
                {
                    ctx.lineTo(dataPoints[i].x*coefX, dataPoints[i].y*coefY);
                }
                ctx.stroke()
                // console.timeEnd("canvas paint")
            }
        }
    }
}
