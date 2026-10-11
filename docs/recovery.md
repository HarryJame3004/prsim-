# Recovery and removal

Prism Preview runs as a normal process under the existing desktop. Close it with the top bar **×**, the window manager close action, or terminate only the `prism-preview` process if it becomes unresponsive. No KDE Plasma, KWin or SDDM configuration is changed by the repository build or tests.

To remove a local build, remove only the build directory you chose (`build` by default). The CPack TGZ can be extracted into a private directory and removed by deleting that extracted directory. The project has not installed a system session entry; continue using the normal Plasma login choice.

Appearance settings and favorites use the `PrismDesktop/PrismDesktop.ini` user configuration under the platform's XDG config directory. Back it up before editing or removing it manually. The preview does not write display mode changes.

The virtual KWin test uses separate paths under `/tmp/prism-kwin-*`. A failed virtual test does not replace the active desktop. Review [M6 progress](progress/M6-independent-session.md) before attempting any independent session work.
