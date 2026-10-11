import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: window
    objectName: "foundationWindow"
    width: 520
    height: 280
    minimumWidth: 400
    minimumHeight: 240
    visible: true
    title: "Prism Desktop Foundation"

    Column {
        anchors.centerIn: parent
        spacing: 18

        Label {
            objectName: "foundationMessage"
            text: "Prism Desktop — Foundation Ready"
            font.pixelSize: 22
        }

        Label {
            objectName: "versionLabel"
            text: "Version " + appInfo.version
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Button {
            objectName: "closeButton"
            text: "Close"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: window.close()
        }
    }
}
