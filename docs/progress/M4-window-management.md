# M4 — Window Management

Status: **IN PROGRESS**. Current task: validated compositor integration. Updated: 2026-10-11.

Files changed: `CMakeLists.txt`, `src/window/LayoutPlanner.*`, `tests/LayoutPlannerTests.cpp`.

Implemented: pure C++ layout planner for Floating, Tiling, Hybrid, Focus and Smart modes, workspace filtering, floating exceptions, and a compositor adapter interface. The only adapter currently reports unsupported and cannot apply a plan to live windows.

Verified: CMake build, format and CTest layout test pass. Tests cover geometry preservation, nonoverlapping tiling, exceptions, smart focus and unsupported apply result. No live KWin window movement was attempted.

Capability research: [KWin scripting API](https://develop.kde.org/docs/plasma/kwin/api/) documents workspace/window operations in KWin scripts. [Wayland's protocol model](https://wayland.freedesktop.org/docs/book/Protocol.html) places global window geometry under compositor control. A safe, installed and validated Prism KWin adapter is still required. The nested KWin virtual test for M6 does not validate arbitrary window control.

Limitations: no application window enumeration, focus integration, layout profile or compositor-specific implementation. No new commit yet; prior reference `004b17d`.

Next autonomous action: implement a real compositor adapter only after an isolated integration harness can verify its effects without changing the active Plasma session.
