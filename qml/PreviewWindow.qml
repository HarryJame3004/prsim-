import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: window
    objectName: "previewWindow"
    width: 1180
    height: 740
    minimumWidth: 760
    minimumHeight: 520
    visible: true
    title: "Prism Desktop — Midnight Glass Preview"
    color: "#0C1426"
    property bool searchOpen: false
    property bool settingsOpen: false
    property bool controlOpen: false
    property bool displaysOpen: false
    property bool dockReveal: false

    Shortcut {
        sequence: "Ctrl+Alt+P"
        context: Qt.ApplicationShortcut
        onActivated: { window.searchOpen = true; window.settingsOpen = false; window.controlOpen = false }
    }
    Shortcut {
        sequence: "Ctrl+Alt+D"
        onActivated: window.dockReveal = !window.dockReveal
    }
    Connections {
        target: appearanceSettings
        function onModeChanged() {
            if (appearanceSettings.mode === "Developer") window.dockReveal = false
        }
    }
    Timer {
        id: dockHideTimer
        interval: 1200
        onTriggered: {
            if (appearanceSettings.mode === "Developer" && !dockHover.hovered)
                window.dockReveal = false
        }
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0; color: "#15243B" }
            GradientStop { position: 0.55; color: "#101C32" }
            GradientStop { position: 1; color: "#0C1426" }
        }
    }

    Rectangle {
        width: Math.min(window.width * 0.58, 620)
        height: width
        radius: width / 2
        x: window.width * 0.48
        y: window.height * 0.2
        color: "#172D50"
        opacity: 0.48
    }

    TopBar {
        id: topBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 18
        onCloseRequested: window.close()
        onControlRequested: { window.controlOpen = !window.controlOpen; window.settingsOpen = false; window.searchOpen = false }
    }

    Column {
        anchors.left: parent.left
        anchors.leftMargin: Math.max(48, window.width * 0.095)
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -26
        spacing: 14

        Label {
            text: "MIDNIGHT GLASS  /  PREVIEW 01"
            color: "#87E9C6"
            font.pixelSize: 12
            font.bold: true
            font.letterSpacing: 2
        }
        Label {
            text: "Make room for\nwhat matters."
            color: "#F5F8FF"
            font.pixelSize: Math.min(56, window.width * 0.052)
            font.bold: true
            lineHeight: 0.98
        }
        Label {
            text: "A calm workspace for building ambitious things."
            color: "#BCD0E9"
            font.pixelSize: 16
        }
    }

    GlassPanel {
        width: Math.min(300, window.width * 0.27)
        height: 190
        anchors.right: parent.right
        anchors.rightMargin: Math.max(42, window.width * 0.085)
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 2
        rotation: -3

        Column {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 12
            Label { text: "TODAY'S FOCUS"; color: "#83D7FF"; font.pixelSize: 11; font.bold: true; font.letterSpacing: 2 }
            Label { text: "Create without friction."; color: "#F5F8FF"; font.pixelSize: 20; font.bold: true; wrapMode: Text.WordWrap; width: parent.width }
            Rectangle { width: parent.width; height: 1; color: "#536989" }
            Label { text: appearanceSettings.mode + " mode  ·  Workspace 01"; color: "#BCD0E9"; font.pixelSize: 12 }
        }
    }

    FloatingDock {
        id: floatingDock
        catalog: appCatalog
        visible: appearanceSettings.mode !== "Developer" || window.dockReveal
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        onSearchRequested: { window.searchOpen = !window.searchOpen; window.settingsOpen = false }
        onSettingsRequested: { window.settingsOpen = !window.settingsOpen; window.searchOpen = false }
        HoverHandler {
            id: dockHover
            onHoveredChanged: {
                if (hovered) dockHideTimer.stop()
                else if (appearanceSettings.mode === "Developer") dockHideTimer.restart()
            }
        }
    }
    MouseArea {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 18
        hoverEnabled: true
        visible: appearanceSettings.mode === "Developer"
        onEntered: { window.dockReveal = true; dockHideTimer.restart() }
    }

    GlassPanel {
        id: searchPanel
        objectName: "searchPanel"
        visible: window.searchOpen
        width: Math.min(580, window.width - 64)
        height: Math.min(490, window.height - 150)
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: topBar.bottom
        anchors.topMargin: 32
        z: 10
        glassOpacity: appearanceSettings.opacity

        Column {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 12
            Label { text: "PRISM SPOTLIGHT"; color: appearanceSettings.accent; font.bold: true; font.letterSpacing: 2 }
            TextField {
                id: searchField
                objectName: "searchField"
                width: parent.width
                placeholderText: "Find an application"
                onTextChanged: appCatalog.search = text
            }
            Label { text: appCatalog.lastError; color: "#F59DBE"; visible: text.length > 0; wrapMode: Text.WordWrap; width: parent.width }
            ListView {
                width: parent.width
                height: searchPanel.height - 112
                clip: true
                spacing: 5
                model: appCatalog
                delegate: Rectangle {
                    required property string appId
                    required property string appName
                    required property bool favorite
                    width: ListView.view.width
                    height: 46
                    radius: 10
                    color: "#344761"
                    Row {
                        anchors.fill: parent
                        anchors.margins: 7
                        spacing: 8
                        GlassActionButton { text: appName; width: Math.max(100, parent.width - 126); onClicked: appCatalog.launch(appId) }
                        GlassActionButton { text: favorite ? "★" : "☆"; width: 32; onClicked: appCatalog.toggleFavorite(appId) }
                        GlassActionButton { text: "↑"; width: 32; onClicked: appCatalog.moveFavorite(appId, -1) }
                        GlassActionButton { text: "↓"; width: 32; onClicked: appCatalog.moveFavorite(appId, 1) }
                    }
                }
            }
        }
    }

    GlassPanel {
        id: settingsPanel
        objectName: "settingsPanel"
        visible: window.settingsOpen
        width: 360
        height: Math.min(440, window.height - 120)
        anchors.right: parent.right
        anchors.rightMargin: 30
        anchors.top: topBar.bottom
        anchors.topMargin: 18
        z: 10

        Flickable {
            anchors.fill: parent
            anchors.margins: 14
            clip: true
            contentHeight: settingsColumn.implicitHeight + 16
            Column {
            id: settingsColumn
            width: parent.width
            spacing: 10
            Label { text: "PRISM SETTINGS"; color: appearanceSettings.accent; font.bold: true; font.letterSpacing: 2 }
            Label { text: "Workspace mode"; color: "#F5F8FF" }
            ComboBox {
                objectName: "modeSelector"
                width: parent.width
                model: ["Everyday", "Developer"]
                currentIndex: appearanceSettings.mode === "Developer" ? 1 : 0
                onActivated: appearanceSettings.mode = currentText
            }
            Label { text: "Visual profile"; color: "#F5F8FF" }
            ComboBox {
                width: parent.width
                model: ["Rich", "Balanced", "Eco"]
                currentIndex: model.indexOf(appearanceSettings.visualProfile)
                onActivated: appearanceSettings.visualProfile = currentText
            }
            Label { text: "Glass opacity"; color: "#F5F8FF" }
            Slider {
                width: parent.width
                from: 0.55
                to: 1.0
                value: appearanceSettings.opacity
                onMoved: appearanceSettings.opacity = value
            }
            Label { text: "Corner radius"; color: "#F5F8FF" }
            Slider {
                width: parent.width
                from: 8
                to: 32
                stepSize: 1
                value: appearanceSettings.cornerRadius
                onMoved: appearanceSettings.cornerRadius = Math.round(value)
            }
            Row {
                spacing: 10
                Repeater {
                    model: ["#BBA7FF", "#83D7FF", "#87E9C6", "#F59DBE"]
                    delegate: Rectangle {
                        required property string modelData
                        width: 32; height: 32; radius: 16; color: modelData
                        MouseArea { anchors.fill: parent; onClicked: appearanceSettings.accent = modelData }
                    }
                }
            }
            }
        }
    }

    GlassPanel {
        objectName: "controlCenter"
        visible: window.controlOpen
        width: 310
        height: 310
        anchors.top: topBar.bottom
        anchors.topMargin: 18
        anchors.right: parent.right
        anchors.rightMargin: 30
        z: 11
        Column {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 12
            Label { text: "CONTROL CENTER"; color: "#83D7FF"; font.bold: true; font.letterSpacing: 2 }
            Label { text: "CPU  " + (systemStatus.cpuPercent < 0 ? "Unavailable" : Math.round(systemStatus.cpuPercent) + "%"); color: "#F5F8FF" }
            Label { text: "Memory  " + (systemStatus.memoryPercent < 0 ? "Unavailable" : Math.round(systemStatus.memoryPercent) + "%"); color: "#F5F8FF" }
            Label { text: "Battery  " + (systemStatus.batteryPercent < 0 ? "Unavailable" : systemStatus.batteryPercent + "%" + (systemStatus.charging ? " · Charging" : "")); color: "#F5F8FF" }
            Label { text: "Network  " + (systemStatus.networkConnected ? "Interface active" : "Unavailable"); color: "#F5F8FF" }
            Row {
                spacing: 6
                Label {
                    text: audioService.available ? "Audio  " + audioService.volumePercent + "%" +
                           (audioService.muted ? " · Muted" : "") : "Audio  Unavailable"
                    color: "#F5F8FF"
                    width: 148
                    anchors.verticalCenter: parent.verticalCenter
                }
                GlassActionButton { text: "−"; width: 34; enabled: audioService.available; onClicked: audioService.setVolume(audioService.volumePercent - 5) }
                GlassActionButton { text: "+"; width: 34; enabled: audioService.available; onClicked: audioService.setVolume(audioService.volumePercent + 5) }
                GlassActionButton { text: audioService.muted ? "◌" : "♪"; width: 34; enabled: audioService.available; onClicked: audioService.toggleMute() }
            }
            Label {
                text: bluetoothService.available ?
                      "Bluetooth  " + (bluetoothService.powered ? "On" : "Off") :
                      "Bluetooth  Unavailable"
                color: "#F5F8FF"
            }
            GlassActionButton { text: "Displays  " + displayManager.displayCount; onClicked: { window.displaysOpen = true; window.controlOpen = false } }
        }
    }

    GlassPanel {
        objectName: "displayPreview"
        visible: window.displaysOpen
        width: 400
        height: 290
        anchors.top: topBar.bottom
        anchors.topMargin: 18
        anchors.right: parent.right
        anchors.rightMargin: 30
        z: 12
        Column {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 10
            Label { text: "DISPLAY PREVIEW"; color: "#83D7FF"; font.bold: true; font.letterSpacing: 2 }
            Label { text: "Preview only · system layout is unchanged"; color: "#BCD0E9"; font.pixelSize: 12 }
            ListView {
                width: parent.width
                height: 170
                model: displayManager
                clip: true
                delegate: Row {
                    required property string displayId
                    required property string displayName
                    required property bool previewPrimary
                    spacing: 8
                    width: ListView.view.width
                    height: 44
                    Label { text: displayName + (previewPrimary ? "  ·  Primary" : ""); color: "#F5F8FF"; width: parent.width - 110; elide: Text.ElideRight }
                    GlassActionButton { text: "Select"; onClicked: displayManager.choosePreviewPrimary(displayId) }
                }
            }
            GlassActionButton { text: "Close"; onClicked: window.displaysOpen = false }
        }
    }
}
