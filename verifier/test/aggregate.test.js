import { test } from "node:test";
import assert from "node:assert/strict";
import { SIGNAL, evidence, runAdapters } from "../src/aggregate.js";

test("evidence rejects unknown signal", () => { assert.throws(() => evidence("human_verified")); });
test("evidence rejects out-of-range confidence", () => { assert.throws(() => evidence(SIGNAL.WATERMARK, { confidence: 1.5 })); });
test("adapter failure becomes verifier_unavailable for that source only", async () => {
  const ok = { name: "ok", run: async () => evidence(SIGNAL.NONE, { source: "ok" }) };
  const bad = { name: "bad", run: async () => { throw new Error("boom"); } };
  const out = await runAdapters([ok, bad], {});
  assert.equal(out.length, 2);
  assert.equal(out[1].signal, SIGNAL.UNAVAILABLE);
  assert.equal(out[1].source, "bad");
});
test("adapter timeout becomes verifier_unavailable", async () => {
  const slow = { name: "slow", run: () => new Promise(() => {}) };
  const out = await runAdapters([slow], {}, { timeoutMs: 20 });
  assert.equal(out[0].signal, SIGNAL.UNAVAILABLE);
});
