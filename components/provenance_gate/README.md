# components/provenance_gate

Pure policy engine. Adapters (c2pa-rs, TrustMark, remote verifier client) produce `Evidence`; `Evaluate()` returns a `Decision` the interstitial renders. Wiring into `URLLoaderThrottle` and `security_interstitials` lands in `patches/0002` and `0003`.

Standalone test: `g++ -std=c++20 -DSTANDALONE_TEST -I. components/provenance_gate/policy.cc components/provenance_gate/policy_unittest.cc -o /tmp/t && /tmp/t`
