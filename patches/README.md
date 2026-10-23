# Patches

Chromium provenance-gate series for Antimony. Two series live here, one per Chromium pin.

- `patches-153/`: the 22-patch series and SERIES.md for Chromium 153.0.8010.36 (commit 507c6ee3e2). Kept for history. CI no longer applies it.
- `patches-156/`: the 39-patch series and SERIES.md for Chromium 156.0.8078.47, base commit `1d17a2e2ee147bb7d6ae2ab6abdb3a2510c17775`. Exported with `git format-patch --binary`. `arm64-chromium-build.yml` applies `patches/patches-156/0*.patch` in filename order.

## aarch64 host environment (patch 0039)

Patch 0039 removes the `src/third_party/gperf/cipd` entry from DEPS, because CIPD has no linux-arm64 gperf package. On an arm64 host, build against the apt gperf instead:

```
sudo apt-get install -y gperf
mkdir -p third_party/gperf/cipd/bin
ln -sf /usr/bin/gperf third_party/gperf/cipd/bin/gperf
```

The `reclient`, Android NDK and Fuchsia DEPS entries are not fetched on this host, so they keep their upstream `${{platform}}` form.

Status: apt gperf 3.3 produced byte-identical output to the old CIPD binary on three sample `.gperf` inputs. A full build with the apt gperf has not been run yet. The r1 release was built with the old CIPD binary.
