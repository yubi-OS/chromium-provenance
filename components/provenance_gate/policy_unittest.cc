// Copyright 2026 The yubi-OS Authors. BSD-3-Clause (matches Chromium).
// Builds under Chromium's gtest AND standalone (-DSTANDALONE_TEST) so CI
// can run it without a Chromium checkout.
#include "components/provenance_gate/policy.h"

#ifdef STANDALONE_TEST
#include <cassert>
#include <cstdio>
#define TEST(a, b) static void a##_##b(); [[maybe_unused]] static int a##_##b##_reg = (a##_##b(), 0); static void a##_##b()
#define EXPECT_TRUE(x) assert(x)
#define EXPECT_FALSE(x) assert(!(x))
#define EXPECT_EQ(a, b) assert((a) == (b))
int main() { std::puts("provenance_gate policy tests: PASS"); return 0; }
#else
#include "testing/gtest/include/gtest/gtest.h"
#endif

namespace provenance_gate {
namespace {

Evidence Ev(Signal s, double c = 1.0, size_t chars = 0, const char* src = "test") {
  Evidence e; e.signal = s; e.confidence = c; e.text_chars = chars; e.source = src; return e;
}

TEST(ProvenanceGatePolicy, ModeA_NoEvidence_Allows) {
  Decision d = Evaluate(Policy{}, {});
  EXPECT_TRUE(d.allow);
  EXPECT_EQ(d.reason, "no_signal");
}

TEST(ProvenanceGatePolicy, ModeA_WatermarkAboveThreshold_Blocks) {
  Decision d = Evaluate(Policy{}, {Ev(Signal::kWatermarkDetected, 0.9, 0, "anthropic")});
  EXPECT_FALSE(d.allow);
  EXPECT_TRUE(d.overridable);
  EXPECT_EQ(d.source, "anthropic");
  EXPECT_EQ(d.reason, "watermark_detected");
}

TEST(ProvenanceGatePolicy, ModeA_WatermarkBelowThreshold_Allows) {
  Decision d = Evaluate(Policy{}, {Ev(Signal::kWatermarkDetected, 0.2)});
  EXPECT_TRUE(d.allow);
}

TEST(ProvenanceGatePolicy, ModeA_C2paAiAction_Blocks) {
  Decision d = Evaluate(Policy{}, {Ev(Signal::kC2paValidAiAction, 1.0, 0, "c2pa")});
  EXPECT_FALSE(d.allow);
  EXPECT_EQ(d.reason, "c2pa_manifest_records_ai_generation");
}

TEST(ProvenanceGatePolicy, ModeA_ClassifierShortText_NeverBlocks) {
  Decision d = Evaluate(Policy{}, {Ev(Signal::kClassifierPositive, 0.999, 200, "binoculars")});
  EXPECT_TRUE(d.allow);
}

TEST(ProvenanceGatePolicy, ModeA_ClassifierLongText_BlocksOverridable) {
  Decision d = Evaluate(Policy{}, {Ev(Signal::kClassifierPositive, 0.99, 5000, "binoculars")});
  EXPECT_FALSE(d.allow);
  EXPECT_TRUE(d.overridable);
  EXPECT_EQ(d.reason, "classifier_above_threshold");
}

TEST(ProvenanceGatePolicy, ModeA_VerifierDown_FailsOpen) {
  Decision d = Evaluate(Policy{}, {Ev(Signal::kVerifierUnavailable, 0)});
  EXPECT_TRUE(d.allow);
  EXPECT_EQ(d.reason, "verifier_unavailable_fail_open");
}

TEST(ProvenanceGatePolicy, ModeB_NoProvenance_Blocks) {
  Policy p; p.mode = Mode::kProvenanceRequired;
  Decision d = Evaluate(p, {});
  EXPECT_FALSE(d.allow);
  EXPECT_FALSE(d.overridable);
  EXPECT_EQ(d.reason, "no_trusted_provenance");
}

TEST(ProvenanceGatePolicy, ModeB_ValidProvenance_Allows) {
  Policy p; p.mode = Mode::kProvenanceRequired;
  Decision d = Evaluate(p, {Ev(Signal::kC2paValidNoAiAction)});
  EXPECT_TRUE(d.allow);
  EXPECT_EQ(d.reason, "provenance_valid");
}

TEST(ProvenanceGatePolicy, ModeB_InvalidManifest_BlocksNonOverridable) {
  Policy p; p.mode = Mode::kProvenanceRequired;
  Decision d = Evaluate(p, {Ev(Signal::kC2paInvalid)});
  EXPECT_FALSE(d.allow);
  EXPECT_FALSE(d.overridable);
}

TEST(ProvenanceGatePolicy, ModeB_VerifierDown_FailsClosed) {
  Policy p; p.mode = Mode::kProvenanceRequired;
  Decision d = Evaluate(p, {Ev(Signal::kC2paValidNoAiAction), Ev(Signal::kVerifierUnavailable)});
  EXPECT_FALSE(d.allow);
  EXPECT_EQ(d.reason, "verifier_unavailable_fail_closed");
}

TEST(ProvenanceGatePolicy, StrongestSignalWins) {
  Decision d = Evaluate(Policy{}, {Ev(Signal::kClassifierPositive, 0.99, 5000),
                                   Ev(Signal::kWatermarkDetected, 0.7, 0, "synthid")});
  EXPECT_EQ(d.reason, "watermark_detected");
  EXPECT_EQ(d.source, "synthid");
}

}  // namespace
}  // namespace provenance_gate
