# R9700 quality sanity check — 2026-10-09

183 short-task requests completed on one Radeon AI PRO R9700 with no request
errors, truncated replies, or IFEval checker errors. This is a small sampled
baseline, not a full benchmark or a matched AMD-versus-NVIDIA comparison.

## Setup and method

GLM-5.3-Flash Maya-S (IQ2_XXS-based experts), Maya v1.0.23 integration with
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
