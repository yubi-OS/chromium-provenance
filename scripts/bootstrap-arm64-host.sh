#!/usr/bin/env bash
# Host prerequisites only. Native Chromium clang/Rust still need qualification.
# Run --check unprivileged; run --install locally with sudo after review.
set -euo pipefail

check_tools() {
  [[ $(uname -m) == aarch64 ]] || { echo 'Expected native aarch64 host' >&2; return 78; }
  local missing=0 tool
  for tool in git python3 g++ clang clang++ ld.lld cmake ninja pkg-config curl unzip xz file node npm; do
    if ! command -v "$tool" >/dev/null 2>&1; then
      printf 'Missing prerequisite: %s\n' "$tool" >&2
      missing=1
    fi
  done
  if (( missing )); then
    printf '%s\n' 'Run scripts/bootstrap-arm64-host.sh --install with sudo on the runner host.' >&2
    return 78
  fi
  python3 -c 'import sys; assert sys.version_info >= (3, 9), "Python 3.9+ required"'
  printf '%s\n' 'Host prerequisite executables present. Chromium toolchain compatibility remains a separate gate.'
}

case "${1:---check}" in
  --check) check_tools ;;
  --install)
    [[ $(uname -m) == aarch64 ]] || { echo 'Expected native aarch64 host' >&2; exit 78; }
    [[ $EUID == 0 ]] || { echo 'Run this installer with sudo locally; no sudo password belongs in CI.' >&2; exit 77; }
    command -v apt-get >/dev/null || { echo 'This installer requires Ubuntu/Debian apt-get' >&2; exit 78; }
    export DEBIAN_FRONTEND=noninteractive
    apt-get update
    apt-get install -y --no-install-recommends \
      build-essential clang lld llvm libclang-dev cmake ninja-build pkg-config \
      git python3 python3-venv python3-dev curl ca-certificates xz-utils zip unzip file \
      nodejs npm bison gperf flex patchelf \
      libssl-dev zlib1g-dev libffi-dev libnss3-dev libnspr4-dev \
      libasound2-dev libatk1.0-dev libatk-bridge2.0-dev libcups2-dev \
      libdrm-dev libxkbcommon-dev libxcomposite-dev libxdamage-dev libxfixes-dev \
      libxrandr-dev libgbm-dev libpango1.0-dev libcairo2-dev libgtk-3-dev \
      libx11-xcb-dev libpulse-dev libudev-dev libkrb5-dev xvfb xauth
    check_tools
    printf '%s\n' 'Provisioning complete. No sudoers, runner permissions, sandbox settings or system services were changed.'
    ;;
  *) echo 'Usage: bootstrap-arm64-host.sh [--check|--install]' >&2; exit 64 ;;
esac
