# R9700 quality sanity check — 2026-10-09

183 short-task requests completed on one Radeon AI PRO R9700 with no request
errors, truncated replies, or IFEval checker errors. This is a small sampled
baseline, not a full benchmark or a matched AMD-versus-NVIDIA comparison.

## Setup and method

GLM-5.3-Flash Maya-S v2 (mixed quant; IQ2_XXS gate/up experts, see
[QUANTS.md](QUANTS.md) for the full recipe), Maya v1.0.23 integration with
#38/#39, ROCm 7.2.1, physical GPU 1, 131072 context, INT8 latents. RAM tier
90 GB, reserve 1024 MB, PROMOTE_MIN 6. The frontend and engine run from the
same integration tree. Before/after receipts confirm unchanged config,
engine and tuning-table hashes, boot, and engine PID; no server restart.

Random seed 1234: GSM8K test 40, MMLU-Pro test four per category (56),
HumanEval test 30, IFEval train 40. Twelve local tool cases and five local
writing prompts complete the sample. Math/MMLU/writing use temperature 1.0,
top_p 0.95, reasoning effort low; coding/IFEval/tools use temperature 0,
reasoning off. Every request was token-counted and context-preflighted.
The run took about 29 minutes including the fresh tool pass.

| Suite | Result | Notes |
|---|---:|---|
| GSM8K | 37/40 (92.5%) | Numeric answer grading |
| MMLU-Pro | 48/56 (85.7%) | Four cases per category; not population-weighted |
| HumanEval | 30/30 (100%) | Official tests, corrected prompt-context assembly |
| IFEval | 29/40 strict (72.5%), 35/40 loose (87.5%) | Official lm-eval checker; instruction-level strict 82.5% |
| Tool calls | 11/12 (91.7%) | One extra web search on a weather request; no parser failure |
| Writing | Five outputs reviewed; no aggregate score | Coherent, with constraint misses below |

## Grading correction

The initial HumanEval score was 29/30 because the runner omitted supplied
helpers/imports when a reply contained a complete function. HumanEval/38's
test calls `encode_cyclic`, which the question supplies; omitting it caused
a grader-side `NameError`. Restoring that prompt prefix and rescoring all
30 saved answers against the official tests yields 30/30. No model output
or request was changed or repeated. The original score/error is retained
in the local grading history.

## Observations and limits

The five writing outputs were readable. The LSM summary had five bullets
and the customer email preserved its requested details. The garbage-collection
rewrite had 123 whitespace-delimited words despite an under-120 constraint,
and omitted copying/mark-sweep details. The lighthouse story had about 190
words, used “tide” twice and avoided words longer than ten letters, but added
a separate title and did not end with a question. The code explanation
correctly described most-recent duplicate pairs and average O(n) behavior;
an extra set-plus-map suggestion was redundant.

Small samples have substantial sampling uncertainty; do not extrapolate
these percentages to the full datasets or compare them with published
full-suite scores. No NVIDIA control, perplexity, KL measurement, FP16
versus INT8 ablation, or controlled long-context digit-recall experiment
was run here. E35's AMD-versus-NVIDIA question and E44's digit-corruption
cause remain open. Quality-suite token counts are variable and are not
decode-throughput measurements.

Config SHA256: `7398f2b30abeb147af640c1b92089ac3ef2d980844a1112ec59fbf1d63e4e716`.
Engine SHA256: `edb237f58dac933c313a7a0083287fbb462c8ccedd42fb1dc5f69a161fe1d906`.
Raw requests, responses, usage, grading history, summaries and receipts
are retained locally in `eval/results-r9700-128k-20261009/`.

## Quality gate for speed changes (2026-10-11)

Approximate speed levers (expert skipping, trunk keep, a bigger quant) used to be gated on `KL <= 0.02` versus the
exact model plus at most 1.5 pp on a 178-item eval. That gate was retired. The 0.02 number was borrowed from an
upstream README rather than derived, and the 178-item suite cannot resolve a 1-2 pp loss: at the measured 8-16/178
discordance its 80%-power MDE is 4.4-6.3 pp, and the power to detect 2 pp is 14-24% (6.5-9.1% for 1 pp). The
replacement has three parts.

**Exact changes (Class 0).** Anything that should not change the maths — batched MLA `k_b/v_b`, a fused union MoE
read, a quantize ordering — must pass the existing parity tests (`glm_layer_parity`, `glm_model_test`,
`hip_prefill_fused_parity`, `hip_prefill_mmq_parity`), be greedy token-identical on a fixed prompt set, and show KL
exactly 0 versus the reference. Any nonzero KL is a bug, not a trade.

**Approximate changes (Class 1), the adopted bar.** Three gates. Gate A is a token-level divergence screen over
8-16 fixed held-out texts x 1024 scored tokens: mean KL at most 0.05 to adopt and above 0.10 a hard fail; top-1
agreement at least 90% and below 85% a hard fail; p99 per-token KL at most 25x the mean. Gate B is exact-behaviour
canaries, and any hard failure rejects: no new tool-call structural mismatch (the tools suite to grow from 12 to
about 48 cases), no IFEval strict-pass becoming a fail, NIAH exact non-inferior at 32K/64K/128K, and coding pass@1
non-inferior with the first divergent token reported. The 178-item suite is kept but demoted to a gross-breakage
guard (pre-register a 3 pp non-inferiority margin and report exact McNemar b/c with a 95% CI). Gate C is the speed
gate (+8% decode or +3% e2e). Passing all three makes an everyday candidate; a failed Gate A with passing B/C and a
large gain ships only as an opt-in fast mode; any Gate B hard failure rejects regardless of KL.

**The merged v2 (user approved 2026-10-11).** The later revision keeps Class 0 unchanged and turns KL and top-1
agreement into a diagnostic plus a gross-damage tripwire only: reject above mean KL 0.10 or below 85% top-1, and
always report the direction, reference, corpus and truncation. Acceptance then rests on a paired task suite: 64
development cases plus 160 locked confirmation cases (40 tools, 40 coding and instruction, 40 long-context
RULER-style at 8K/32K/64K/120K actual tokens, 40 multi-turn workflows), with reference-right to candidate-wrong and
wrong-to-right counted separately, exact McNemar for binary outcomes and prompt-level bootstrap CIs. An arm passes
only with a repeatable speed gain, zero reproducible critical right-to-wrong flips, a one-sided 95% CI of overall
paired quality of at least -2 pp, and no domain worse than -3 pp; inconclusive means more cases, not a pass. The
tokenizer, template, greedy sampling, tool schema and grader revision are frozen. Until the suite exists, the
178-item eval plus the 48-case tools set are breakage detectors only.

KL values are not comparable across models, corpora, tokenizers or implementations, so they are only read against our
own reference. The full sourcing is in `results/ds-quality-bar.md` (E98) and GAP-REGISTER.md.
