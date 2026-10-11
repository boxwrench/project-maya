# Roadmap

The priority hardware is what most people have: **one R9700 / RX 9070**, and **Strix Halo**. Two-GPU setups and the
RX 7900 XT come second.

## Now

- **Decode bandwidth and critical-path measurement.** On R9700 and Strix Halo, decode is close to a memory-bandwidth
  limit, so the useful question is which bytes are on the critical path and which reads overlap. We are measuring the
  expert/tier reads and the per-token critical path before changing kernels. Related: the upstream
  [#95](https://github.com/mw00/project-maya/pull/95) GEMV result (RDNA GEMV on the RX 7900 XT was +1.1% decode,
  bit-exact) shows how small the kernel-level headroom is once bandwidth is the wall.
- **Upstream [#96](https://github.com/mw00/project-maya/pull/96) single-GPU MTP vs our two-row verify on Strix Halo.**
  Upstream's gate keeps MTP off at ~28% experts in VRAM; forced, it gives reasoning +12.6%, chat +5.5%, code +3.3%,
  prose −4.6%. Our unmerged two-row prototype is token-identical and +5–21% decode by prompt type on Strix. Next is
  the same comparison done properly: count grouped expert reads across the verify window and bytes per accepted token,
  not just wall-clock.
- **Recently landed (v1.0.32).** [#38](https://github.com/mw00/project-maya/pull/38) (RDNA4 `wmma2` prompt attention),
  [#39](https://github.com/mw00/project-maya/pull/39) (tier/lending race crash fix),
  [#52](https://github.com/mw00/project-maya/pull/52) (HIP device-query mappings) and
  [#61](https://github.com/mw00/project-maya/pull/61) (skip the last layer's unused prompt outputs, +1–1.4%,
  byte-identical) are all merged. Fused prompt MoE on Strix Halo gfx1151 is open as
  [#98](https://github.com/mw00/project-maya/pull/98): +13.5%/+8.1%/+5.0% prefill at 4/8/16K.
  RAM shadows ([#15](https://github.com/mw00/project-maya/pull/15)) stay on hold.

## Next

- **Expert kernels and DSA attention for prefill.** The R9700 prefill profile puts attention ~21%, KDA projections
  ~19% and MoE ~27% of prompt time, so these are the remaining prefill levers. DSA attention is the sparse-attention
  path for longer prompts.
- **Cost-aware expert placement: compute in place vs copy.** Issue [#6](https://github.com/mw00/project-maya/issues/6)
  found that, for RAM-tier experts, computing on the CPU beat copying the weights over PCIe. We want the placement
  planner to choose per expert and per token between compute-in-place and copy, using the measured cost of each.
- **Maya-M (larger quant) on R9700 and Strix Halo.** Our measured results are all Maya-S v2. Maya-M has ~2–3-bit
  routed experts and may help exact-digit recall (an open question), at the cost of fewer resident experts. It needs
  its own RAM-tier sizing, not a reused 90 GB allocation.

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

- **Strix Halo decode:** measured 17.2–18 tok/s (falling to ~15.5 after ~120K). The quant reads ~9 GB of weights per
  token and the chip reads ~240 GB/s, giving a rough ~28–30 tok/s bandwidth ceiling. Getting past the measured figure
  needs a smaller quant, better overlap, or speculation.
- **Any single card:** decode is set by how many experts fit in VRAM. More VRAM beats faster kernels.
- **Prefill is compute-bound, not copy-bound.** Removing all SSD re-reads on Strix gave no prefill gain, so prompt
  work should target the math (attention, KDA projections, MoE), not the I/O.
