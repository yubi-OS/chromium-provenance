# chromium-provenance

Overlay for [`yubi-OS/chromium`](https://github.com/yubi-OS/chromium) (a clean mirror fork) that turns Chromium into a **provenance-gated browser**: a page renders only if no configured detector reports AI-generated content, with an optional strict mode that additionally requires valid C2PA provenance on media.

Same model as Brave / ungoogled-chromium: the fork stays pristine, this repo pins a Chromium tag (`PINNED.md`) and carries the delta (`components/`, `patches/`, `policy/`, `verifier/`).

## What it can and cannot promise

- **Can:** block pages where a detector fires — Anthropic Claude ("Fable" 5.1 / Mythos 5.1) text watermark, Google SynthID, OpenAI provenance signals, Adobe TrustMark, C2PA manifests recording AI actions, open-source zero-shot classifiers (Binoculars) above a conservative threshold.
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

Chromium needs ~1–3 h on 32+ cores / 64 GB. CI here only builds and tests the policy engine and the verifier; the full browser build runs on a self-hosted or GitHub large runner (see `docs/adr/ADR-000-provenance-gate.md` §4).
