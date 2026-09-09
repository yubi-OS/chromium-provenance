// Turns adapter results into the Evidence[] shape components/provenance_gate consumes.
// Pure. Mirrors policy.h Signal names exactly.
export const SIGNAL = Object.freeze({
  NONE: "none",
  C2PA_VALID_NO_AI: "c2pa_valid_no_ai_action",
  C2PA_VALID_AI: "c2pa_valid_ai_action",
  C2PA_INVALID: "c2pa_invalid",
  WATERMARK: "watermark_detected",
  CLASSIFIER: "classifier_positive",
  UNAVAILABLE: "verifier_unavailable",
});

export function evidence(signal, { confidence = 0, source, text_chars = 0 } = {}) {
  if (!Object.values(SIGNAL).includes(signal)) throw new Error(`bad signal ${signal}`);
  if (confidence < 0 || confidence > 1) throw new Error("confidence out of range");
  return { signal, confidence, source, text_chars };
}

// Run adapters with a per-adapter timeout; a failure or timeout becomes
// UNAVAILABLE for that source rather than poisoning the whole response.
export async function runAdapters(adapters, input, { timeoutMs = 4000 } = {}) {
  const out = await Promise.all(adapters.map(async (a) => {
    try {
      return await Promise.race([
        a.run(input),
        new Promise((_, rej) => setTimeout(() => rej(new Error("timeout")), timeoutMs)),
      ]);
    } catch {
      return evidence(SIGNAL.UNAVAILABLE, { source: a.name });
    }
  }));
  return out.flat();
}
