# M7 — Optimization and Release

Status: **IN PROGRESS**. Current task: measured preview performance and preview packaging. Updated: 2026-10-11.

Files changed: `src/core/AppearanceSettings.*`, `src/system/SystemStatus.*`, `src/app/preview_main.cpp`, `qml/GlassPanel.qml`, `qml/FloatingDock.qml`, `qml/TopBar.qml`, `qml/PreviewWindow.qml`, tests, `CMakeLists.txt`, `packaging/org.prismdesktop.Preview.desktop`.

Implemented: persistent Rich/Balanced/Eco visual profiles. Eco disables dock scale animation and lowers system polling to 15 seconds; Balanced uses 5 seconds and Rich 3 seconds. Corner radius is configurable and validated. A preview-only desktop launcher and CPack TGZ staging are configured. `tests/measure-preview.py` provides a bounded, isolated offscreen CPU/RSS regression gate. This is not a stable v1.0 or independent session release.

Verified: build, QML lint, format, desktop-file validation, `clang-tidy` on seven C++ modules and CTest 9/9 passed. CPack produced `build/package/prism-desktop-preview-0.1.0-Linux.tar.gz` (SHA-256 `3acec13b45be66d536f1aefbd917827ebc97d55b77f847793b40c253e51ee6bc`). The archive contains the binary, desktop launcher, LICENSE and README. The extracted binary stayed running for a three-second offscreen smoke test (expected timeout exit 124, no diagnostics). The desktop file initially produced a duplicate-category hint; its category was narrowed and validation rerun without diagnostics.

Performance measurement: the four-second CTest guardrail reported 0.445 CPU seconds (11.0% of one core, including startup) and peak RSS 95,000 KiB. The harness intentionally terminated the process (signal 15) after it stayed alive for the full interval; stderr was empty. Limits are 50% of one core and 256 MiB RSS, intended to catch major regressions rather than certify optimization. The measurements are offscreen on this machine; no GUI GPU or battery measurement is claimed.

Limitations: blur is not implemented, animations and performance need real display testing, no long-duration leak test, no full release readiness. No new commit yet; prior reference `004b17d`.

Next autonomous action: implement remaining earlier milestones, run real desktop validation, and improve release checks before considering a stable release.
