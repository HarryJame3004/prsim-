# M2 — Application Shell

Status: **TESTING**. Current task: visible shell validation and remaining settings controls. Updated: 2026-10-11.

Files changed: `CMakeLists.txt`, `src/core/AppCatalog.*`, `src/core/AppearanceSettings.*`, `src/app/preview_main.cpp`, `qml/PreviewWindow.qml`, `qml/FloatingDock.qml`, `tests/AppCatalogTests.cpp`, `tests/PreviewTests.cpp`, CI workflow and README.

Implemented: GIO desktop entry discovery with hidden entries filtered; case-insensitive application search; GIO launching only for discovered IDs; explicit launch errors; persistent favorites with reordering; dock favorite tiles; Spotlight; Prism Settings with validated persistent Everyday/Developer mode, accent and opacity. Developer mode hides the Dock until bottom-edge hover or `Ctrl+Alt+D`; `Ctrl+Alt+P` opens Spotlight. Live window tiling remains M4 work.

Verified: CMake configure/build, later CTest 9/9, `qmllint qml/*.qml`, and `clang-format --dry-run --Werror` all passed. Catalog tests use temporary desktop entries and isolated settings; they verify hidden filtering, search, unknown launch rejection, a valid `/bin/true` launch, favorites and reordering. Preview tests click Dock positions to open Spotlight and Settings, then verify Developer Dock hiding and the Spotlight shortcut offscreen.

Errors/fixes: first build failed because `QColor` needed Qt Gui and Qt's `signals` macro collided with a GIO header; fixed target linkage and scoped macro undefinition. CTest catalog initially lacked an offscreen platform setting; direct offscreen test passed and CTest environment was corrected. Initial UI test could not locate Repeater delegates through QObject ownership; actual scene clicks now verify the user action. A later `Ctrl+Space` shortcut test passed offscreen but failed under virtual KWin Wayland; changing it to application-scoped `Ctrl+Alt+P` made both test paths pass.

Limitations: no live KDE rendering verification, no blur, no saved developer layout profiles, no user-facing launch-error timeout. Corner radius and visual profiles were added in M7. CI workflow updated but not run on hosted GitHub Actions. No new commit yet; prior reference `004b17d`.

Next autonomous action: complete developer layout profiles and live GUI validation. M2 remains TESTING while the remaining acceptance scope is incomplete.
