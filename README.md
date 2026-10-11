# Project Maya on AMD

This branch collects the work to run **[Project Maya](https://github.com/mw00/project-maya)** on AMD GPUs. Maya runs
GLM-5.3-Flash, a 321-billion-parameter mixture-of-experts model, on ordinary PCs. The code lives upstream: everything
here has been sent there as pull requests, and most of it is merged. This page is the overview: what works, how fast
it is, how to run it, and what's next.

> **Status: experimental.** Linux with ROCm 7, text only. One GPU, or two discrete GPUs split by layers. ROCm 10.2
> TheRock nightlies have been evaluated on Strix Halo and R9700. The tested R9700 nightly regresses prompt speed;
> keep its ROCm 7.2.1 everyday build ([E46](EXPERIMENTS.md#e46---r9700-isolated-therock-runtime-comparison-dropped)).
>
> **Crash fix merged.** The "illegal memory access" reports on AMD were traced to an async table-update race, a
> resident-mode background-promotion race, and a router path that indexed `INT_MAX` after NaN scores. The fixes are
> upstream as of v1.0.32 (PR [#39](https://github.com/mw00/project-maya/pull/39)). The tested integration build has
> completed recall requests through ~240K tokens on Strix. RAM shadows
> ([#15](https://github.com/mw00/project-maya/pull/15)) remain on hold.

## What works

| Hardware | Architecture | Status |
|---|---|---|
| Radeon RX 7900 XT / XTX | gfx1100 (RDNA3) | Supported since Maya v1.0.11 |
| Radeon AI PRO R9700 / RX 9070 | gfx1201 (RDNA4) | Supported since v1.0.11 |
| Two of the above (layer split + MTP drafting) | mixed is fine | Supported since v1.0.14 |
| Strix Halo: Ryzen AI Max+ 395, Radeon 8060S | gfx1151 (RDNA3.5, unified memory) | Supported since v1.0.15 |

## Quantization variants

All three are mixed quants made directly from Z.ai's FP8 GLM-5.3-Flash release:

| Quant | Model size | Routed expert gate/up | Routed expert down |
|---|---|---|---|
| Maya-S v2 (our tested model) | 96.5 GB | IQ2_XXS | IQ2_S; IQ3_XXS in sensitive layers |
| Maya-M | 116.0 GB | IQ2_S | IQ3_XXS; IQ3_S in sensitive layers |
| Maya-L | 156.3 GB | IQ3_S | IQ4_XS; Q5_K in sensitive layers |

Sensitive layers here means the first and last four MoE layers. Attention and shared experts remain Q6_K
in all three. See [QUANTS.md](QUANTS.md) for the full tensor recipes, source links, draft-layer formats and
memory implications. M/L speeds have not been measured on our AMD machines.

## How fast

Maya-S v2 mixed quant (IQ2_XXS gate/up experts), greedy, measured on our machines. These are setup-specific results:
prompt lengths, ROCm versions and run counts differ, so the notes label single runs and small samples.

| Setup | Prefill | Decode | Notes |
|---|---|---|---|
| AI PRO R9700 (32 GB) | ~800 tok/s at 8K/32K | ~26.6 tok/s (256-token replies) | Maya v1.0.32, 128K context, INT8 latents, RAM tier 90 GB, `RESERVE_MB 512`. Single runs of small prompt samples. |
| Strix Halo (128 GB unified) | ~347 / ~340 / ~334 tok/s at 8K/32K/64K | ~23.7 tok/s (4-type mean) | Maya v1.0.32 + fused prompt MoE ([#98](https://github.com/mw00/project-maya/pull/98)) + #96 MTP + our verify-fuse branch, `RAM_GB=8`, wmma2, TheRock ROCm 10.2, 262144 context. |
| Strix Halo, MTP speculation (#96 + verify-fuse) | ~340 tok/s at 32K | verify round 78.1 -> 73.9 ms; +17.7% over no-spec | Our verify-fuse branch (batched MLA `k_b/v_b` + fused union routed MoE), bit-identical. #96 carries the MTP path; [#113](https://github.com/mw00/project-maya/pull/113) carries the gfx1151 wmma2 default. |
| R9700 + RX 7900 XT | ~721 at 8K, ~1129 at 32K | ~40.3 tok/s | Maya v1.0.33 `build-multi` + `RESERVE_MB 1024` + wmma2 + `PREFILL_SUB 2048` + KV INT8, 128K, auto split K=24, MTP drafts on the second card, 80.2% accepted; decode after 32K 38.2. Its 8K prompt is -10% vs one R9700 (a single 8K chunk cannot overlap the cards). |
| RX 7900 XT (20 GB) | ~570 tok/s at 4K | ~18.3 tok/s | v1.0.23 integration build, 90 GB RAM. Our upstream [#95](https://github.com/mw00/project-maya/pull/95) comment measures RDNA GEMV here at +1.1% decode, bit-exact. RAM shadows [#15](https://github.com/mw00/project-maya/pull/15) are on hold. |

The test box with the discrete cards has 192 GB of RAM, so experts that don't fit in VRAM come from pinned RAM rather
than the SSD. With less RAM, decode is slower.

> **Correction (2026-10-09, updated 2026-10-11).** The single-R9700 decode figure was 28.1 tok/s, and the apparent
> drop to about 22 on later builds was taken as a regression. Neither holds. The 28.1 run's replies averaged about 26
> tokens; short replies stay on VRAM-resident experts and decode faster. With 256-token replies the old build gave
> ~25 tok/s (21.2–21.5 at 48 GB RAM); the current everyday config is ~26.6 tok/s with the 90 GB RAM tier and
> `RESERVE_MB 512`. No decode regression was found.

### Recommended settings

- **One R9700, everyday:** 128K context (`--max-context 131072`) with INT8 latents (`STRATA_GLM_KV_INT8=1`),
  `STRATA_GLM_RAM_GB=90`, `STRATA_GLM_RESERVE_MB=512`, `STRATA_GLM_PROMOTE_MIN=6`. That holds ~26.6 tok/s decode and
  ~800 tok/s prompt speed at 8K/32K. A larger prompt chunk of 32768 measured +4.1% at 32K (single run). The 1M
  model maximum runs but costs ~12% decode and 31–37% prompt speed.
- **Strix Halo, everyday:** `--max-context 262144`, `STRATA_GLM_RAM_GB=8`, `STRATA_GLM_PREFILL_ATTN=wmma2`,
  `amd_iommu=off`, FP16 latents; the everyday exe is our verify-fuse branch (v1.0.32 + fused prompt MoE #98 +
  #96 MTP + batched MLA `k_b/v_b` + fused union routed MoE). Decode ~23.7 tok/s (4-type mean) and prompt
  ~347/340/334 tok/s at 8K/32K/64K. The pool retains 12,096 expert slots (80.17 GB); reserving 262K instead of 64K
  changes matched-length prompt speed by at most 1.6%.
- **Two cards:** `--gpus 0,1`; setup puts the larger card first. GPU numbers follow the KFD order that `--check` prints.
  The best measured dual config is `maya-2gpu-best-7900.json` (v1.0.33 + `RESERVE_MB 1024` + wmma2 +
  `PREFILL_SUB 2048` + KV INT8, 128K): ~40.3 tok/s decode, ~1129 tok/s at 32K.

> **Strix context (2026-10-09).** Muse reports 12/12 single-needle and 4/4 multi-hop hits through ~240K; raw logs
> confirm those request lengths, but the reply texts were not retained for independent rescoring. This is a small
> recall sample, not a general quality result. Longer actual prompts still reduce decode speed.

Current-base gfx1151 fused-MoE validation (v1.0.26): same-binary prefill
+11.3%/+7.1%/+5.1% at ~4K/~8K/~16K; 48 fused-parity cases pass.
Small sequential samples, provenance gaps and 3/9 changed prefill replies
prevent a quality-equivalence claim. Everyday 262K server restored; the fix is
now open upstream as [#98](https://github.com/mw00/project-maya/pull/98).
See [E48](EXPERIMENTS.md#e48---gfx1151-fused-moe-on-v1026-kept).

## Where the time goes (theoretical vs actual)

A public summary of the efficiency audits. Estimates are labelled; everything else is a measurement on our machines.

- **Prefill is compute-bound on both chips.** On Strix, removing all SSD re-reads gave no prefill gain (single run),
  and the phase profile shows the time in compute, not copies. Prompt-side optimizations should target math, not I/O.
- **The Strix prefill ceiling was 2.4x lower than the spec sheet.** The assumed 59 TFLOPS was the 2x-FP32/WMMA spec;
  the WMMA f16 figure is ~30 TFLOPS and hipBLASLt measures 24.2 TFLOPS (20.9 at the ~2.47 GHz, 85 W "balanced"
  prefill clock). Against that measured ceiling the engine is at ~50%, not 21%.
- **Where R9700 prefill time goes (measured shares, one profile):** attention ~21%, KDA projections ~19%, MoE ~27%,
  the rest in the remaining dense projections and glue. This is why MoE and attention kernels are the prefill levers.
- **GEMM tuning is a small lever.** The hipBLASLt GEMMs already run at ~66% of the R9700's FP16 peak (154 TFLOPS at
  2350 MHz), but GEMMs are only ~11% of prefill time. Better GEMM selection is worth roughly 1–4% end-to-end
  (estimate); FP8 GEMMs could be ~2x for the future. GEMM tuning is parked for now.
- **The other engines are not a like-for-like comparison.** Halogen/Gufo/CIRU prefill looks several times faster
  because their Qwen3.8-Flash-Next activates ~6B parameters per token, against GLM's ~18B. Very roughly ~3x of the gap
  is the model and ~1.5x is engine efficiency (estimates, different engines and quants, not rerun by us).
- **Decode is a bandwidth ceiling.** Strix Halo's quant reads ~9 GB of weights per token against ~240 GB/s, giving a
  rough ~28–30 tok/s ceiling; the measured ~17–18 tok/s is below it. Closing that gap needs a smaller quant, more
  VRAM, or speculation.

The R9700 prefill profile is [E75](EXPERIMENTS.md#e75---r9700-prefill-profile-compute-bound-transfers-hidden-kept),
the GEMM audit is [E76](EXPERIMENTS.md#e76---r9700-gemm-efficiency-audit-dropped), and the closed Strix disk/tier
hypothesis is [E77](EXPERIMENTS.md#e77---strix-prefill-disktier-stall-hypothesis-closed-dropped).

## Quality sanity check

One R9700, 128K INT8 config: math (GSM8K) 37/40 (92.5%), MMLU-Pro 48/56 (85.7%), HumanEval 30/30 (100%),
IFEval 29/40 strict (72.5%) / 35/40 loose (87.5%), tools 11/12. All 183 short-task cases completed without request
errors or truncation. Writing was coherent but missed some explicit constraints. These are **small samples of a
~2-bit quant**, not full benchmark or AMD/NVIDIA equivalence claims.

Long-context needle-in-a-haystack recall at 32–122K sometimes garbles exact digits (for example 4817→4481,
KQ-2291→KQ-2219), while multi-hop sums score 3/3; quant precision is one hypothesis, not an established cause.
See [QUALITY.md](QUALITY.md) for method, the grading correction and limitations, and
[E44](EXPERIMENTS.md#e44---niah-recall-garbles-exact-digits-open).

## How to run it

On Linux with ROCm 7 installed (`/opt/rocm` or a versioned `/opt/rocm-7.x.y`):

```sh
git clone https://github.com/mw00/project-maya && cd project-maya
./maya.sh --backend hip --gpu 0 --check                 # finds your AMD GPUs and ROCm
./maya.sh --backend hip --gpu 0 --context 8192 --no-vision --yes --download-model
./maya.sh --backend hip                                 # starts the server and dashboard
```

- Two cards: use `--gpus 0,1` instead of `--gpu 0`. Setup puts the larger card first.
- GPU numbers follow the KFD order that `--check` prints.
- `--bench` and `--report` work on AMD (v1.0.15, [#18](https://github.com/mw00/project-maya/pull/18)).

Upstream's [docs/AMD_MAYA.md](https://github.com/mw00/project-maya/blob/main/docs/AMD_MAYA.md) has the full setup
notes. The ROCm 10.2 tests used isolated TheRock virtual environments; neither changed system ROCm.
On R9700, nightly `10.2.0a20261009` plus a fresh gfx1201 table gave ~537/~581 prompt tok/s at
8.3K/33.1K versus ~785/~782 on the repeated ROCm 7.2.1 baseline (128K reserved). Observed decode
28 versus 26 tok/s used differing replies and is not a clean kernel-speed gain. Defaults are unchanged;
see [E46](EXPERIMENTS.md#e46---r9700-isolated-therock-runtime-comparison-dropped) for gates and limitations,
and [NOTES.md](NOTES.md#hip--rocm-lessons) for the earlier Strix evaluation path.

## Pull requests

| PR | What | State |
|---|---|---|
| [#2](https://github.com/mw00/project-maya/pull/2) | HIP memory fences for the GPU-to-CPU handoff | merged |
| [#7](https://github.com/mw00/project-maya/pull/7) | Linux HIP setup, R9700, faster prompts (hipBLASLt tables, bigger sub-batches) | merged, v1.0.11 |
| [#14](https://github.com/mw00/project-maya/pull/14) | Two GPUs with MTP drafting; one hipBLASLt table per card | merged, v1.0.14 |
| [#15](https://github.com/mw00/project-maya/pull/15) | Opt-in RAM shadows: about 3x fewer SSD reads | on hold: crash under investigation |
| [#16](https://github.com/mw00/project-maya/pull/16) | rocWMMA prompt attention + FP16 MLA (Strix prompts +16%) | merged, v1.0.15 |
| [#17](https://github.com/mw00/project-maya/pull/17) | Strix Halo: unified-memory sizing, installer support | merged, v1.0.15 |
| [#18](https://github.com/mw00/project-maya/pull/18) | `--bench` / `--report` on AMD | merged, v1.0.15 |
| [#19](https://github.com/mw00/project-maya/pull/19) | Faster RDNA decode expert kernels (7900 XT +8.6%, bit-identical) | merged, v1.0.15 |
| [#24](https://github.com/mw00/project-maya/pull/24) | PCIe link wake and RAM-demotion serving | merged, v1.0.15 |
| [#25](https://github.com/mw00/project-maya/pull/25) | RAM-resident expert tier | merged, v1.0.15 |
| [#26](https://github.com/mw00/project-maya/pull/26) | `PROMOTE_MIN`, to avoid one-off promotions | merged, v1.0.15 |
| [#38](https://github.com/mw00/project-maya/pull/38) | RDNA4 `wmma2` prompt attention | merged |
| [#39](https://github.com/mw00/project-maya/pull/39) | Fix tier/lending races + invalid-route indexing (crash fix) | merged |
| [#52](https://github.com/mw00/project-maya/pull/52) | HIP mappings for the device queries #44 uses (build fix) | merged |
| [#61](https://github.com/mw00/project-maya/pull/61) | Skip the last layer's unused prompt outputs on one device (+1-1.4%, byte-identical) | merged |
| [#98](https://github.com/mw00/project-maya/pull/98) | Fused prompt MoE on Strix Halo gfx1151 (+13.5/+8.1/+5.0% at 4/8/16K) | open; waiting on the maintainer's Windows build |
| [#113](https://github.com/mw00/project-maya/pull/113) | gfx1151 default to wave32 WMMA (wmma2) prompt attention (+3.7/+3.8/+4.3% at 8/32/64K) | open |

We also posted measurements on upstream PRs:
[#95](https://github.com/mw00/project-maya/pull/95) (RDNA GEMV on RX 7900 XT: +1.1% decode, bit-exact) and
[#96](https://github.com/mw00/project-maya/pull/96) (single-GPU MTP: on R9700 the gate keeps it off at ~28% experts in
VRAM; forced, it gives reasoning +12.6%, chat +5.5%, code +3.3%, prose −4.6%; on Strix it is part of the everyday
build, with our verify-fuse branch on top).

Also see the [roadmap](ROADMAP.md), the [engineering notes](NOTES.md) and the [experiment log](EXPERIMENTS.md).

## Credits

[Project Maya](https://github.com/mw00/project-maya) is by mw00, who reviewed and merged this work and checked every
change against NVIDIA. It grew out of [Strata](https://github.com/Niko1221/Strata) by Niko1221, which several of the
AMD techniques here come from. The AMD work was AI-assisted; see [NOTES.md](NOTES.md#how-this-was-built).
