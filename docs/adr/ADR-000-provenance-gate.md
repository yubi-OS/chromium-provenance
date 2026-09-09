# ADR-000: Provenance-gated Chromium

**Date:** 2026-09-09 · **Status:** DRAFT, awaiting policy-mode decision
**Fork:** https://github.com/yubi-OS/chromium (created 2026-09-09T20:44Z, `default_branch_only`, mirror of chromium/chromium main, 66 GB)
**Overlay repo (proposed):** `yubi-OS/chromium-provenance` — patches + policy + build recipe, Brave/ungoogled-chromium model. The fork stays a clean mirror; the overlay pins a Chromium tag and applies patches.

## 1. What the user asked for, and what is physically possible

> "only show a webpage if it does not contain any AI generated content"

No detector on earth can certify **absence** of AI content. Every vendor says so in writing (Anthropic, Google, OpenAI). What CAN be built, honestly:

| Mode | Semantics | Achievable? |
|---|---|---|
| **A. Block-on-detect** | Render unless some detector returns *positive* (watermark found, C2PA manifest records an AI action, classifier over threshold). | Yes. Misses unwatermarked / unlabeled AI content. |
| **B. Provenance-required (fail-closed)** | Render media only if it carries a *valid, trusted* C2PA manifest whose actions record no AI generation. Text rendered only if it passes the text stack. Everything else → interstitial. | Yes, but blocks ~all of today's web (few sites sign C2PA). Useful as an opt-in "strict" toggle. |
| C. "Certified human" | Prove content is human-made. | **No.** Not offered; anyone claiming it is lying. |

Recommendation: ship **A as default + B as a per-site/global strict toggle**, with a clear interstitial explaining *which* signal fired and its confidence. Detector output is evidence, not a verdict.

## 2. Detection stack (industry-leading, as of 2026-09-09)

### Text
| Source | What it catches | Access | Where it runs |
|---|---|---|---|
| **Anthropic Claude watermark ("Fable")** — Claude Fable 5.1 / Mythos 5.1 (models ≥ 2026-08-02, EU AI Act) | Statistical token-choice watermark; probabilistic; weak on short/code/heavily-edited text | **Private-preview API**, gated to regulators / media / fact-checkers / independent researchers / edu / EU civil society. Application required. Older Claude models (Opus 5, Sonnet 5, Fable 5) **not watermarked yet**. | Remote API via our verifier service |
| **Google SynthID Text** | Gemini-family watermark | Open-source reference detector (google-deepmind/synthid-text, Python/Transformers) — needs watermark key config; Google Cloud **AI Content Detection API** (2026) for hosted | Remote (Cloud API) or server-side |
| **OpenAI content provenance** | OpenAI image/audio signals (C2PA/SynthID) — text not covered | `POST /v1/content_provenance_checks`, hosted | Remote |
| **Binoculars** (open source, ~0.01% FPR @ >90% TPR on paper datasets) | Unwatermarked LLM text, any vendor | Two-LLM zero-shot; GPU/server | Server-side (our verifier) |
| GPTZero (commercial, closed, vendor-claimed 0.08–0.9% FPR) | Unwatermarked text | Paid API | Optional remote |

### Images / video / audio
| Source | Access | Where it runs |
|---|---|---|
| **C2PA Content Credentials** — `c2pa-rs` (Rust, MIT/Apache-2) | Open source | **In-process in Chromium** (Chromium already builds Rust). Validate manifest, signer trust list, look for `c2pa.created` with `digitalSourceType = trainedAlgorithmicMedia` etc. |
| **Adobe TrustMark** (MIT; Python/Rust/JS-ONNX decoder) | Open source | In-process (ONNX Runtime / TFLite already in Chromium) |
| **Google SynthID image/audio/video** | Google-hosted (SynthID Detector portal, Cloud API) — no open multimodal detector | Remote |
| **Meta Video Seal / Stable Signature** | Open weights, research code | Server-side verifier (model conversion later) |

### Verifier service
Remote/heavy checks go through **one** endpoint we control (Cloudflare Worker + GPU backend later), so the browser never sends page content to five vendors. Privacy: hash-and-cache; only send text spans that exceed a length floor; never send from private-mode by default.

## 3. Architecture in Chromium

Minimal-patch surface (each a separate commit in the overlay):

1. **`components/provenance_gate/`** (new) — policy engine. Inputs: per-resource verdicts. Output: `ALLOW | BLOCK(reason, confidence, signal)`. Prefs: mode (A/B), per-origin allowlist, thresholds.
2. **Network interception** — `URLLoaderThrottle` for main-frame HTML + subresource images/video/audio. Media: run c2pa-rs + TrustMark in a utility process (sandboxed, like `data_decoder`). Text: extract main-content text after parse (Blink hook post-`DOMContentLoaded`, before first paint commit), send to verifier, hold the commit until verdict or timeout.
3. **Interstitial** — reuse `security_interstitials` framework (same as Safe Browsing pages). Shows signal, confidence, "proceed anyway" (configurable off in strict mode).
4. **Fail-behaviour** — verifier unreachable ⇒ mode A: allow + badge; mode B: block.
5. **Omnibox indicator** — provenance chip (C2PA-verified / unknown / AI-detected).

Everything else is stock Chromium at a pinned tag.

## 4. Build reality

- Full Chromium build: ~1–3 h on 32+ cores / 64 GB RAM. **Not** possible on GitHub-hosted standard runners (6 h cap, 4 cores, 16 GB) or rock1.
- Options: (a) GitHub **larger runners** (64-core, paid), (b) self-hosted x86 builder registered to the org, (c) Cloudflare-side build farm. Pick one before phase 3.
- Until then, phases 1–2 develop against `content_shell` / component build on a dev box, and CI runs unit tests for `components/provenance_gate/` only.

## 5. Phases

| # | Deliverable | Gate |
|---|---|---|
| 0 | ✅ Fork created | done |
| 1 | Overlay repo scaffold: `PINNED.md` (Chromium tag), `patches/`, `policy/`, ADR-000 (this plan), CONSTRAINTS.md | approval to create repo |
| 2 | Verifier service (Cloudflare Worker): C2PA + TrustMark + Binoculars adapter + Anthropic/Google adapters behind feature flags; test corpus with planted watermarks | passes planted-signal tests, FPR measured |
| 3 | Chromium patches 1–5 above; builds `content_shell` with gate | self-hosted/large builder online |
| 4 | Full `chrome` build, Linux x86_64 first (yubiOS target), ARM64 after | CI green |
| 5 | Apply for Anthropic watermark-detection preview as independent researcher (yubi-OS org) | application sent |

## 6. Known limits to state in the README

- Absence of a mark ≠ human-authored (Anthropic, Google, OpenAI all say this).
- Watermarks only cover models that emit them; Claude < 2026-08-02, most open-weight models, and paraphrased text are invisible.
- Text classifiers have real false positives on non-native English writers and short text — mode A must never hard-block on classifier alone below a length floor.
- Strict mode (B) will block most of the current web. That is the point of the toggle, and the interstitial must say so.

## Sources
- https://www.anthropic.com/news/claude-text-watermark · https://support.claude.com/en/articles/16266773-how-claude-marks-ai-generated-content
- https://github.com/google-deepmind/synthid-text · https://ai.google.dev/responsible/docs/safeguards/synthid
- https://github.com/contentauth/c2pa-rs · https://github.com/adobe/trustmark
- https://arxiv.org/abs/2401.12070 (Binoculars) · https://github.com/facebookresearch/meta-seal
- https://developers.openai.com/api/docs/guides/content-provenance
