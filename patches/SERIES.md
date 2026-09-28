# Patch series (against PINNED.md Chromium tag)

Generated with `git format-patch` from the working branch on the fork. 0001-0003 + 0005 authored 2026-09-28 on the HIGH-MEM checkout (provenance-gate branch, commits c7ac21abe2 / 71168f9b07 / 1f96ba5fda / 800365457d) from the real tree. 0004 + 0006 + 0007 pending.

| # | File | Touches | Purpose |
|---|---|---|---|
| 0001 | `0001-build-add-provenance_gate-policy-engine-component.patch` ✅ AUTHORED | `components/BUILD.gn`, `chrome/browser/BUILD.gn` | Wire `//components/provenance_gate` + unit_tests into the build |
| 0002 | `0002-network-provenance-gate-throttle-wired-into-content_.patch` ✅ AUTHORED | `chrome/browser/…/chrome_content_browser_client.cc`, new `provenance_gate_throttle.{h,cc}` | `URLLoaderThrottle` for main-frame HTML + image/video/audio subresources; defers commit until `Decision` |
| 0003 | `0003-ui-provenance-interstitial.patch` ✅ AUTHORED (content_shell adaptation: throttle cancels blocked loads with `ERR_BLOCKED_BY_CLIENT` + stable reason; shell shows its network error page — the chrome interstitial with signal/source/confidence/proceed-anyway lands with the chrome port) | `components/security_interstitials/`, `chrome/browser/…/interstitials/` | Interstitial page (signal, source, confidence, proceed-anyway when `overridable`) |
| 0004 | `0004-blink-main-content-text-extraction.patch` | `third_party/blink/renderer/core/…` | Post-parse main-content text extraction, 800-char floor, sends to browser process |
| 0005 | `0005-prefs-and-policy.patch` ✅ AUTHORED (content_shell adaptation: `--provenance-mode=block-on-detect\|provenance-required` switch supplies the Policy; chrome prefs + enterprise policy templates land with the chrome port) | `chrome/common/pref_names.h`, `components/policy/` | Prefs: mode, per-origin allowlist, verifier origin; enterprise policy templates |
| 0006 | `0006-omnibox-provenance-chip.patch` | `chrome/browser/ui/views/location_bar/` | Omnibox chip: provenance valid / unknown / AI detected |
| 0007 | `0007-utility-c2pa-trustmark-decoders.patch` | `services/data_decoder/`, `third_party/rust/` | Sandboxed utility-process adapters over vendored c2pa-rs + TrustMark ONNX |

Validated end-to-end on the box (2026-09-28): mode A allows (`allow=1 reason=no_signal` at VERBOSE1 on example.com), mode B blocks (`allow=0 reason=no_trusted_provenance`, ERR_BLOCKED_BY_CLIENT, the shell's error page renders). The CI applies `patches/0*.patch` idempotently before ninja (b3b0d592).
