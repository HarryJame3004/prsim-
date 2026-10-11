# M5 — Adaptive Displays

Status: **IN PROGRESS**. Current task: layout profiles and compositor-specific validation. Updated: 2026-10-11.

Files changed: `CMakeLists.txt`, `src/displays/DisplayManager.*`, `src/app/preview_main.cpp`, `qml/PreviewWindow.qml`, `tests/DisplayManagerTests.cpp`, `tests/PreviewTests.cpp`.

Implemented: `QScreen` detection and geometry/hotplug observation, laptop/external classification by connector name, preview primary selection, persistent named primary profile, fallback when a selected display disappears, and a non-applying display panel in Control Center.

Verified: CMake build, format, QML lint and CTest display simulation pass. Test covers two displays, unplug fallback, failed restore while absent and successful restore after reconnect. No actual display mode or primary output was changed.

Error/fix: first build used a nonexistent `QScreen::nameChanged` signal; removed it and retested all six CTest suites successfully.

Limitations: connector names may not be stable across compositors; saved profile currently stores screen IDs and preview primary only, not full resolution/position/scale. No confirmation and rollback workflow for real mode changes exists. No new commit yet; prior reference `004b17d`.

Next autonomous action: retain preview-only operation until a validated compositor display adapter and rollback path exist.
