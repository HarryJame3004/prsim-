#!/bin/sh
set -eu

export QT_QPA_PLATFORM=wayland
exec "${1:-$(dirname "$0")/../build/prism-preview}"
