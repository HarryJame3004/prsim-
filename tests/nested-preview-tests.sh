#!/bin/sh
set -eu

export QT_QPA_PLATFORM=wayland
exec "$(dirname "$0")/../build/prism-preview-tests" -v2
