# chromium-provenance

Experimental overlay for [`yubi-OS/chromium`](https://github.com/yubi-OS/chromium), a clean mirror fork. The intended browser will gate rendering on configured AI-content signals, with an optional strict provenance mode. Current implementation contains a standalone policy engine and verifier scaffolding; browser interception and detector integration remain unbuilt.

Same model as Brave / ungoogled-chromium: the fork stays pristine, this repo pins a Chromium tag (`PINNED.md`) and carries the delta (`components/`, `patches/`, `policy/`, `verifier/`).

## What it can and cannot promise

- **Planned detection targets:** Anthropic Claude watermark, Google SynthID, OpenAI provenance signals, Adobe TrustMark, C2PA AI-action declarations and calibrated text classifiers. Access, actual schemas and calibration must be verified before enabling any adapter.
- **Cannot:** certify content is human-made. Every vendor states in writing that absence of a mark is not evidence of human authorship. Unwatermarked models, paraphrased text and pre-2026-08-02 Claude output are invisible to watermark detectors. We say so on the interstitial.

## Modes

| Mode | Default | Semantics |
|---|---|---|
| `block_on_detect` | yes | Allow unless a signal fires. Verifier down → allow + badge (fail-open). |
| `provenance_required` | toggle (global or per-origin) | Media must carry a valid, trusted C2PA manifest with no AI-generation action; text must pass the text stack; verifier down → block (fail-closed). Blocks most of today's web by design. |

## Layout

```
PINNED.md                    Chromium tag + fork SHA this overlay applies to
components/provenance_gate/  Policy engine (C++, Chromium style) + unit tests — new files, no upstream edits
patches/                     Ordered patch series against the pinned tag (SERIES.md describes each)
policy/                      Default policy JSON + schema
verifier/                    Cloudflare Worker: single endpoint the browser calls for remote/heavy checks
docs/adr/                    Architecture decision records
```

## Build

Linux ARM64 qualification runs on the org's `HIGH-MEM` self-hosted runner (12 CPUs, 62 GiB RAM observed). See [ARM64 CI](docs/arm64-linux-ci.md) for host setup, access restrictions, evidence and native toolchain limitations. The qualification run reached the runner but failed because compilers are absent and CI cannot install them without an administrator. Full Chromium builds and browser-level provenance enforcement are **not implemented or verified yet**. Existing green CI covers standalone policy logic and verifier scaffolding only.
