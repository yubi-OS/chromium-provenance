# Patch series: Chromium 156.0.8078.47

Base: Chromium M156 commit 1d17a2e2ee147bb7d6ae2ab6abdb3a2510c17775 (2026-10-19), tag 156.0.8078.47.

Generated with git format-patch --binary from the provenance-gate branch (box tree). Apply in filename order against the base with git apply.

| # | Patch | Subject |
|---|---|---|
| 0001 | `0001-build-add-provenance_gate-policy-engine-component.patch` | Subject: [PATCH 01/39] build: add provenance_gate policy engine component |
| 0002 | `0002-network-provenance-gate-throttle-wired-into-content_.patch` | Subject: [PATCH 02/39] network: provenance gate throttle wired into |
| 0003 | `0003-network-provenance-gate-cancels-blocked-loads.patch` | Subject: [PATCH 03/39] network: provenance gate cancels blocked loads |
| 0004 | `0004-policy-provenance-mode-switch-for-content_shell.patch` | Subject: [PATCH 04/39] policy: --provenance-mode switch for content_shell |
| 0005 | `0005-blink-report-main-content-page-text-to-the-browser-s.patch` | Subject: [PATCH 05/39] blink: report main-content page text to the browser |
| 0006 | `0006-data_decoder-C2PA-manifest-inspection-first-cut-seri.patch` | Subject: [PATCH 06/39] data_decoder: C2PA manifest inspection first cut |
| 0007 | `0007-provenance-wire-C2PA-inspection-into-the-gate-series.patch` | Subject: [PATCH 07/39] provenance: wire C2PA inspection into the gate (series |
| 0008 | `0008-provenance-defer-the-img-walk-off-the-DidFinishLoad-.patch` | Subject: [PATCH 08/39] provenance: defer the img walk off the DidFinishLoad |
| 0009 | `0009-chrome-integrate-the-provenance-gate-into-the-full-b.patch` | Subject: [PATCH 09/39] chrome: integrate the provenance gate into the full |
| 0010 | `0010-ui-omnibox-provenance-chip-series-0011.patch` | Subject: [PATCH 10/39] ui: omnibox provenance chip (series 0011) |
| 0011 | `0011-ui-provenance-chip-persists-across-navigations-serie.patch` | Subject: [PATCH 11/39] ui: provenance chip persists across navigations (series |
| 0012 | `0012-ui-provenance-interstitial-replaces-the-blocked-load.patch` | Subject: [PATCH 12/39] ui: provenance interstitial replaces the blocked-load |
| 0013 | `0013-policy-live-modes-persistent-exemptions-prefs-sync-s.patch` | Subject: [PATCH 13/39] policy: live modes + persistent exemptions + prefs sync |
| 0014 | `0014-policy-soft-block-images-mode-with-in-page-notices-s.patch` | Subject: [PATCH 14/39] policy: soft-block-images mode with in-page notices |
| 0015 | `0015-ui-disable-AI-Mode-entry-points-and-remove-the-AI-se.patch` | Subject: [PATCH 15/39] ui: disable AI Mode entry points and remove the AI |
| 0016 | `0016-policy-session-vs-persistent-exemptions-chrome-setti.patch` | Subject: [PATCH 16/39] policy: session-vs-persistent exemptions + |
| 0017 | `0017-fix-settings-restore-AddSettingsPageUIHandler-body-0.patch` | Subject: [PATCH 17/39] fix(settings): restore AddSettingsPageUIHandler body - |
| 0018 | `0018-ui-settings-provenance-redesign-settings-box-section.patch` | Subject: [PATCH 18/39] ui: settings/provenance redesign - settings-box |
| 0019 | `0019-ui-settings-provenance-Add-button-inline-with-the-si.patch` | Subject: [PATCH 19/39] ui: settings/provenance - Add button inline with the |
| 0020 | `0020-ui-kill-AI-Mode-omnibox-entrypoint-via-IsAimEligible.patch` | Subject: [PATCH 20/39] ui: kill AI Mode omnibox entrypoint via IsAimEligible - |
| 0021 | `0021-ui-interstitial-drop-the-second-sentence-claim-unver.patch` | Subject: [PATCH 21/39] ui: interstitial - drop the second sentence |
| 0022 | `0022-ui-provenance-chip-becomes-a-smart-chip-OmniboxChipB.patch` | Subject: [PATCH 22/39] ui: provenance chip becomes a smart chip - |
| 0023 | `0023-ui-provenance-chip-center-the-label-and-lead-with-th.patch` | Subject: [PATCH 23/39] ui: provenance chip - center the label and lead with |
| 0024 | `0024-policy-settings-radio-fires-selected-changed-not-cha.patch` | Subject: [PATCH 24/39] policy: settings radio fires selected-changed not |
| 0025 | `0025-policy-per-image-URL-evidence-so-soft-mode-notices-o.patch` | Subject: [PATCH 25/39] policy: per-image-URL evidence so soft-mode notices |
| 0026 | `0026-parser-WebP-RIFF-C2PA-support-CollectWebPC2PA-walks-.patch` | Subject: [PATCH 26/39] parser: WebP RIFF C2PA support (CollectWebPC2PA walks |
| 0027 | `0027-renderer-reduce-GetBlockedImages-retry-delay-from-2s.patch` | Subject: [PATCH 27/39] renderer: reduce GetBlockedImages retry delay from 2s |
| 0028 | `0028-series-0017-on-device-AI-model-blocking-provenance.b.patch` | Subject: [PATCH 28/39] series 0017: on-device AI model blocking |
| 0029 | `0029-series-0018-rebrand-the-browser-to-Antimony-BRANDING.patch` | Subject: [PATCH 29/39] series 0018: rebrand the browser to Antimony (BRANDING |
| 0030 | `0030-series-0018-fixup-icons-into-default_100-200_percent.patch` | Subject: [PATCH 30/39] series 0018 fixup: icons into default_100/200_percent |
| 0031 | `0031-series-0018-fixup-2-omnibox-secure-chip-Antimony-vec.patch` | Subject: [PATCH 31/39] =?UTF-8?q?series=200018=20fixup=202:=20omnibox=20se?= |
| 0032 | `0032-series-0018-fixup-3-omnibox-chip-icon-the-omnibox-na.patch` | Subject: [PATCH 32/39] =?UTF-8?q?series=200018=20fixup=203:=20omnibox=20ch?= |
| 0033 | `0033-series-0018-fixup-4-rotate-the-Antimony-chip-icon-90.patch` | Subject: [PATCH 33/39] series 0018 fixup 4: rotate the Antimony chip icon |
| 0034 | `0034-series-0018-fixup-5-settings-help-copyright-notice-r.patch` | Subject: [PATCH 34/39] series 0018 fixup 5: settings/help copyright notice |
| 0035 | `0035-series-0018-fixup-6-settings-help-license-line-only-.patch` | Subject: [PATCH 35/39] =?UTF-8?q?series=200018=20fixup=206:=20settings/hel?= |
| 0036 | `0036-series-0019-deb-packaging-identity-antimony-chromium.patch` | Subject: [PATCH 36/39] =?UTF-8?q?series=200019:=20deb=20packaging=20identi?= |
| 0037 | `0037-debian-packaging-novendor-icon-registration-warn-onl.patch` | Subject: [PATCH 37/39] debian packaging: --novendor icon registration + |
| 0038 | `0038-cross-build-add-clang_arm64_v8_x64-toolchain-def-amd.patch` | Subject: [PATCH 38/39] cross-build: add clang_arm64_v8_x64 toolchain def + |
| 0039 | `0039-series-0023-aarch64-host-build-env-native-apt-gperf-.patch` | Subject: [PATCH 39/39] series 0023: aarch64 host build env (native apt gperf |
| 0040 | `SERIES.md` |  |
