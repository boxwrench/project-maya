# Roadmap

The priority hardware is what most people have: **one R9700 / RX 9070**, and **Strix Halo**. Two-GPU setups and the
RX 7900 XT come second.

## Now

- **Fix the full-RAM-tier crash** (affects RAM_RESIDENT from #25, released in v1.0.15, and RAM shadows #15): on a 7900 XT
  with a 90 GB tier, crashes within 2-4 requests at the prompt-to-decode handoff; an ownership fix for shadows (7646b6b)
  did not resolve it. Under investigation.
- **Expert tiers on a single R9700: measured.** RAM-resident tier + `PROMOTE_MIN=6` (+ #24) gives 23.4 -> 25.55 tok/s
  (+9%) with zero disk reads. Earlier notes on this item: The R9700's 32 GB holds only part of the 86 GB of experts, so decode depends on
  how experts move between VRAM, RAM and SSD. Under test together:
  - our RAM shadows ([#15](https://github.com/mw00/project-maya/pull/15))
  - the community's RAM-resident tier ([#25](https://github.com/mw00/project-maya/pull/25))
  - `PROMOTE_MIN`, which stops one-off promotions ([#26](https://github.com/mw00/project-maya/pull/26))
  - the demotion fix ([#24](https://github.com/mw00/project-maya/pull/24); needs a small HIP build fix, posted there)
- **Prompt attention follow-up.** After [#16](https://github.com/mw00/project-maya/pull/16), a WMMA attention kernel
  combined with #16's FP16 MLA products is slightly faster again on the R9700 (e.g. 744 vs 716 tok/s at 8K).

## Next

- **RDNA4 decode expert kernels.** [#19](https://github.com/mw00/project-maya/pull/19) tuned the expert kernels for
  RDNA3/3.5 and left the R9700 on the old ones. Tuning them for gfx1201 is the obvious next decode step for the R9700.
- **Long prompts on the R9700.** Maya v1.0.12 lifted the 8K prompt-chunk cap. Measure 16-32K prompts on a single R9700,
  where bigger chunks mean fewer passes over the experts that don't fit in VRAM.
- **Recommended single-R9700 settings in the AMD docs** once the crash fix lands.
- **Answer review comments** on the open PRs as they come in.

## Looked at, not worth it (for now)

| Idea | Why not |
|---|---|
| Single-GPU MTP speculation | Large build (85-170 h). Consecutive tokens share only ~30% of their experts (#26's data), so verifying two tokens costs almost twice the expert reads. Expected 1.1-1.2x at best on Strix. |
| Fused int8 WMMA prompt MoE (from Strata) | Ported and correct, but no end-to-end gain on the R9700 or RX 7900 XT so far. |
| llama.cpp's RDNA4 MMQ patch ([#25940](https://github.com/ggml-org/llama.cpp/pull/25940)) | No repeatable gain for Maya's IQ formats. |
| Shared expert on a second stream (Strix) | Correct, identical output, but within noise (17.2 vs 17.9 tok/s). |
| HIP graphs for decode | Tiny kernels are only ~5% of a Strix token; the time is in reading weights. |
| Stopping stale speculation early (two GPUs) | The second card sets the pace; the first card's wasted work runs in its idle time. |
| Rebalancing the two-GPU split | Moving layers just moves the VRAM shortage; the default split is best. |
| Tuning the CPU expert lane | It is worth +11 tok/s on two GPUs, and its automatic split is already the best of those we tried. |

## Ceilings to keep in mind

- **Strix Halo decode:** about 24-26 tok/s at best with Maya-S. The quant reads ~9 GB of weights per token, and the chip
  reads ~240 GB/s. Today's 17.8 is limited mostly by the dense weights, which already run at ~91% of bandwidth. Getting
  past this needs a smaller quant or speculation.
- **Any single card:** decode is set by how many experts fit in VRAM. More VRAM beats faster kernels.
