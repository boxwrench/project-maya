# Roadmap

The priority hardware is what most people have: **one R9700 / RX 9070**, and **Strix Halo**. Two-GPU setups and the
RX 7900 XT come second.

## Now

- **Crash fix ([#39](https://github.com/mw00/project-maya/pull/39)) and RDNA4 prompt attention
  ([#38](https://github.com/mw00/project-maya/pull/38)), rebased onto v1.0.21.** The rebase sits on top of
  [#41](https://github.com/mw00/project-maya/pull/41) (INT8 latent cache), [#44](https://github.com/mw00/project-maya/pull/44)
  (auto split and chunks), and [#31](https://github.com/mw00/project-maya/pull/31) (speculation groups). Verify it, push
  both, and leave CUDA testing and the merge to the maintainer. Also collect fresh two-GPU, single-R9700, and RX 7900 XT
  numbers from that build.
- **Fused WMMA prompt MoE on gfx11: parity fix.** The kernel fails parity, likely because the SwiGLU clamp is missing
  and/or the WMMA operand order is wrong. Fix it, then re-measure on Strix Halo. It gave +5-8% prompt speed before the fix.
- **[#52](https://github.com/mw00/project-maya/pull/52): v1.0.21 does not build on AMD** without three HIP mappings. Open.

## Next

- **R9700 decode expert kernels for RDNA4.** The IQ2_S down kernel went from 64.1 to 50.1 us (+28%); IQ3_XXS down
  shows no gain. Finish the IQ2_S work, prove bit-identical parity, and run a full-model decode A/B.
- **Skip the dead final-layer prompt work (MG-L005).** On one GPU with no MTP block loaded, the last layer's FFN and
  attention output for every prompt token except the last feed nothing. Skipping it is expected to give +1-2% prompt
  speed, with identical output.
- **Single-GPU MTP speculation for Strix Halo (reopened).** Measure first: single-card draft acceptance and draft cost.
  Two-GPU acceptance was about 0.76. A two-token verify is estimated at about 1.2x a one-token step, because dense
  weights are read once. Then decide whether to build it. Potential gain: up to about 1.4x decode.
- **R9700 prompt sub-batch sweep** at 1024, 2048, and 4096.
- **Point upstream reporters at the fix.** After [#39](https://github.com/mw00/project-maya/pull/39) is pushed, tell the
  reporters of [#50](https://github.com/mw00/project-maya/issues/50) (NVIDIA) and
  [#53](https://github.com/mw00/project-maya/issues/53) (Windows RX 7900 XTX). Both show the same crash signature.

## Later, bigger bets (only if the numbers justify them)

- **Exchange swap for expert tiers.** Another AMD GLM engine measured decode going from 10.9 to 17.9 tok/s, with less
  host RAM. This matters most on R9700 machines with less RAM.
- **Windowed sparse attention for prompts beyond 32K.** Another engine reports 3x at 128K.
- **Two-card prompt pipeline.** Dual GPU only, and not a priority.

## Looked at, not worth it (for now)

| Idea | Why not |
|---|---|
| Single-GPU MTP speculation | **Reopened and being measured:** first measure expert overlap and single-card draft cost. Consecutive tokens share only ~30% of their experts (#26's data), so verifying two tokens may cost almost twice the expert reads. The earlier 1.1-1.2x estimate needs a fresh Strix measurement. |
| Fused int8 WMMA prompt MoE (from [Strata](https://github.com/Niko1221/Strata)) | Ported and correct, but no end-to-end gain on the R9700 or RX 7900 XT so far. |
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
