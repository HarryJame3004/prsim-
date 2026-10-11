# Current architecture

The M0 binary is an ordinary Qt Quick application. No session, compositor, display manager, or KDE integration is included.

```text
src/app/main.cpp            application entry point and QML engine
src/core/AppInfo.*          version data exposed to QML
qml/FoundationWindow.qml    reusable foundation window UI
tests/FoundationTests.cpp   version, QML load, content, and close interaction tests
```

`prism_core` owns application data. The executable creates `AppInfo`, exposes it through the QML context, and loads the UI from a compiled Qt resource. The QML window owns its presentation and close button. The same resource is compiled into the test binary so tests load exactly the shipped QML file.

The version comes from the CMake project version. Build products stay outside source files in the chosen build directory. CTest uses Qt's offscreen platform plugin for deterministic automated UI interaction; it does not prove that a human can see the window in a particular desktop session.

## Preview and core modules

```text
src/app/preview_main.cpp        ordinary preview application and QML wiring
src/core/AppCatalog.*           GIO discovery, validated launching, favorites
src/core/AppearanceSettings.*   validated persistent appearance settings
src/system/SystemStatus.*       asynchronous Linux metrics and probe interface
src/system/AudioService.*       asynchronous default-sink audio commands
src/system/BluetoothService.*   asynchronous controller status
src/window/LayoutPlanner.*      pure layout planner and compositor adapter contract
src/displays/DisplayManager.*   QScreen model, hotplug fallback, preview profiles
qml/PreviewWindow.qml           window composition and temporary panels
qml/TopBar.qml                  clock and Control Center entry
qml/FloatingDock.qml            launcher and favorite actions
qml/GlassPanel.qml              shared panel surface
qml/GlassActionButton.qml       shared panel button
```

The preview is an ordinary application. `prism_core`, `prism_system`, `prism_layout`, and `prism_displays` are separate static CMake targets. QML receives their QObject instances through the engine context. `ISystemProbe` makes asynchronous status updates testable without a live service. `ICompositorAdapter` deliberately has only an unsupported implementation today; layout planning does not imply compositor control. `DisplayManager` reads screens and saves preview choices without changing output configuration.

Tests compile the same QML resources as the app. CTest defaults to offscreen. The scripts in `tests/` can run the preview tests inside a virtual KWin Wayland session when D-Bus socket access is available. Neither test path verifies a real SDDM login.
