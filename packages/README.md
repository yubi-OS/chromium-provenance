# packages/ — release binaries for the yubiOS build

## provenance-content-shell-arm64-2ece72f2fa.tar.zst

Provenance-gated content_shell (Chromium 153.0.8010.36) runtime bundle,
built on the HIGH-MEM aarch64 runner from yubi-OS/chromium fork commit
2ece72f2fa (provenance-gate branch) with overlay patches 0001-0007 applied.
Split into 6 parts (GitHub git blobs cap at 100 MB).

## Reassembly

```
cat provenance-content-shell-arm64-2ece72f2fa.tar.zst.part-00 provenance-content-shell-arm64-2ece72f2fa.tar.zst.part-01 provenance-content-shell-arm64-2ece72f2fa.tar.zst.part-02 provenance-content-shell-arm64-2ece72f2fa.tar.zst.part-03 provenance-content-shell-arm64-2ece72f2fa.tar.zst.part-04 provenance-content-shell-arm64-2ece72f2fa.tar.zst.part-05 > provenance-content-shell-arm64-2ece72f2fa.tar.zst
```

## Verify (the yubiOS build is immutable — always verify)

```
sha256sum -c SHA256SUMS
```

Full bundle: 225861970 bytes,
`4e777d45482d312dabaa1ff7c499d0b5738849101bdd9e4d2b01c5de37fc87d8`.
Per-part digests are in SHA256SUMS; the bundle carries its own per-file
MANIFEST.sha256 inside release-bundle/ after extraction.

## Use

```
tar --zstd -xf provenance-content-shell-arm64-2ece72f2fa.tar.zst
cd release-bundle
./content_shell --ozone-platform=headless --provenance-mode=block_on_detect <url>
```

Contents: content_shell (17.7 MB aarch64 ELF) + 434 Chromium shared libs
(rpath $ORIGIN, flat in lib/) + icudtl.dat, v8_context_snapshot.bin, .pak
resources, locales/ + MANIFEST.sha256 + provenance.txt. System libs
(asound, atk, cairo, ...) come from the distro.

Also tagged as release
[content-shell-v0.1.0](https://github.com/yubi-OS/chromium-provenance/releases/tag/content-shell-v0.1.0).
