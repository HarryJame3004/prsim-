# M0 foundation progress

Updated: 2026-10-11. Overall status: **BLOCKED** pending a real desktop-session launch check. M0 code and automated verification are complete; M1 preview development has begun independently.

| Step | Status | Completed work and modified files | Build result | Test result | Issues, fixes, and remaining limitations |
| --- | --- | --- | --- | --- | --- |
| M0.1 Repository | DONE | Added `LICENSE`, `README.md`, `CONTRIBUTING.md`, `.gitignore`, `.clang-format`, `CMakeLists.txt`, `docs/architecture.md`, `docs/roadmap.md`; preserved `AGENTS.md`. | Full build passed. | File and license review passed. | No pre-existing source files existed. |
| M0.2 Architecture | DONE | Added `src/core/AppInfo.*`, `src/app/main.cpp`, `qml/FoundationWindow.qml`, `tests/FoundationTests.cpp`. | Core, app, and test targets compiled. | QtTest passed. | No empty future modules added. |
| M0.3 Application | TESTING | Built normal Qt Quick `ApplicationWindow` with required message, version, and Close button. | Executable compiled. | Offscreen QML load, text, and click-to-close passed. | Visible KDE session launch remains unverified. Direct Wayland/X11 attempts aborted with code 134 in sandbox. |
| M0.4 Build system | DONE | CMake enforces C++20, finds Qt 6 modules, enables warnings and CTest, supports out-of-source builds. | Configure and compilation passed. | CTest passed. | Qt reported optional missing `WrapVulkanHeaders` during configure; not required by this app. |
| M0.5 Tests | DONE | Added version and QML interaction tests. | Test executable compiled. | CTest: 1/1 passed; QtTest: 4 passed, 0 failed. `clang-format --dry-run --Werror` and `qmllint` passed. | Initial C++ format check failed; formatted source and reran successfully. `qmlformat --check` is unsupported by installed version, so `qmllint` was used. Offscreen testing does not prove visible rendering. |
| M0.6 Reports | DONE | Added `docs/progress/STATUS.md` and this report. | Documentation is build independent. | Content reviewed against actual command results. | GUI limitation recorded. |
| M0.7 CI | DONE | Added `.github/workflows/build.yml` with Ubuntu 24.04 build, format, QML lint, and CTest jobs. | Equivalent local build passed. | Equivalent local checks passed. | Hosted GitHub Actions run is unverified; nothing was pushed. |
| M0.8 Final validation | BLOCKED | Reviewed structure, dependencies, license, source, tests, and Git status. | Local build passed. | Automated checks passed. | Real KDE window launch cannot be confirmed from the restricted execution environment. |

## Verified commands and results

Environment: CMake 4.4.3, GCC 16.2.1, Qt 6.11.2. Qt development modules were present before building; no packages were installed.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 2
clang-format --dry-run --Werror src/app/main.cpp src/core/AppInfo.h src/core/AppInfo.cpp tests/FoundationTests.cpp
qmllint qml/FoundationWindow.qml
ctest --test-dir build --output-on-failure
QT_QPA_PLATFORM=offscreen ./build/prism-tests -v1
```

The configure, build, format, QML lint, and CTest commands exited 0. CTest reported 1/1 passed; direct QtTest reported 4 passed, 0 failed, 0 skipped. Build output contained no compiler warnings. `timeout 3s env QT_QPA_PLATFORM=offscreen ./build/prism-foundation` exited 124 after the expected timeout with no diagnostic output, showing that the app remained running; this is not visual GUI verification. `timeout 3s env QT_QPA_PLATFORM=wayland ./build/prism-foundation` and the equivalent `xcb` command exited 134 in this environment. A debugger trace was unavailable because ptrace is restricted. Later M6 work successfully ran the preview and its tests in an isolated KWin Wayland compositor, supporting an environment-specific cause for the direct sandbox abort but not replacing live KDE verification.

## Commit reference

Initial M0 foundation commit: `0c23a70` (`Establish M0 Qt foundation`). No remote push or publication was performed.

## Next verification needed

From an unrestricted KDE Plasma session, run `./build/prism-foundation`, confirm the normal window displays the message and version, and use **Close**. Record the result here before changing M0 or M0.3/M0.8 to DONE.
