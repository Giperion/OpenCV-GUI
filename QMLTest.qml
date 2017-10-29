import QtQuick 2.6
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

ApplicationWindow {
    id: mainWindow
    visible: true

    Rectangle {
        id: rectangle
        y: 0
        height: 39
        color: "#3d3d3d"
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
    }

    Flickable {
        id: flickable
        x: 501
        width: 139
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 38

        Rectangle {
            id: rectangle3
            color: "#242424"
            anchors.fill: parent

            VideoOutputWidget {
                id: videoOutputWidget
                x: 5
                y: 10
                width: 126
                height: 123
                videoName: qsTr("VideoInput")
                clip: true
            }

            VideoOutputWidget {
                id: videoOutputWidget1
                x: 5
                y: 148
                width: 126
                height: 123
                clip: true
                videoName: qsTr("VideoOutput")
            }
        }
    }

    Flickable {
        id: flickable1
        width: 139
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 38
        Rectangle {
            id: rectangle4
            color: "#242424"
            anchors.fill: parent
        }
    }

    StackLayout {
        id: stackLayout
        anchors.top: parent.top
        anchors.topMargin: 38
        anchors.right: parent.right
        anchors.rightMargin: 140
        anchors.left: parent.left
        anchors.leftMargin: 139
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0

        Rectangle {
            id: rectangle1
            color: "#666666"
        }
    }

}
