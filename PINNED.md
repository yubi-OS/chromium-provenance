# Pinned upstream

| Component | Pin | Source |
|---|---|---|
| Chromium stable (Linux) | `156.0.8078.47` | chromiumdash fetch_releases, channel=Stable, platform=Linux, 2026-09-09 |
| Chromium commit | `1d17a2e2ee147bb7d6ae2ab6abdb3a2510c17775` | same |
| Fork | https://github.com/yubi-OS/chromium (mirror of chromium/chromium, main only, created 2026-09-09T20:44:29Z) | GitHub API |
| c2pa-rs | TBD (pin on first vendoring) | https://github.com/contentauth/c2pa-rs |
| adobe/trustmark | TBD | https://github.com/adobe/trustmark |
| Binoculars | TBD | https://github.com/ahans30/Binoculars |

Bump procedure: update this table, regenerate `patches/` against the new tag, run `verifier` + `components` tests, open a PR titled `chore(pin): chromium <version>`.
