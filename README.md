# Antimony

Antimony is a Linux web browser built on Chromium 153.0.8010.36 with an **engine-layer content-provenance gate**: C2PA manifest inspection runs inside the browser (C++ in the data-decoder service), and the browser can block, soft-block, or require provenance for AI-generated content — configurable, user-overridable, and fail-closed in strict mode. There is no extension and no proxy; the gate lives in the browser itself.

Development follows the Brave / ungoogled-chromium model: [`yubi-OS/chromium`](https://github.com/yubi-OS/chromium) is a clean mirror fork that is never patched directly, and this repo pins a Chromium tag (`PINNED.md`) and carries the delta (`components/`, `patches/`, `policy/`, `verifier/`, `demo/`).

**Status: prerelease.** The first packages shipped 2026-10-07 as [`antimony-v153.0.8010.36-r2`](https://github.com/yubi-OS/chromium-provenance/releases/tag/antimony-v153.0.8010.36-r2). This is not production-ready software; the honest boundaries are listed below and enforced in `CONSTRAINTS.md`.

## Install (prerelease r2)

| Package | Architecture | Size | SHA256 |
|---|---|---|---|
| [antimony-stable_153.0.8010.36-1_amd64.deb](https://github.com/yubi-OS/chromium-provenance/releases/download/antimony-v153.0.8010.36-r2/antimony-stable_153.0.8010.36-1_amd64.deb) | x86_64 | 120.7 MB | `3ffedd4165db5fa6a29e318bdfa4d1eaeebb36d7b91afabcb6a84429db1bbea0` |
| [antimony-stable_153.0.8010.36-1_arm64.deb](https://github.com/yubi-OS/chromium-provenance/releases/download/antimony-v153.0.8010.36-r2/antimony-stable_153.0.8010.36-1_arm64.deb) | arm64 | 107.7 MB | `1046f0f5c809e1e3785d77f2013f2cc4ce64b3225335ee1abce94be02631e763` |
| [antimony-stable-153.0.8010.36-1.x86_64.rpm](https://github.com/yubi-OS/chromium-provenance/releases/download/antimony-v153.0.8010.36-r2/antimony-stable-153.0.8010.36-1.x86_64.rpm) | x86_64 | 122.1 MB | `cdfc72060e48c0b4cc7902edc53707d63f1ae32a368633753b33b2552f87e518` |
| [antimony-stable-153.0.8010.36-1.aarch64.rpm](https://github.com/yubi-OS/chromium-provenance/releases/download/antimony-v153.0.8010.36-r2/antimony-stable-153.0.8010.36-1.aarch64.rpm) | arm64 | 109.1 MB | `5472c7a92a9d66bef89daf2fcdc7b506215b938902d1bf278a155cea00ba2c47` |

Verify before installing:

```sh
sha256sum antimony-stable_153.0.8010.36-1_amd64.deb
```

All four are official builds (`is_official_build=true`, thin LTO + CFI) with stripped inner binaries, built from this repo's patch series pinned at Chromium 153.0.8010.36. The deb installs to `/opt/antimony` under the package name `antimony`.

## What the gate does

- **C2PA content-credential inspection** shipped: JPEG APP11, PNG caBX, and WebP RIFF JUMBF manifests are parsed in-process; images carrying unverified AI-generation claims can be gated.
- **Four modes** (default `block_on_detect`; change in `chrome://settings/provenance`):

| Mode | Semantics |
|---|---|
| `off` | Gate disabled. |
| `block_on_detect` (default) | Render unless a signal fires. Verifier down → allow + badge (fail-open). |
| `soft_block_images` | Gate-flagged images are replaced in-page with same-dimension notices and a per-page Allow action. |
| `provenance_required` | Media must carry a valid, trusted C2PA manifest with no AI-generation action; verifier down → block (fail-closed). Blocks most of today's web by design. |

- **Bypass scoping**: interstitial Proceed and soft-mode Allow create session- or page-scoped exemptions; persistent per-origin exemptions are managed only in the settings page.
- **On-device model toggle**: optionally removes the browser's on-device AI models (Writer/Rewriter/Summarizer) from the scripting context, with its own exemption list.
- **Omnibox provenance chip** shows gate state per page and opens the settings page.

## What it can and cannot promise

- **Shipped detector:** C2PA AI-action declarations (unverified-manifest gating).
- **Planned detection targets** (not yet enabled): Google SynthID, OpenAI provenance signals, Adobe TrustMark, the Anthropic Claude text watermark (private-preview API, application required), calibrated text classifiers, and a Binoculars backend (disabled until its false-positive rate is measured). Access, actual schemas and calibration must be verified before enabling any adapter.
- **Cannot:** certify content is human-made. Every vendor states in writing that absence of a mark is not evidence of human authorship. Unwatermarked models, paraphrased text and pre-2026-08-02 Claude output are invisible to watermark detectors. The interstitial says so.
- **Never claimed:** "human-verified". Classifier-only blocks use bounded floors (≥800 characters, ≥0.98 confidence) and stay user-overridable.

## Try it

The `demo/` directory carries a test page with a synthetic C2PA-tagged image and a human-generated control. Serve it locally, load it in Antimony, and watch the gate fire in `block_on_detect` or `soft_block_images` mode. The full block-chain demo (scan → evidence → block → interstitial) runs end-to-end on the built browser.

## Layout

```
PINNED.md                    Chromium tag + fork SHA this overlay applies to
components/provenance_gate/  Policy engine + gate (C++, Chromium style) + unit tests
patches/                     Ordered patch series against the pinned tag (SERIES.md describes each)
policy/                      Default policy JSON + schema
verifier/                    Cloudflare Worker: single endpoint the browser calls for remote/heavy checks
demo/                        Gate test pages (synthetic C2PA image + human control)
docs/                        CI, build-args, and cross-build documentation
scripts/                     Build and verification scripts
```

## Build and CI

The arm64 Chromium build runs on the org's self-hosted ARM64 runner, and the amd64 build is produced cross-target from the same tree; see [ARM64 CI](docs/arm64-linux-ci.md), the release-args reference (`docs/arm64-release-args.gn`), and [AMD64 cross-build](docs/amd64-cross-build.md) for the toolchain story, including the missing Linux host-toolchain definition this fork had to add. CI on this repo validates the policy engine, the verifier, and the falsification harnesses; full-browser builds are produced on the dedicated build host.

## Known gaps

- **Chromium base is pinned at 153.0.8010.36.** The rebase cadence is tracked in `PINNED.md`; this README makes no claim that Antimony is security-current with upstream Chromium. Check upstream advisories for CVEs fixed after the pin.
- **Detector coverage is partial.** Only the C2PA path is enabled today; the deferred detectors above have not shipped.
- **Packages are not GPG-signed yet.** Verify with the SHA256 table above; signing infrastructure is on the roadmap.
- **False-positive rate is not yet measured on a public corpus.** Report gate false positives as issues; a dedicated reporting template is planned.
- CI compiles `content_shell`; the full chrome target is validated on the build host and by real-hardware testing rather than in CI.

## Independence

Antimony is an independent open-source project by [yubi-OS](https://github.com/yubi-OS). It is not affiliated with, endorsed by, or certified by Google LLC. Chromium is a trademark of Google LLC; the upstream attribution ("Antimony is made possible by the Chromium open source project") is preserved in the product.
