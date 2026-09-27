# Linux ARM64 build lane

## Latest verified status, 2026-09-24

[ARM64 qualification run 36053621319](https://github.com/yubi-OS/chromium-provenance/actions/runs/36053621319) is green on org runner `ubuntu` (HIGH-MEM), overlay `6d9ebdb6ba1af140ea99e96a21088bda3f2ff3cb`. Native policy tests pass; the executable is ELF AArch64. All 4 verifier tests pass. [Evidence artifact](https://github.com/yubi-OS/chromium-provenance/actions/runs/36053621319/artifacts/10831830946).

Host packages were installed through the authenticated root shell bridge using the hash-verified installer from `839369e2`: clang/LLD 21.1.8, CMake 4.2.3, Ninja 1.13.2, pkg-config 2.5.1, Node 22.22.1, npm 9.2.0. Existing Rust is 1.93.1. Installer exited 0, executable prerequisite check passes, about 180 GiB free disk remains. No sudoers or persistent runner-privilege changes. Installation evidence is on the host under `/var/lib/chromium-provision/` (`bootstrap.sh`, `install.log`, `exit-code`), unit `chromium-host-deps-20260924`.

Ubuntu public Funnel TLS returned 525 while private-tailnet HTTPS worked. After the operator restarted tailscaled, root commands succeeded (one transient 525 remained before subsequent successful requests). The exact TLS cause was not proven. The installer ran as a separate systemd oneshot so the single-threaded HTTP bridge could still answer status queries. The launch request timed out; checking the unit and log confirmed it continued, so it was not relaunched.

**Scope:** this is host qualification, standalone C++ policy and verifier scaffold testing. Chromium source sync, native Chromium toolchain compatibility, a full browser binary and browser-level AI filtering remain pending. Installed distribution compiler versions are not proof of compatibility with Chromium 153.

## Initial qualification, 2026-09-09

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

First update that checkout to the reviewed commit containing the script (or download the script at that immutable commit). The installer uses the host's existing apt repositories, installs build/runtime development packages, and checks the resulting executables. It neither changes sudoers nor grants unattended root access. This setup was completed on 2026-09-24; see the latest verified status above.

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

## Toolchain gate closed, 2026-09-25

[Run 36199519712](https://github.com/yubi-OS/chromium-provenance/actions/runs/36199519712) green end-to-end on HIGH-MEM at overlay 'e083b1bf': throttled sync, CIPD gn fetch, gn gen, evidence upload. Zero host halts.

Three findings baked into the workflow:

1. **Throttle (mandatory on this host)** — the hardware halts under high I/O (user-reported; two syncs died this way before the fix). All heavy steps run under `nice -n 19`; the sync uses `ionice -c3` (idle class) and `gclient sync --jobs 4` (default is 32). Result: 27 GB tree synced in ~5 min with no halt. Keep this pattern for every future build step on the box.
2. **GN comes from CIPD, not bootstrap** — at pin '507c6ee3' (153.0.8010.36) `tools/gn/build/gen.py` does not exist; `tools/gn/bootstrap/bootstrap.py` is leftover dead code. Workflow fetches `gn/gn/linux-arm64` pinned to instance `NiNl49qPQkD69P3Ou9yieBc5pI9WN5v0Vso_IW0ySCwC` (verified ELF aarch64, gn 2577).
3. **LASTCHANGE.committime** — `compute_build_timestamp.py` needs `build/util/LASTCHANGE.committime`, produced by the `lastchange` hook that '--nohooks' skips. The workflow derives it from HEAD commit time.

**CIPD linux-arm64 gap:** four `${{platform}}` packages have no linux-arm64 variant at this pin: gperf (`infra/3pp/tools/gperf`), reclient (`infra/rbe/client`), android_toolchain, fuchsia sdk core. DEPS on the build box is patched to their linux-amd64 variants with `git update-index --assume-unchanged DEPS` (keeps gclient's dirty-tree guard green). Re-apply on any fresh checkout.

**gn gen result** (system clang 21.1.8, `clang_base_path=/usr`, component build, symbols off): `Done. Made 32194 targets from 5017 files in 3077ms`. Build directory: `/home/ubuntu/chromium-build/src/out/arm64-qual`.

**Next gate:** throttled native build — `ninja -C out/arm64-qual content_shell` first, then `chrome`; measure duration, then provenance-gate patches.


## Build gate progress, 2026-09-26

`arm64-chromium-build.yml` (commit `e94f8049`) runs throttled `ninja -C out/arm64-qual content_shell` on HIGH-MEM (manual dispatch, same `chromium-high-mem` concurrency group, evidence artifact on every outcome). Four fixes landed to get the pipeline executing; the current gate is crubit.

1. **Bindgen libclang mismatch** — the first build died at `[17/42090]` because bindgen loaded the downloaded toolchain's libclang, which predates flags Chromium passes at this pin (`-fdiagnostics-show-inlining-chain`, `-fno-lifetime-dse`). Fix: `rust_bindgen_root` now points at the freshly built native clang-24 host toolchain (`third_party/rust-toolchain-intermediate/llvm-host-install`) with `bin/bindgen` staged there and a `bin/rustfmt` wrapper (native stage1-tools rustfmt, `LD_LIBRARY_PATH` to `stage1/lib` for `librustc_driver`). Failing action validated green.
2. **Runner group repo membership** — dispatched runs sat queued because the `chromium-high-mem` runner group had no repositories selected. Fixed by the operator in the org settings (2026-09-26); first CI pickup immediately after. Include the repo when creating a restricted runner group.
3. **Root-owned build files** — `build.ninja` was root-owned 0600 after an out-of-CI `gn gen`, failing CI with `loading 'build.ninja': Permission denied`. `chown -R ubuntu:ubuntu /home/ubuntu/chromium-build` repaired it. Rule: any manual tree work on HIGH-MEM runs as the `ubuntu` user (`sudo -u ubuntu`), never as root.
4. **Split-metadata .rmeta companions** — the run failed at `[195/42073]` with `only metadata stub found for 'rlib' dependency 'proc_macro'`. The toolchain sysroot's aarch64 `libproc_macro-681e7cea8be22eb0.rlib` is a split-metadata stub whose companion was missing; the full pair exists in `third_party/rust-src/build/aarch64-unknown-linux-gnu/stage1-std/.../dist/build/proc_macro/681e7cea8be22eb0/out/`. Copied the companion `.rmeta` files (proc_macro, rustc_literal_escaper) next to the stubs in `third_party/rust-toolchain/lib/rustlib/aarch64-unknown-linux-gnu/lib/`. Validated green.
5. **Crubit `cc_bindings_from_rs` is x86_64-only — current gate.** The next run reached `[418/41902]` (proc_macro2 green, local rustc sysroot std building) and failed on `OSError: Exec format error` for `third_party/rust-toolchain/bin/cc_bindings_from_rs` (x86_64 ELF). It generates `rs_core.h`/`rs_alloc.h`/`rs_std.h` under `enable_cpp_api_from_rust`, unconditionally on for chromium builds using the chromium rust toolchain. Forcing the flag off breaks `gn gen` (`font_format_bindings` unresolved under `//third_party/blink/renderer/platform`) — the flag-off graph is untested upstream at this pin, and crubit source is not vendored in the tree.

Next-gate options: (a) build `cc_bindings_from_rs` from crubit source for aarch64, (b) patch the graph to drop `cpp_api_from_rust` (blink `font_format_bindings` plus every consumer — whack-a-mole risk), (c) source an upstream aarch64 crubit prebuilt.

Attempt log (all on HIGH-MEM, pinned `507c6ee3`, overlay `e94f8049`): 36271046440 (Permission denied, fixed), 36271274831 (proc_macro stub, fixed), 36272127419 (cc_bindings_from_rs, current gate).


## Route (a) executed: native cc_bindings_from_rs, 2026-09-26/27

`cc_bindings_from_rs` is a pure-Rust cargo binary (crubit's Bazel/LLVM weight is only for the reverse-direction `rs_bindings_from_cc`), so it builds natively on the aarch64 host. Route (c) — an upstream prebuilt — was ruled out first: crubit publishes no release binaries and its CI is x86_64-only, no distro packages it on any arch, and Chromium's toolchain CIPD has no `Linux_arm64` variant at the pin or on main. Full survey: `refs/cc-bindings-from-rs-aarch64-2026-09-26.md` on `yubi-OS/yubiOS` (commit `71e8c6da`).

Fixes landed on HIGH-MEM to get here:

1. **bindgen → native clang-24 toolchain**: `rust_bindgen_root` repointed at `third_party/rust-toolchain-intermediate/llvm-host-install`, with `bin/bindgen` (0.73.2 aarch64) staged there + a `bin/rustfmt` wrapper.
2. **Split-metadata `.rmeta` companions**: the sysroot's `libproc_macro-681e7cea8be22eb0.rlib` was a stub without its companion; copied the `.rmeta` from the stage1-std dist (same hash).
3. **The consistent toolchain rebuild**: the installed toolchain was piecemeal (aarch64 rustc/cargo swapped into an x86_64 package) and had NO rustc-dev at all. Fixed by a full `build_rust.py --skip-checkout` (build + `x.py install --stage 2`): the extended install ships rustc-dev + std + cargo/clippy/rustfmt/rust-analyzer, all self-compiled by the pinned nightly (`0913b18e`). **Caveat: the install wipes the toolchain `bin/` — RE-STAGE bindgen + rustfmt into `llvm-host-install/bin/` after any toolchain re-install** (they are not part of the installer's component set).
4. **build_crubit.py green**: `cc_bindings_from_rs` compiled + installed into the toolchain (aarch64 ELF), with `crubit/support` + the gn files at `lib/third_party/crubit/`.
5. **Runtime fixes for the generated bindings**: `librustc_driver-8cdff714e4d6ff45.so` installed (the toolchain lib only had `c6de4f5a`/`d48c6bde`), and `buildtools/linux64-format/clang-format` swapped for the aarch64 build (the x86_64 one kept as `clang-format.x64`).

Also fixed in `build_rust.py` (aarch64-native support): `RustTargetTriple` returns aarch64 on aarch64 and `VendorForStdlib` uses the system cargo (pre-existing patches), plus NEW: `DownloadDebianSysroot('amd64')` is now arch-aware (arm64 on aarch64 hosts) and `AddOpenSSLToEnv` is skipped on aarch64 (the 3pp package is linux-amd64 OpenSSL 1.1.1 — too old and wrong arch; the system OpenSSL 3.5.5 via pkg-config works).

Result: the `rs_core.h` crubit generation target is GREEN on-box, and CI run 36290640690 is building the full content_shell graph. Failure-history during this stretch: E0514 (dev libs compiled by the stage0 beta — fixed by the full toolchain rebuild), E0463 `std` (sysroot gutted during the repair — restored), E0463 `quote`/`proc_macro2`/`syn` in the crubit support proc macros (status was being re-verified in CI run 36290640690), the bindgen-wipe (re-staged).
