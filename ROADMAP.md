# Roadmap

The priority hardware is what most people have: **one R9700 / RX 9070**, and **Strix Halo**. Two-GPU setups and the
RX 7900 XT come second.

## Now

- **Strix Halo is the long-context machine right now.** nimo is being set up for 64K with v1.0.16 and ROCm 10.2;
  it has run 8-30K-token prompts cleanly.
- **Crash fix + upstream PR.** v1.0.15/16 can hit an AMD "illegal memory access", especially on long prompts and
  RAM-heavy modes (`RAM_RESIDENT` and RAM shadows), and sometimes with defaults. Review found that pinned table-update
  buffers can be rewritten while an earlier async copy still reads them; prompt lending creates a second boundary where
  this can happen. Resident mode also has a background-promotion race. If the tables are corrupted, NaN router scores
  can leave `INT_MAX` as an index. A fix is being verified and then needs to go upstream. Until then, use short prompts
  and default tiers on a single R9700 or RX 7900 XT. Strix Halo has run 8-30K-token prompts cleanly.
- **Usable long context on the R9700.** v1.0.16 measured 749-781 tok/s prefill at 8-20K on one card, but the same
  typical-user run crashed on 7/13 prompts above 7K and 0/3 around 28K. Reproduce the failure, verify the fix, and
  publish settings that can handle long prompts before calling this done.
- **ROCm 10 on the R9700.** TheRock 10.2 nightlies are a pip-wheel evaluation path in a virtual environment. Strix
  Halo gained 5-11% prefill in one stable comparison and decode did not move; build the R9700 path and retune its
  hipBLASLt table before recommending it there.
- **RDNA4 prompt attention.** [#38](https://github.com/mw00/project-maya/pull/38) is open. Its `wmma2` default for gfx12
  passed parity and measured about 750-800 tok/s at 4K and about 830 tok/s at 8K on the R9700, versus roughly 680-767
  for [#16](https://github.com/mw00/project-maya/pull/16)'s arm.

## Next

- **Fused WMMA prompt MoE for gfx1151.** A separate Strix-Halo-only engine reports 1,584 tok/s prefill on 8K prompts
  and 37.6 tok/s serial decode; its draft head reaches 46 tok/s at 32K context and 46-56 tok/s on its tested
  workloads. That shows prefill headroom beyond Maya's current 258-290 tok/s, but it is a different model and engine,
  so use it as a direction, not a directly comparable benchmark.
- **Single-GPU speculation on Strix Halo (reopened).** Measure expert overlap first. The separate Strix engine gets about
  +22% from a draft head on one machine; Maya still needs its own measurement.
- **RDNA4 decode expert kernels.** [#19](https://github.com/mw00/project-maya/pull/19) tuned the expert kernels for
  RDNA3/3.5 and left the R9700 on the old ones. Tuning them for gfx1201 is the obvious next decode step for the R9700.
- **Measure `amd_iommu=off` on Strix Halo.** A separate Strix engine reports 13-16% higher prefill with it off on one
  machine; Maya has not measured that setting yet, and it changes DMA-translation behavior machine-wide.
- **Recommended single-R9700 settings in the AMD docs** once the crash fix lands.
- **Answer review comments** on the open PRs as they come in.

## Looked at, not worth it (for now)

| Idea | Why not |
|---|---|
| Single-GPU MTP speculation | **Reopened:** measure expert overlap first. Consecutive tokens share only ~30% of their experts (#26's data), so verifying two tokens may cost almost twice the expert reads. The earlier 1.1-1.2x estimate needs a fresh Strix measurement. |
| Fused int8 WMMA prompt MoE (from Strata) | Ported and correct, but no end-to-end gain on the R9700 or RX 7900 XT so far. |
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
