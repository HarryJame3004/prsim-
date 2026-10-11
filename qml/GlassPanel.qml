import QtQuick

Rectangle {
    property real glassOpacity: 0.88
    color: Qt.rgba(0.15, 0.22, 0.34, glassOpacity)
    border.color: "#496182"
    border.width: 1
    radius: appearanceSettings.cornerRadius
}
