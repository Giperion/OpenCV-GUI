import QtQuick 2.6
import QtQuick.Layouts 1.3

Item {
    id: item1
    property string videoName: "VideoSource"
    ColumnLayout {
        id: columnLayout
        width: 100
        height: 100
        clip: false
        spacing: 5

        Text {
            id: videoSourceTxt
            color: "#c6c6c6"
            text: videoName
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.right: parent.right
            anchors.rightMargin: 0
            fontSizeMode: Text.HorizontalFit
            textFormat: Text.PlainText
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12
        }

        Image {
            id: videoSource
            width: 50
            height: 50
            clip: false
            fillMode: Image.PreserveAspectCrop
        }

    }
}
