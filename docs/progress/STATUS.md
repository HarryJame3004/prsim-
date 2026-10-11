# Project status

Updated: 2026-10-11. M0–M7 are authorized. No desktop or display-manager configuration has been changed.

Git checkpoint: `git add -A` failed because `.git/index.lock` could not be created on a read-only filesystem. All changes remain in the working tree; last existing commit is `004b17d`. No permission bypass or remote push was attempted.

| Milestone | Status | Summary |
| --- | --- | --- |
| M0 Foundation | BLOCKED | Code, build, static checks, and offscreen tests pass. A real KDE/Wayland window could not be verified inside this sandbox; direct Wayland and X11 launches abort with exit code 134 and no Qt diagnostic. |
| M1 Midnight Glass Preview | TESTING | Separate preview, glass surfaces, dock, top bar, live clock, and close action pass offscreen tests; visible KDE rendering remains unverified. |
| M2 Application Shell | TESTING | GIO discovery/launch, Spotlight, favorites, and persistent settings pass automated tests. Developer behavior and GUI rendering remain incomplete. |
| M3 System Integration | IN PROGRESS | CPU/RAM/battery/network probe, audio commands, Bluetooth status and Control Center pass tests; notifications and direct service APIs remain. |
| M4 Window Management | IN PROGRESS | Layout planner passes tests; no validated live compositor adapter. |
| M5 Adaptive Displays | IN PROGRESS | QScreen model, preview primary, profile and simulated hotplug pass tests; no real display changes. |
| M6 Independent Session | IN PROGRESS | Preview QtTest passes in isolated KWin Wayland; a selectable shell/login session is not implemented. |
| M7 Optimization and Release | IN PROGRESS | Power profiles, automated offscreen performance guardrail and preview TGZ package pass; full release gates remain. |

See [M0](M0-foundation.md), [M1](M1-preview.md), [M2](M2-application-shell.md), [M3](M3-system-integration.md), [M4](M4-window-management.md), [M5](M5-adaptive-displays.md), [M6](M6-independent-session.md), [M7](M7-release.md), and [checkpoint](CHECKPOINT.json). No system desktop configuration was changed.
