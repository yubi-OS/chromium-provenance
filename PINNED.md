# Pinned upstream

| Component | Pin | Source |
|---|---|---|
| Chromium stable (Linux) | `153.0.8010.36` | chromiumdash fetch_releases, channel=Stable, platform=Linux, 2026-09-09 |
| Chromium commit | `507c6ee3e2f3b2ca0e660547e5b9ea4820c67f4c` | same |
| Fork | https://github.com/yubi-OS/chromium (mirror of chromium/chromium, main only, created 2026-09-09T20:44:29Z) | GitHub API |
| c2pa-rs | TBD (pin on first vendoring) | https://github.com/contentauth/c2pa-rs |
| adobe/trustmark | TBD | https://github.com/adobe/trustmark |
| Binoculars | TBD | https://github.com/ahans30/Binoculars |

Bump procedure: update this table, regenerate `patches/` against the new tag, run `verifier` + `components` tests, open a PR titled `chore(pin): chromium <version>`.
