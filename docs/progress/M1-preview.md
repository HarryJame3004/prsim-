# M1 — Midnight Glass Preview

Status: **TESTING**. Current task: visible KDE rendering. Updated: 2026-10-11.

Files changed: `CMakeLists.txt`, `src/app/preview_main.cpp`, `qml/PreviewWindow.qml`, `qml/GlassPanel.qml`, `qml/TopBar.qml`, `qml/FloatingDock.qml`, `tests/PreviewTests.cpp`.

Implemented: separate Qt Quick preview executable; responsive navy background, reusable glass panel, translucent top bar, floating dock with hover response, real system clock, and close action. The preview is an ordinary window and does not alter Plasma.

Verified: CMake configure and build passed; `clang-format --dry-run --Werror` and `qmllint` passed; later CTest 9/9 includes offscreen preview load, component discovery, frame capture, clock text, and click-to-close. Offscreen preview stayed running for three seconds without diagnostics (`timeout` exit 124). A captured offscreen frame was inspected for layout and contrast: [preview image](images/preview-offscreen.png). Preview QtTest also passed under a nested KWin Wayland compositor. Live KDE display output remains unverified.

Error and fix: initial format gate identified two new files; formatted them and reran all gates successfully. The optional Vulkan-header CMake notice is not a required dependency. Wayland and X11 launches of the M0 binary still abort (134) without Qt diagnostic inside the sandbox, so visible GUI remains unverified.

Limitations: dock tiles are visual in M1; application actions belong to M2. Rendering quality and animation smoothness need a real GUI test. No new Git commit yet; prior reference `004b17d`.

Next autonomous action: implement M2 application discovery and settings while visible GUI access remains blocked.
