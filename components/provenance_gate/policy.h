// Copyright 2026 The yubi-OS Authors. BSD-3-Clause (matches Chromium).
#ifndef COMPONENTS_PROVENANCE_GATE_POLICY_H_
#define COMPONENTS_PROVENANCE_GATE_POLICY_H_

#include <cstddef>
#include <string>
#include <vector>

namespace provenance_gate {

enum class Mode { kBlockOnDetect, kProvenanceRequired };

// One piece of evidence about a resource. Produced by detector adapters
// (c2pa-rs in-process, TrustMark in-process, remote verifier); consumed here.
enum class Signal {
  kNone,                  // adapter ran, found nothing
  kC2paValidNoAiAction,   // trusted manifest, no AI-generation action recorded
  kC2paValidAiAction,     // trusted manifest records trainedAlgorithmicMedia etc.
  kC2paInvalid,           // manifest present but signature/trust fails
  kWatermarkDetected,     // Anthropic / SynthID / TrustMark / OpenAI signal
  kClassifierPositive,    // zero-shot text classifier (Binoculars etc.)
  kVerifierUnavailable,   // remote verifier unreachable / timed out
};

struct Evidence {
  Signal signal = Signal::kNone;
  double confidence = 0.0;     // [0,1], adapter-calibrated
  std::string source;          // e.g. "anthropic", "synthid", "c2pa", "binoculars"
  size_t text_chars = 0;       // for text evidence: chars evaluated
};

struct Policy {
  Mode mode = Mode::kBlockOnDetect;
  double watermark_threshold = 0.5;
  double classifier_threshold = 0.98;
  size_t classifier_min_chars = 800;
};

struct Decision {
  bool allow = true;
  bool overridable = true;     // "proceed anyway" offered on interstitial
  Signal signal = Signal::kNone;
  double confidence = 0.0;
  std::string source;
  std::string reason;          // stable, machine-readable
};

// Pure function. No I/O, no globals. See CONSTRAINTS.md #7.
Decision Evaluate(const Policy& policy, const std::vector<Evidence>& evidence);

const char* SignalName(Signal s);

}  // namespace provenance_gate

#endif  // COMPONENTS_PROVENANCE_GATE_POLICY_H_
