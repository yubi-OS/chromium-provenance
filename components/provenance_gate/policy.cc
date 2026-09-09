// Copyright 2026 The yubi-OS Authors. BSD-3-Clause (matches Chromium).
#include "components/provenance_gate/policy.h"

namespace provenance_gate {

namespace {

Decision Block(const Evidence& e, const char* reason, bool overridable) {
  Decision d;
  d.allow = false;
  d.overridable = overridable;
  d.signal = e.signal;
  d.confidence = e.confidence;
  d.source = e.source;
  d.reason = reason;
  return d;
}

}  // namespace

const char* SignalName(Signal s) {
  switch (s) {
    case Signal::kNone: return "none";
    case Signal::kC2paValidNoAiAction: return "c2pa_valid_no_ai_action";
    case Signal::kC2paValidAiAction: return "c2pa_valid_ai_action";
    case Signal::kC2paInvalid: return "c2pa_invalid";
    case Signal::kWatermarkDetected: return "watermark_detected";
    case Signal::kClassifierPositive: return "classifier_positive";
    case Signal::kVerifierUnavailable: return "verifier_unavailable";
  }
  return "unknown";
}

Decision Evaluate(const Policy& policy, const std::vector<Evidence>& evidence) {
  const bool strict = policy.mode == Mode::kProvenanceRequired;
  bool saw_verifier_down = false;
  bool saw_valid_provenance = false;
  const Evidence* classifier_hit = nullptr;

  // Pass 1: hard signals. Order matters: strongest evidence first.
  for (const Evidence& e : evidence) {
    switch (e.signal) {
      case Signal::kC2paValidAiAction:
        return Block(e, "c2pa_manifest_records_ai_generation", !strict);
      case Signal::kWatermarkDetected:
        if (e.confidence >= policy.watermark_threshold)
          return Block(e, "watermark_detected", !strict);
        break;
      case Signal::kC2paInvalid:
        if (strict) return Block(e, "c2pa_manifest_invalid", false);
        break;
      case Signal::kClassifierPositive:
        if (e.confidence >= policy.classifier_threshold &&
            e.text_chars >= policy.classifier_min_chars &&
            (!classifier_hit || e.confidence > classifier_hit->confidence))
          classifier_hit = &e;
        break;
      case Signal::kVerifierUnavailable:
        saw_verifier_down = true;
        break;
      case Signal::kC2paValidNoAiAction:
        saw_valid_provenance = true;
        break;
      case Signal::kNone:
        break;
    }
  }

  // Pass 2: soft signals. Classifier blocks are always overridable
  // (CONSTRAINTS.md #2) because zero-shot detectors have real FPR.
  if (classifier_hit)
    return Block(*classifier_hit, "classifier_above_threshold", true);

  Decision d;
  if (strict) {
    if (saw_verifier_down) {
      d.allow = false; d.overridable = false;
      d.signal = Signal::kVerifierUnavailable;
      d.reason = "verifier_unavailable_fail_closed";
      return d;
    }
    if (!saw_valid_provenance) {
      d.allow = false; d.overridable = false;
      d.signal = Signal::kNone;
      d.reason = "no_trusted_provenance";
      return d;
    }
    d.reason = "provenance_valid";
    return d;
  }

  d.allow = true;
  d.reason = saw_verifier_down ? "verifier_unavailable_fail_open" : "no_signal";
  if (saw_verifier_down) d.signal = Signal::kVerifierUnavailable;
  return d;
}

}  // namespace provenance_gate
