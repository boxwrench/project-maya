# Maya-S, Maya-M and Maya-L quantization reference

Verified against the author's model card and recipe reports on 2026-10-09.
These are three mixed-precision GGUF quantizations of the same GLM-5.3-Flash
321B MoE model, made directly from Z.ai's original FP8 release. They are not
successive requantizations of Maya-S and are not uniformly 2-, 3- or 4-bit.
The letter identifies a quantization recipe, not a different model architecture.

## What each contains

Here, **S means Maya-S v2**, the `Maya-S-v2-IQ2_XXS` release used in our AMD
tests. The sizes below are the author's published model-file sizes, not VRAM
requirements or the size of our converted runtime pack.

| Component | Maya-S v2 | Maya-M | Maya-L |
|---|---|---|---|
| Published model size | 96.5 GB | 116.0 GB | 156.3 GB |
| Routed experts: gate/up | IQ2_XXS | IQ2_S | IQ3_S |
| Routed experts: down, ordinary MoE layers | IQ2_S | IQ3_XXS | IQ4_XS |
| Routed experts: down, first and last four MoE layers | IQ3_XXS | IQ3_S | Q5_K |
| Attention projections and shared experts | Q6_K | Q6_K | Q6_K |
| Three dense layers, embeddings, output, MLA k_b/v_b | Q6_K | Q6_K | Q6_K |
| Small KDA projections and DSA indexer | Q8_0 | Q8_0 | Q8_0 |
| Router, norms and stream-mixing weights | F32 | F32 | F32 |
| NextN/MTP draft experts: gate/up | Q2_K | Q3_K | Q4_K |
| NextN/MTP draft experts: down | Q3_K | Q4_K | Q5_K |

In plain language: **S has roughly 2-bit routed experts; M raises them to
2–3 bits; L raises them to 3–4 bits, with 5-bit down weights in sensitive
layers.** All three retain higher precision in the shared, always-active
parts. Format names describe block-quantization schemes: their nominal bit
labels are not exact whole-model bits per weight.

The author uses per-expert activation statistics collected from the FP8
model and GPTQ-style error-feedback rounding for the routed gate/up weights.
Calibration includes chat, multilingual text, reasoning, code and tool calls;
M and L put more weight on tool calls and front-end code. L uses M's
calibration/statistics at higher precision; its source weights are still FP8,
not the already-quantized M file.

**Maya-S24 is separate:** it keeps S's routed experts but changes attention
and shared experts to Q4_K, yielding a 94.7 GB file. It is not the S v2
recipe used for the measurements on this branch.

## What this means for our machines

Our AMD speed and quality results on this branch are for **Maya-S v2**.
We have not measured Maya-M or Maya-L on the R9700 or Strix Halo. More bits
reduce quantization error but enlarge experts, leaving fewer in a fixed-size
memory tier and increasing the bytes needed when fetching nonresident experts.
Exact speed also depends on quantization kernels, tier allocation, actual
prompt length and speculative decoding; model-file size alone is not a
throughput predictor.

The earlier conversational estimates of about 21 tok/s for M and 15.5 tok/s
for L on the R9700 were **unvalidated inverse-file-size calculations** from
S's 25.1 tok/s measurement, not benchmarks or performance commitments.
Neither should be used as a measured result. Any M/L run must retune the
RAM tier rather than blindly reuse S's 90 GB allocation. A 156.3 GB L model
cannot be fully resident in Strix Halo's 128 GB total unified memory; OS,
runtime buffers and context state need memory too.

Higher precision is a candidate for improving exact-digit recall and
constraint adherence, not an established fix. Our current samples do not
attribute those failures to quantization. See [QUALITY.md](QUALITY.md) for
what was actually tested and its limitations.

## Primary sources

- [Author's model card and complete tensor table](https://huggingface.co/peasantsmith/GLM-5.3-Flash-Maya-GGUF)
- [Maya-S v2 recipe/report](https://github.com/mw00/project-maya/blob/main/bench/results/MAYA-S.md): `tools/maya_quant/recipes/maya-s-v2.json`
- [Maya-M recipe/report](https://github.com/mw00/project-maya/blob/main/bench/results/MAYA-M.md): `tools/maya_quant/recipes/maya-m.json`
- [Maya-L recipe/report](https://github.com/mw00/project-maya/blob/main/bench/results/MAYA-L.md): `tools/maya_quant/recipes/maya-l.json`

These links track the author's current documents; the verification date above
identifies this documentation snapshot. No new model download or GPU run was
performed for this reference.
