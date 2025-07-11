import QtQuick
import QtQuick.Controls

Slider{
    id: control

    snapMode: Slider.SnapAlways

    background: Rectangle {
             x: control.leftPadding
             y: control.topPadding + control.availableHeight / 2 - height / 2
             implicitWidth: 200
             implicitHeight: 4
             width: control.availableWidth
             height: implicitHeight
             radius: 2


             color: "#ffbca6"


             Rectangle {
                 width: control.visualPosition * parent.width
                 height: parent.height

                 gradient: Gradient{
                     orientation: Gradient.Horizontal

                     GradientStop{position: 0.0; color: "purple"}
                     GradientStop{position: 1.0; color: "orange"}
                 }

                 radius: 2
             }
    }

}
