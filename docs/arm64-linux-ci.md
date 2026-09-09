# Linux ARM64 build lane

## Current evidence, 2026-09-09

Runner `ubuntu` (org runner id 22): labels `self-hosted`, `Linux`, `ARM64`, `HIGH-MEM`. The machine reports Ubuntu 26.04.1, 12 CPUs, 62 GiB RAM, 8 GiB swap, 188 GiB available disk. Hardware is available; build duration is unmeasured. The earlier 32-core estimate was a sizing suggestion, not a minimum requirement.

[Qualification run 34417263107](https://github.com/yubi-OS/chromium-provenance/actions/runs/34417263107) executed on the intended runner at overlay `650a324bfb6b95bdb6ff163bce725afff09d55cc`. Host inspection passed; compilation failed with `g++: command not found` (exit 127). `sudo -n true` reported `interactive authentication is required`. No browser binary has been built.

Only git and python3 from the inspected development-tool list were present. `scripts/bootstrap-arm64-host.sh --check` now names missing prerequisites before compilation. No tests are skipped to report success.

## Runner access

The original Default runner group excluded public repositories. Runner 22 was moved to group `chromium-high-mem` (id 3), restricted to this repository and `.github/workflows/arm64-linux.yml@refs/heads/main`. This enables the public overlay without exposing `rock1` or changing the Default group's public-repository policy. It also means other repositories cannot use runner 22 until explicitly granted access.

The workflow is manual-only, guards `refs/heads/main`, uses read-only contents permission, removes checkout credentials, serializes HIGH-MEM jobs and uploads diagnostic evidence on failures. Pull requests from forks never trigger this runner workflow.

## One-time administrator setup

On HIGH-MEM, use the overlay checkout already created by qualification:

```sh
sudo bash /home/ubuntu/actions-runner/_work/chromium-provenance/chromium-provenance/scripts/bootstrap-arm64-host.sh --install
```

First update that checkout to the reviewed commit containing the script (or download the script at that immutable commit). The installer uses the host's existing apt repositories, installs build/runtime development packages, and checks the resulting executables. It neither changes sudoers nor grants unattended root access. Package installation and availability on this host remain unverified until this command actually runs.

## Native toolchain work after provisioning

Pinned Chromium: `153.0.8010.36`, commit `507c6ee3e2f3b2ca0e660547e5b9ea4820c67f4c`.

The pin's `tools/clang/scripts/update.py` maps Linux to `Linux_x64`; `tools/rust/update_rust.py` uses the same platform helper. Blindly running hooks would fetch x86-64 clang/Rust on this ARM64 host. Depot tools and the Chromium compiler package have distinct host-support matrices.

Next gates, all pending:

1. Pin depot_tools and sync a shallow Chromium checkout plus DEPS into a persistent, exclusively locked build directory. Preserve disk reserve; never clean unrelated host files. Source syncing has not yet started.
2. Establish native clang and Rust executables. `build.py --host-cc/--host-cxx` bypasses downloading the x64 bootstrap compiler. Its optional bootstrap stage hardcodes `LLVM_TARGETS_TO_BUILD=X86` on Linux, requiring an explicit ARM64 correction if used. System packages alone are not proof of compatibility with Chromium's pinned compiler revisions.
3. For a custom native Rust toolchain, upstream declares `rust_sysroot_absolute` and `rustc_version`; `use_chromium_rust_toolchain` is computed, not a settable argument. Keep Rust enabled. Resolve native bindgen and remaining host tools too.
4. Generate an ARM64 component build, initially cap compile parallelism at 8 and heavy linking at 1, disable PGO/ThinLTO for iteration, retain sandboxing. These settings are a proposed starting point, not measured tuning.
5. Build native policy tests and an upstream browser baseline (`content_shell` followed by `chrome`), verify ELF AArch64, run sandboxed local-fixture smoke tests, then package runtime dependencies plus SHA256 manifests. Source/compiler pins, GN args and logs accompany artifacts.
6. Land and test actual provenance-gate patches. Baseline browser compilation alone does not provide AI filtering. Detector adapters remain prototypes pending real APIs and calibration.

## Primary sources at the pinned revision

- [Linux build requirements](https://chromium.googlesource.com/chromium/src/+/507c6ee3e2f3b2ca0e660547e5b9ea4820c67f4c/docs/linux/build_instructions.md)
- [Clang platform selection](https://chromium.googlesource.com/chromium/src/+/507c6ee3e2f3b2ca0e660547e5b9ea4820c67f4c/tools/clang/scripts/update.py)
- [Clang build implementation](https://chromium.googlesource.com/chromium/src/+/507c6ee3e2f3b2ca0e660547e5b9ea4820c67f4c/tools/clang/scripts/build.py)
- [Rust download selection](https://chromium.googlesource.com/chromium/src/+/507c6ee3e2f3b2ca0e660547e5b9ea4820c67f4c/tools/rust/update_rust.py)
- [Custom Rust toolchain arguments](https://chromium.googlesource.com/chromium/src/+/507c6ee3e2f3b2ca0e660547e5b9ea4820c67f4c/build/config/rust.gni)
