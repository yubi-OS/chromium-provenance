import { SIGNAL, evidence } from "./aggregate.js";

// Each adapter: { name, run(input) -> Evidence | Evidence[] }.
// Disabled adapters are simply not instantiated (see index.js), so the
// browser never sees a phantom "unavailable" for a feature we never turned on.

// Anthropic Claude text watermark (Fable 5.1 / Mythos 5.1). Private preview
// API as of 2026-09-09; endpoint + schema to be filled from the preview docs
// once access is granted. Until then this adapter must stay disabled.
export function anthropicWatermark(env) {
  return {
    name: "anthropic",
    async run({ text }) {
      if (!env.ANTHROPIC_WATERMARK_ENDPOINT) throw new Error("not configured");
      const r = await fetch(env.ANTHROPIC_WATERMARK_ENDPOINT, {
        method: "POST",
        headers: { "content-type": "application/json", "x-api-key": env.ANTHROPIC_API_KEY },
        body: JSON.stringify({ text }),
      });
      if (!r.ok) throw new Error(`anthropic ${r.status}`);
      const j = await r.json();
      // Expected: probabilistic score; map to our Evidence. Field names TBD.
      const p = Number(j.score ?? j.probability ?? 0);
      return evidence(p > 0 ? SIGNAL.WATERMARK : SIGNAL.NONE, { confidence: p, source: "anthropic", text_chars: text.length });
    },
  };
}

// OpenAI content provenance (images/audio): POST /v1/content_provenance_checks
export function openaiProvenance(env) {
  return {
    name: "openai",
    async run({ mediaUrl }) {
      if (!mediaUrl) return evidence(SIGNAL.NONE, { source: "openai" });
      const r = await fetch("https://api.openai.com/v1/content_provenance_checks", {
        method: "POST",
        headers: { "content-type": "application/json", authorization: `Bearer ${env.OPENAI_API_KEY}` },
        body: JSON.stringify({ url: mediaUrl }),
      });
      if (!r.ok) throw new Error(`openai ${r.status}`);
      const j = await r.json();
      return evidence(j.detected ? SIGNAL.WATERMARK : SIGNAL.NONE, { confidence: j.detected ? 1 : 0, source: "openai" });
    },
  };
}

// Binoculars (open-source zero-shot text detector) behind our own GPU service.
export function binoculars(env) {
  return {
    name: "binoculars",
    async run({ text }) {
      if (!text || text.length < 800) return evidence(SIGNAL.NONE, { source: "binoculars", text_chars: text?.length ?? 0 });
      const r = await fetch(env.BINOCULARS_URL, { method: "POST", headers: { "content-type": "application/json" }, body: JSON.stringify({ text }) });
      if (!r.ok) throw new Error(`binoculars ${r.status}`);
      const { p_ai } = await r.json();
      return evidence(SIGNAL.CLASSIFIER, { confidence: p_ai, source: "binoculars", text_chars: text.length });
    },
  };
}
