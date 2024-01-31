import QtQuick 2.15
import QtQml.Models 2.15


MouseArea{
    id: _rootMA

    width: _listView.width
    height: _listView.height/10

    property bool dragEnabled: true
    property bool held: false

    property var panelType

    drag.target: held ? contentRectangle : undefined
    drag.axis: Drag.YAxis

    pressAndHoldInterval: 200
    onPressAndHold:  held = (dragEnabled) ? true : false
    onReleased:
    {
        contentRectangle.Drag.drop()
        held = false
    }

//[root.components]
    DropArea {
        anchors { fill: parent;}

        onEntered: function(drag){
            var from = drag.source.DelegateModel.itemsIndex;
            var to = _rootMA.DelegateModel.itemsIndex;

            _visualModel.items.move(from, to)
        }

        onDropped: {

        }
    }

    Rectangle{
        id: contentRectangle

        width: _rootMA.width*0.9
        height: _rootMA.height
        opacity: _rootMA.held ? 0.8 : 1

        anchors {
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }

        border
        {
            width: held ? 3 : 1
            color: held ? "darkblue" : "black"
        }

        radius: width/10

        color:  "white"

        Drag.active: _rootMA.held
        Drag.source: _rootMA
        Drag.hotSpot.x: width / 2
        Drag.hotSpot.y: height / 2

        Column{
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            Text{
                visible: dragEnabled

                // замена quint8 на qint8 приводит к тому, что номер не передаётся. Разобраться где теряется связь
                text: "TEXT2"

                leftPadding: width/10
                rightPadding: width/10
                width: parent.width

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                elide: Text.ElideMiddle
            }

            Text{
                text: "TEXT"

                leftPadding: width/10
                rightPadding: width/10
                width: parent.width

                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                elide: Text.ElideMiddle
            }
        }
        //[rect.states]
        states: State {
            when: _rootMA.held

            ParentChange { target: contentRectangle; parent: _rootMA}//_listView }
            AnchorChanges {
                target: contentRectangle
                anchors { horizontalCenter: undefined; verticalCenter: undefined }
            }
        }
    }
}
