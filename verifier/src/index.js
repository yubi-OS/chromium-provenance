import { runAdapters } from "./aggregate.js";
import { anthropicWatermark, openaiProvenance, binoculars } from "./adapters.js";

function enabledAdapters(env, kind) {
  const a = [];
  if (kind === "text") {
    if (env.ENABLE_ANTHROPIC === "true") a.push(anthropicWatermark(env));
    if (env.BINOCULARS_URL) a.push(binoculars(env));
  } else {
    if (env.ENABLE_OPENAI === "true") a.push(openaiProvenance(env));
  }
  return a;
}

export default {
  async fetch(req, env) {
    const url = new URL(req.url);
    if (req.method !== "POST") return new Response("POST /v1/text | /v1/media", { status: 405 });
    let body; try { body = await req.json(); } catch { return Response.json({ error: "bad json" }, { status: 400 }); }
    if (url.pathname === "/v1/text") {
      if (typeof body.text !== "string" || body.text.length < 800) return Response.json({ error: "text < 800 chars is never evaluated (CONSTRAINTS #5)" }, { status: 400 });
      return Response.json({ evidence: await runAdapters(enabledAdapters(env, "text"), { text: body.text }) });
    }
    if (url.pathname === "/v1/media") {
      if (typeof body.url !== "string") return Response.json({ error: "url required" }, { status: 400 });
      return Response.json({ evidence: await runAdapters(enabledAdapters(env, "media"), { mediaUrl: body.url }) });
    }
    return new Response("not found", { status: 404 });
  },
};
