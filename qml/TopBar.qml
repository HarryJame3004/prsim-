import QtQuick
import QtQuick.Controls

GlassPanel {
    id: bar
    objectName: "topBar"
    height: 52
    radius: appearanceSettings.cornerRadius * 0.75

    property date currentTime: new Date()
    property string timeText: Qt.formatDateTime(currentTime, "ddd d MMM  hh:mm")
    signal closeRequested()
    signal controlRequested()

    Timer {
        interval: appearanceSettings.visualProfile === "Eco" ? 60000 : 1000
        running: true
        repeat: true
        onTriggered: bar.currentTime = new Date()
    }

    Row {
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: parent.verticalCenter
        spacing: 14

        Label {
            text: "◆"
            color: "#BBA7FF"
            font.pixelSize: 21
        }
        Label {
            text: "PRISM"
            color: "#F5F8FF"
            font.pixelSize: 15
            font.bold: true
            font.letterSpacing: 2
        }
        Rectangle {
            width: 1
            height: 20
            color: "#516786"
        }
        Label {
            text: appearanceSettings.mode + " workspace"
            color: "#BCD0E9"
            font.pixelSize: 13
        }
    }

    Row {
        anchors.right: parent.right
        anchors.rightMargin: 14
        anchors.verticalCenter: parent.verticalCenter
        spacing: 18

        GlassActionButton {
            objectName: "controlButton"
            text: "☷"
            width: 32
            height: 32
            onClicked: bar.controlRequested()
        }

        Label {
            objectName: "clockLabel"
            text: bar.timeText
            color: "#F5F8FF"
            font.pixelSize: 13
            anchors.verticalCenter: parent.verticalCenter
        }
        GlassActionButton {
            objectName: "previewCloseButton"
            text: "×"
            width: 32
            height: 32
            font.pixelSize: 22
            onClicked: bar.closeRequested()
        }
    }
}
