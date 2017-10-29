import QtQuick 2.6
import QtQuick.Layouts 1.3

Item {
RowLayout
{
    Image {
        id: image
        width: 100
        height: 100
        Layout.maximumHeight: 32
        Layout.maximumWidth: 32
        source: "qrc:/qtquickplugin/images/template_image.png"
    }

}
}
