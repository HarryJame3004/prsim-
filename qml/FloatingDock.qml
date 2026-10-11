import QtQuick
import QtQuick.Controls

GlassPanel {
    id: dock
    objectName: "floatingDock"
    width: dockRow.implicitWidth + 32
    height: 78
    radius: appearanceSettings.cornerRadius * 1.25
    glassOpacity: 0.95
    property var catalog
    signal searchRequested()
    signal settingsRequested()

    Row {
        id: dockRow
        anchors.centerIn: parent
        spacing: 12

        Repeater {
            objectName: "fixedDockRepeater"
            model: [
                { glyph: "◆", name: "Prism", tint: "#BBA7FF", action: "search" },
                { glyph: "⌕", name: "Search", tint: "#83D7FF", action: "search" },
                { glyph: "⚙", name: "Settings", tint: "#87E9C6", action: "settings" }
            ]

            delegate: Rectangle {
                required property var modelData
                objectName: "dockTile_" + modelData.name
                width: 54
                height: 54
                radius: 16
                color: tileMouse.containsMouse ? "#40577B" : "#2D4261"
                border.color: tileMouse.containsMouse ? modelData.tint : "#526782"
                border.width: 1
                scale: tileMouse.containsMouse ? 1.09 : 1
                Behavior on scale {
                    NumberAnimation {
                        duration: appearanceSettings.visualProfile === "Eco" ? 0 :
                                  appearanceSettings.visualProfile === "Balanced" ? 100 : 150
                    }
                }

                Label {
                    anchors.centerIn: parent
                    text: modelData.glyph
                    color: modelData.tint
                    font.pixelSize: 26
                }
                MouseArea {
                    id: tileMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        if (modelData.action === "settings") dock.settingsRequested()
                        else dock.searchRequested()
                    }
                }
                ToolTip.visible: tileMouse.containsMouse
                ToolTip.text: modelData.name
                ToolTip.delay: 500
            }
        }
        Repeater {
            model: dock.catalog ? dock.catalog.favorites : []
            delegate: Rectangle {
                required property string modelData
                width: 54
                height: 54
                radius: 16
                color: favoriteMouse.containsMouse ? "#40577B" : "#2D4261"
                border.color: "#BBA7FF"
                Label { anchors.centerIn: parent; text: modelData.slice(0, 1).toUpperCase(); color: "#F5F8FF"; font.pixelSize: 23 }
                MouseArea {
                    id: favoriteMouse
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: dock.catalog.launch(modelData)
                }
                ToolTip.visible: favoriteMouse.containsMouse
                ToolTip.text: modelData
            }
        }
    }
}
