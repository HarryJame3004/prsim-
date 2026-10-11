# M6 — Independent Prism Session

Status: **IN PROGRESS**. A selectable, functional login session still depends on compositor and shell protocol integration. Updated: 2026-10-11.

Files changed: `tests/nested-preview.sh`, `tests/nested-preview-tests.sh`; no system session files changed.

Verified in isolation: installed `kwin_wayland` supports `--virtual` and `--exit-with-session` per its local help output. With an isolated `XDG_RUNTIME_DIR`, configuration/data paths and D-Bus session, `kwin_wayland --virtual --exit-with-session /bin/true` exited 0. The first sandboxed attempt failed because D-Bus could not bind a socket; the approved isolated rerun succeeded. Prism Preview then stayed alive under nested Wayland for the expected six-second timeout (exit 124). `prism-preview-tests` ran under nested Wayland and passed 4/4 QtTest cases, with KWin/session exit 0.

Diagnostics: the nested run logged portal, AT-SPI and PipeWire availability warnings; QtTest still passed. A later nested test exposed a `Ctrl+Space` shortcut conflict and failed after its timeout; the shortcut changed to `Ctrl+Alt+P`, and the latest nested QtTest completed 4/4 with exit 0. This proves a compositor-backed test path, not a complete shell or login session.

Outstanding dependency: Prism Preview is still an ordinary top-level application. There is no verified layer-shell, output-bound shell surface, compositor adapter, logout lifecycle or recovery supervisor. Shipping an SDDM session entry now would present an incomplete desktop at login. No session metadata is installed or staged as selectable. KDE Plasma and SDDM were not modified.

Next autonomous action: implement and validate the compositor shell protocol and lifecycle in nested Wayland, then stage session metadata and packaging. Actual SDDM login requires a later deployment gate.
