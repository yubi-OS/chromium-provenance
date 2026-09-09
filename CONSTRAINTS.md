# CONSTRAINTS

Quality bar for this repo. Weakening any line requires an ADR.

1. **No "human-verified" claim anywhere in UI or docs.** Only `detected`, `provenance valid`, `provenance invalid`, `no signal`, `verifier unavailable`.
2. **Classifier-only blocks require ≥ 800 chars of extracted text and confidence ≥ 0.98**, and are always user-overridable in `block_on_detect`.
3. **Watermark or C2PA-AI-action hits block at confidence ≥ 0.5** (vendor-calibrated).
4. **Fail-open in `block_on_detect`, fail-closed in `provenance_required`.** Never silently the other way.
5. **Privacy:** the browser talks to exactly one verifier origin; text below 800 chars is never sent; nothing is sent from Incognito unless the user opts in.
6. **Every detector adapter has a planted-signal positive test and a clean-corpus negative test with a measured FPR** before it can be enabled by default.
7. **Policy engine is pure:** `Evaluate()` has no I/O, no globals, deterministic.
8. Upstream Chromium files are only touched via `patches/`; new code lives in `components/provenance_gate/`.
9. No `@ts-ignore`, no skipped tests, no lowered thresholds without an ADR.
