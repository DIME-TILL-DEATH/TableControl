import QtQuick
import QtQuick.Controls

import UiObjects

Item{
    width: parent.width
    height: _canvas.width

    property var dataPoints

    function update()
    {
        _canvas.requestPaint();
    }

    Canvas{
        id: _canvas

        //y: _chkRotate.height

        width: parent.width
        height: width

        renderStrategy: Canvas.Threaded

        property real coefX : width/ (400*2)
        property real coefY : width/ (400*2)

        onPaint: {
            var ctx = getContext("2d")

            ctx.reset()

            ctx.lineWidth = 0.25
            ctx.strokeStyle = "white"

            ctx.translate(_canvas.width/2, _canvas.height/2)
            ctx.rotate(Math.PI)

            ctx.beginPath()

            for(var i=0; i<dataPoints.length; i++)
            {
                ctx.lineTo(dataPoints[i].x*coefX, dataPoints[i].y*coefY);
            }

            ctx.closePath()
            ctx.stroke()
        }
    }
}
