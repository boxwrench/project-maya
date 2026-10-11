# Roadmap

The priority hardware is what most people have: **one R9700 / RX 9070**, and **Strix Halo**. Two-GPU setups and the
RX 7900 XT come second.

## Now

- **Strix prefill is the priority.** The measured f16 ceiling is 24.2 TFLOPS (20.9 at the prefill clock), so the
  engine is at ~50% of peak, not 21%: the ranked excess is routed MoE (32%), MLA/DSA attention, hyper-connections
  and KDA projections. The next experiments are in Next. See [E94](EXPERIMENTS.md#e94---strix-gfx1151-prefill-push-measured-f16-ceiling-per-phase-excess-and-three-levers-closed-no-gain-the-assumed-59-tflops-is-24x-too-high-no-lever-clears-3).
- **Upstream [#96](https://github.com/mw00/project-maya/pull/96) single-GPU MTP is adopted on Strix, with our
  verify-fuse work on top.** Adaptive MTP is +17.7% over no-spec ([E89](EXPERIMENTS.md#e89---strix-gfx1151-pr-96-single-gpu-mtp-speculation-config-knob-sweep-closed-no-gain-keep-the-everyday-default-draft3--adapt-mtp-is-177-over-no_spec-1919---2257-toks-and-adaptive-beats-fixed-23-by-6-9-points-but-no-arm-clears-the-pre-registered-bar));
  batching the absorbed MLA `k_b/v_b` and fusing the routed MoE union read cuts the T=2 verify round 78.1 -> 73.9 ms,
  bit-identical, and is now the everyday Strix build ([E91](EXPERIMENTS.md#e91---strix-gfx1151-pr-96-verify-round-batched-mla-k_bv_b-a--fused-union-routed-moe-b-kept-ab-cuts-the-t2-verify-round-7809---7394-ms-c2t-143---135-bit-identical-with-no-no_spec-regression),
  [E92](EXPERIMENTS.md#e92---nimo-everyday-verify-fuse-build-adopted-adopted-maya-nimo-nextjson-now-runs-the-verify-fuse-exe-6136fe7d-default-decode-43-33k-54)).
  On R9700 the upstream gate still keeps MTP off at ~28% experts in VRAM.
- **Open upstream: [#98](https://github.com/mw00/project-maya/pull/98) and
  [#113](https://github.com/mw00/project-maya/pull/113).** Fused prompt MoE on gfx1151 (#98:
  +13.5/+8.1/+5.0% at 4/8/16K) is waiting on the maintainer's Windows build, and the gfx1151 wmma2 default (#113) is
  open. The current everyday configs are R9700 `RESERVE_MB 512`, the nimo verify-fuse build, and
  `maya-2gpu-best-7900.json` on the dual pair.
- **Recently landed (v1.0.32).** [#38](https://github.com/mw00/project-maya/pull/38) (RDNA4 `wmma2` prompt attention),
  [#39](https://github.com/mw00/project-maya/pull/39) (tier/lending race crash fix),
  [#52](https://github.com/mw00/project-maya/pull/52) (HIP device-query mappings) and
  [#61](https://github.com/mw00/project-maya/pull/61) (skip the last layer's unused prompt outputs, +1–1.4%,
  byte-identical) are all merged. RAM shadows ([#15](https://github.com/mw00/project-maya/pull/15)) stay on hold.

## Next

- **Power-mode A/B on Strix.** The stock "balanced" mode runs 2.47 GHz at 85 W; the measured prefill ceiling scales
  with the clock, so test the power/clock modes before more kernel work (E94).
- **Small-wins bundle.** The sub-gate levers are individually below the e2e gate but stack: `PREFILL_SUB=8192` +
  `PREFILL_MB=24576` (+1.7% at 32K), the retuned hipBLASLt table (+0.8% at 32K) and the MMQ chunk-fastest quant
  order (+0.6–0.8% on the fallback path). Combine them and re-measure the bundle (E94, E96, E99).
- **Fused-tile sweep and DFlash2 on Strix.** Retune the fused prompt MoE tiles (E94, E97) and take the next
  draft/verify change after the verify-fuse work (E91, E92).
- **DSA rows.** Work the DSA indexer and projections, the remaining context-dependent prefill growth
  (`glm_dsa_select` is O(n_vis²)); the ROCmFPX map found no transferable GLM DSA patch (E93, E94).
- **Maya-M and trunk keep under the new quality gate.** Re-run the PR #103 trunk-keep decision (E95: +6–16% decode
  on R9700 but KL 0.025–0.062) and Maya-M vs Maya-S against the paired quality suite below, not the old
  "KL <= 0.02" (E98).
- **Paired 224-case quality suite.** 64 development + 160 locked confirmation cases (40 tools, 40 coding+instruction,
  40 long-context, 40 multi-turn) with exact McNemar and bootstrap CIs (E98; GAP-REGISTER "QUALITY BAR v2"). Until it
  exists, the 178-item eval + 48-case tools set are breakage detectors only.
- **Cost-aware expert placement: compute in place vs copy.** Issue [#6](https://github.com/mw00/project-maya/issues/6)
  found that, for RAM-tier experts, computing on the CPU beat copying the weights over PCIe. We want the placement
  planner to choose per expert and per token between compute-in-place and copy, using the measured cost of each.

## Later, bigger bets (only if the numbers justify them)

- **Routing-locality and hardware-aware quantization research (opt-in, quality-gated).** Study whether expert routing
  has enough locality to exploit, and whether quant recipes can be tuned to the memory tiers and kernels we actually
  run. Any change would be opt-in and gated on quality, not a default swap.
- **PR [#86](https://github.com/mw00/project-maya/pull/86) long-context validation on AMD.** Validate the upstream
  long-context path on R9700 and Strix Halo before relying on it.
- **Vision path.** Text is the tested surface today; the model's vision path is untested on AMD.
- **APU + discrete GPU together.** Use Strix Halo's unified memory as a tier alongside a discrete card, instead of
  treating them as separate machines.

## Looked at, not worth it (for now)

| Idea | Why not |
|---|---|
| Single-GPU MTP via the prompt path | Parked: a 2-row verify through the existing batched path costs 8.2x a decode step (dense FP16 GEMMs dominate) and fails parity. Reopened as the two-row decode-kernel work above. Draft acceptance itself is good (75% greedy on Strix). |
| RDNA4 decode expert kernels | Tuned and bit-identical (+18-29% in microbenchmarks) but only +1% end-to-end decode: experts are a small slice of a step. Parked. |
| R9700 prefill sub-batch 1024 vs 2048 vs 4096 | No gain within noise (single samples); 4096 reads lower at 8K/28K. Keep 1024-2048. |
| R9700 switch to TheRock `10.2.0a20261009` + fresh table | Correctness screens pass, but prefill loses 26–32%; observed decode improvement uses different replies. Keep ROCm 7.2.1, not a blanket judgment of all nightlies. |
| GEMM/hipBLASLt tuning on R9700 | The GEMMs already run at ~66% of the chip's FP16 peak and are only ~11% of prefill time; better selection is worth an estimated 1–4%. Parked until prefill compute work lands. |
| BF16 expert paths for Strix (E57, E60) | Two GPU prototypes: the old one was 10.9–18x slower than native INT8; the improved v2 was 4–7.5x faster than the old prototype but still 2.3–3.3x slower than native INT8. Native INT8 wins; not re-run. |
| R9700 one-row-filtered hipBLASLt table (E65/E66) | A narrow single-operation saving (~33%) did not survive as model gain: the same-engine prefill A/B was −0.36% at ~8K and −0.21% at ~32K. Keep the original table. |
| Packed-activation reuse for discrete MMQ (E67 H1) | Device validation failed 48/525 packed cases and the follow-up was stopped; the numerical-domain contract is unresolved. Small CPU defects were fixed, but there is no device result to adopt. |
| Fused int8 WMMA prompt MoE (from [Strata](https://github.com/Niko1221/Strata)) | Ported and correct, but no end-to-end gain on the R9700 or RX 7900 XT so far. (On Strix Halo the fused path does win; see Now.) |
| llama.cpp's RDNA4 MMQ patch ([#25940](https://github.com/ggml-org/llama.cpp/pull/25940)) | No repeatable gain for Maya's IQ formats. |
| Shared expert on a second stream (Strix) | Correct, identical output, but within noise (17.2 vs 17.9 tok/s). |
| HIP graphs for decode | Tiny kernels are only ~5% of a Strix token; the time is in reading weights. |
| Stopping stale speculation early (two GPUs) | The second card sets the pace; the first card's wasted work runs in its idle time. |
| Rebalancing the two-GPU split | Moving layers just moves the VRAM shortage; the default split is best. |
| Tuning the CPU expert lane | It is worth +11 tok/s on two GPUs, and its automatic split is already the best of those we tried. |

## Ceilings to keep in mind

- **Strix Halo decode:** measured ~19 tok/s no-spec and ~23.7 tok/s with #96 MTP plus the verify-fuse branch. The
  quant reads ~9 GB of weights per token and the chip reads ~240 GB/s, giving a rough ~28–30 tok/s bandwidth ceiling.
  Getting past the measured figure needs a smaller quant, better overlap, or speculation.
- **Any single card:** decode is set by how many experts fit in VRAM. More VRAM beats faster kernels.
- **Prefill is compute-bound, not copy-bound.** Removing all SSD re-reads on Strix gave no prefill gain, so prompt
  work should target the math (attention, KDA projections, MoE), not the I/O. The measured Strix f16 ceiling is
  24.2 TFLOPS (20.9 at the prefill clock), so the engine is at ~50% of peak (E94).
