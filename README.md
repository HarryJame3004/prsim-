# Prism Desktop

Prism Desktop is an open-source Linux desktop shell project. The repository contains the M0 foundation and a Midnight Glass preview with an application shell under development. Both binaries are ordinary applications under an existing desktop session; neither replaces KDE Plasma.

## Requirements

- CMake 3.21 or newer and a C++20 compiler
- Qt 6.4 or newer with Core, Gui, Qml, Quick, QuickControls2, and Test development modules
- Qt Quick and Qt Quick Controls QML runtime modules
- GIO development libraries (`gio-2.0` and `gio-unix-2.0`) for desktop entry discovery and launching
- Python 3 for the automated preview performance smoke test

Optional runtime clients: `pactl` for audio status/control and `bluetoothctl` for Bluetooth status. Prism shows unavailable states when they or their services are absent.

On Ubuntu 24.04, the corresponding packages include `qt6-base-dev`, `qt6-declarative-dev`, `libglib2.0-dev`, `python3`, `qml6-module-qtquick`, `qml6-module-qtquick-controls`, and `qml6-module-qtquick-window`. Install dependencies using your normal system administration process; the project does not install them.

## Build and test

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

CTest runs Qt Quick interaction tests through Qt's `offscreen` platform plugin. To launch the preview in your desktop session:

```sh
./build/prism-preview
```

Use the top bar **×** button or your window manager's close action to exit. `./build/prism-foundation` still runs the original M0 smoke-test window. Preview rendering in the live KDE session has not been verified from this restricted environment.

The preview can search standard desktop applications, pin favorites, change appearance and mode settings, show basic system information, and preview detected displays. Developer mode currently persists a choice but has no live tiling behavior. Control Center queries audio and Bluetooth through `pactl` and `bluetoothctl`, showing unavailable states when their services cannot be reached.
Developer mode hides the Dock; move the pointer to the bottom edge or press `Ctrl+Alt+D` to reveal it. Press `Ctrl+Alt+P` to open Spotlight.

## Preview package

```sh
cpack -G TGZ --config build/CPackConfig.cmake -B build/package
```

The archive contains only the preview application, desktop launcher, license and README. It is not an SDDM session. See [recovery](docs/recovery.md) before any manual deployment.

## Project status

See [architecture](docs/architecture.md), [roadmap](docs/roadmap.md), and [progress](docs/progress/STATUS.md). Features that depend on a compositor or independent session remain incomplete.

## License

Prism Desktop is licensed under GPL-3.0-or-later. See [LICENSE](LICENSE).
