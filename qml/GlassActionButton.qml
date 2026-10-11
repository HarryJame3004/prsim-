import QtQuick
import QtQuick.Controls

Button {
    id: control
    implicitHeight: 34
    background: Rectangle {
        radius: 10
        color: !control.enabled ? "#29364B" : control.hovered ? "#40577B" : "#344761"
        border.color: "#526782"
    }
    contentItem: Text {
        text: control.text
        font: control.font
        color: control.enabled ? "#F5F8FF" : "#8799B3"
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}
