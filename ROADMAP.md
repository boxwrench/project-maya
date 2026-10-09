# Roadmap

The priority hardware is what most people have: **one R9700 / RX 9070**, and **Strix Halo**. Two-GPU setups and the
RX 7900 XT come second.

## Now

- **Crash fix ([#39](https://github.com/mw00/project-maya/pull/39)) and RDNA4 prompt attention
  ([#38](https://github.com/mw00/project-maya/pull/38)), rebased and pushed.** Both sit on current main with
  [#41](https://github.com/mw00/project-maya/pull/41) (INT8 latent cache) handled, and await the maintainer's CUDA
  check. Reporters of [#50](https://github.com/mw00/project-maya/issues/50) (NVIDIA) and
  [#53](https://github.com/mw00/project-maya/issues/53) (Windows RX 7900 XTX) have been pointed at #39; a CUDA
  confirmation from #50 would help the merge.
- **Skip the dead final-layer prompt work ([#61](https://github.com/mw00/project-maya/pull/61)).** Open. On one GPU
  with no MTP block loaded, the last layer's FFN and attention output projection for every prompt token except the
  last feed nothing. Measured +1-1.4% prompt speed, byte-identical output.
- **Everyday context configs.** R9700: 128K INT8 latents, decode ~25 tok/s, prompt ~782 tok/s at
  8K / ~743 at 119K. Strix Halo: 262144 FP16, ~343/337/327/314 prompt tok/s at 8K/32K/64K/120K;
  reserving the larger window costs at most 1.6% at matched lengths. Expert capacity is preserved, but prompt
  lending still evicts residents and decode fetches from disk. The two-row worker restores this config at completion.
- **Quality baseline and local showcase route validated.** Fresh R9700 128K run: math 37/40, MMLU-Pro 48/56,
  HumanEval 30/30 (grader-context correction audited), IFEval 29/40 strict / 35/40 loose, tools 11/12.
  All 183 cases completed without request errors or truncation; five writing outputs have constraint misses.
  See [QUALITY.md](QUALITY.md). No matched NVIDIA comparison or long-context quality attribution.
  Local-Maya dsh smoke passed eight independently rerun tests; large builds remain untested.
  Next GLM-5.3-Flash alone, driven through
  the local Maya server, rebuilds public demo projects (first `landscape-forge`, then
  `Water-Treatment-Plant-Simulator`).
- **Fused WMMA prompt MoE for gfx1151: PR preparation.** The tested v1.0.16 prototype gives +13/+7.5/+4.8%
  Strix prefill at 4/8/16K. It applies cleanly to v1.0.26; preparation restricts runtime dispatch to gfx1151 and
  preserves MMQ scratch on other devices or with the switch disabled. Current-main engine/parity targets compile
  for gfx1151/gfx1100/gfx1201; Python checks pass. Fresh Strix runtime validation is pending.

## Next

- **R9700 follow-up decision: keep ROCm 7.2.1.** The isolated `10.2.0a20261009`
  runtime/compiler plus fresh gfx1201 table passes four correctness screens but
  loses 26–32% prefill versus a stable repeated baseline. Decode 28 versus 26 tok/s
  uses different replies, so it is not an isolated kernel-speed gain. Original
  131072 server restored; [E46](EXPERIMENTS.md#e46---r9700-isolated-therock-runtime-comparison-dropped)
  records the negative switch decision and incomplete remainder tuning.
  Independent 13312-token prompt chunks / 2048-token compute sub-batches and
  expert-wise reuse already exist ([E45](EXPERIMENTS.md#e45---r9700-transfer-window-reuse-audit-dropped)).
  A future lead is profiling the nightly's prompt regression or expert-tier
  admission/exchange; neither has been launched. No repeat sub-batch sweep.
- **Two-token decode step for single-GPU speculation.** Running two rows through the prompt path costs 8.2x a
  decode step, so speculation needs purpose-built two-row decode kernels (dense GEMV sharing weight reads, two-row
  experts and attention) at <= ~1.4x a step. Staged work with gates; stop at the first failed gate.
- **NIAH digit precision.** Long-context recall sometimes garbles exact digits (4817->4481) while multi-hop sums
  score 3/3 on the R9700. Quant precision is one hypothesis; the Strix FP16 sample used different needles and
  reported no misses, so the cause is not established. The short-task eval is a sanity baseline, not a controlled
  test of these long-context failures.
- **Close [#15](https://github.com/mw00/project-maya/pull/15)** (RAM shadows, on hold) once #39 lands.

## Later, bigger bets (only if the numbers justify them)

- **Exchange swap for expert tiers.** Another AMD GLM engine measured decode going from 10.9 to 17.9 tok/s, with less
  host RAM. This matters most on R9700 machines with less RAM.
- **Windowed sparse attention for prompts beyond 32K.** Another engine reports 3x at 128K.
- **Two-card prompt pipeline.** Dual GPU only, and not a priority.

## Looked at, not worth it (for now)

| Idea | Why not |
|---|---|
| Single-GPU MTP via the prompt path | Parked: a 2-row verify through the existing batched path costs 8.2x a decode step (dense FP16 GEMMs dominate) and fails parity. Reopened as the two-row decode-kernel work above. Draft acceptance itself is good (75% greedy on Strix). |
| RDNA4 decode expert kernels | Tuned and bit-identical (+18-29% in microbenchmarks) but only +1% end-to-end decode: experts are a small slice of a step. Parked. |
| R9700 prefill sub-batch 1024 vs 2048 vs 4096 | No gain within noise (single samples); 4096 reads lower at 8K/28K. Keep 1024-2048. |
| R9700 switch to TheRock `10.2.0a20261009` + fresh table | Correctness screens pass, but prefill loses 26–32%; observed decode improvement uses different replies. Keep ROCm 7.2.1, not a blanket judgment of all nightlies. |
| Fused int8 WMMA prompt MoE (from [Strata](https://github.com/Niko1221/Strata)) | Ported and correct, but no end-to-end gain on the R9700 or RX 7900 XT so far. (On Strix Halo the fused path does win; see Now.) |
| llama.cpp's RDNA4 MMQ patch ([#25940](https://github.com/ggml-org/llama.cpp/pull/25940)) | No repeatable gain for Maya's IQ formats. |
| Shared expert on a second stream (Strix) | Correct, identical output, but within noise (17.2 vs 17.9 tok/s). |
| HIP graphs for decode | Tiny kernels are only ~5% of a Strix token; the time is in reading weights. |
| Stopping stale speculation early (two GPUs) | The second card sets the pace; the first card's wasted work runs in its idle time. |
| Rebalancing the two-GPU split | Moving layers just moves the VRAM shortage; the default split is best. |
| Tuning the CPU expert lane | It is worth +11 tok/s on two GPUs, and its automatic split is already the best of those we tried. |

## Ceilings to keep in mind

- **Strix Halo decode:** Maya-S is at 17.2-17.8 tok/s in the v1.0.16 sweep. The older 24-26 tok/s figure is a rough
  bandwidth ceiling, not a measured current result. The quant reads ~9 GB of weights per token, and the chip reads
  ~240 GB/s; getting past the ceiling needs a smaller quant or speculation.
- **Any single card:** decode is set by how many experts fit in VRAM. More VRAM beats faster kernels.
