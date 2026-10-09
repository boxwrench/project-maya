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
- **128K everyday configs.** Done on the R9700 (single card, INT8 latents, decode ~25 tok/s, prompt ~782 tok/s at
  8K / ~743 at 119K); in progress on Strix Halo (128K arm measuring now, then the everyday server switches over).
- **Quality eval, then showcase builds.** Next: confirm tool calling (11/12) and run the full brief quality eval on
  the R9700 (~2.5-3 h, unattended) — the first real AMD quality numbers. Then GLM-5.3-Flash alone, driven through
  the local Maya server, rebuilds public demo projects (first `landscape-forge`, then
  `Water-Treatment-Plant-Simulator`).
- **Fused WMMA prompt MoE for gfx1151: needs a PR.** The SwiGLU-clamp fix passes parity and gives +13/+7.5/+4.8%
  Strix prefill at 4/8/16K, but is committed on top of v1.0.16 and must be rebased onto current main (gfx1151
  only; enabling gfx1100 needs a separate validation).

## Next

- **Two-token decode step for single-GPU speculation.** Running two rows through the prompt path costs 8.2x a
  decode step, so speculation needs purpose-built two-row decode kernels (dense GEMV sharing weight reads, two-row
  experts and attention) at <= ~1.4x a step. Staged work with gates; stop at the first failed gate.
- **NIAH digit precision.** Long-context recall sometimes garbles exact digits (4817->4481) while multi-hop sums
  score 3/3 — suspected ~2-bit quant precision. The quality eval should show whether it matters.
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
