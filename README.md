# Project Maya on AMD

This branch collects the work to run **[Project Maya](https://github.com/mw00/project-maya)** on AMD GPUs. Maya runs
GLM-5.3-Flash, a 321-billion-parameter mixture-of-experts model, on ordinary PCs. The code lives upstream: everything
here has been sent there as pull requests, and most of it is merged. This page is the overview: what works, how fast
it is, how to run it, and what's next.

> **Status: experimental.** Linux with ROCm 7, text only. One GPU, or two discrete GPUs split by layers. ROCm 10.2
> TheRock nightlies have been evaluated on Strix Halo and R9700. The tested R9700 nightly regresses prompt speed;
> keep its ROCm 7.2.1 everyday build ([E46](EXPERIMENTS.md#e46---r9700-isolated-therock-runtime-comparison-dropped)).
>
> **Known issue:** Maya v1.0.15/16 can still crash with "illegal memory access" on AMD. It happens most often with
> long prompts and RAM-heavy modes (`STRATA_GLM_RAM_RESIDENT` and RAM shadows [#15](https://github.com/mw00/project-maya/pull/15)),
> and occasionally with the default tiers. Review found an async table-update race, a resident-mode background-promotion
> race, and a router path that indexes `INT_MAX` after NaN scores. A fix is being verified. Until then, on one R9700 or
> RX 7900 XT keep prompts short and use the default tiers unless running the tested integration fixes.
> The Strix integration build has completed recall requests through ~240K tokens.

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

Maya-S v2 mixed quant (IQ2_XXS gate/up experts), greedy, measured on our machines. These are setup-specific results: prompt lengths,
ROCm versions and run counts differ, so the notes say when a number is a small or single run.

| Setup | Prefill | Decode | Notes |
|---|---|---|---|
| RX 7900 XT (20 GB) | ~415 tok/s | ~15-16.4 tok/s | Local v1.0.15 baseline; [#19](https://github.com/mw00/project-maya/pull/19) reports ~16.4 with faster RDNA3/3.5 kernels. The v1.0.23 integration build measures ~570 tok/s prefill at 4K and ~18.3 tok/s decode (90 GB RAM). RAM shadows [#15](https://github.com/mw00/project-maya/pull/15) are on hold. |
| AI PRO R9700 (32 GB) | ~782 tok/s at 8K, ~743 at 119K (128K context, INT8 latents) | ~25 tok/s (256-token replies, 90 GB RAM + tips below) | v1.0.23 integration build + [#38](https://github.com/mw00/project-maya/pull/38)/[#39](https://github.com/mw00/project-maya/pull/39), `PROMOTE_MIN=6`, three scored requests per arm. With 48 GB RAM the same build gives 21.2-21.5 tok/s decode. The earlier 28.1 figure was a measurement artifact (see the correction below). |
| Strix Halo (128 GB unified) | ~343/337/327/314 tok/s at 8K/32K/64K/120K (262144 context, FP16 latents) | ~17 short-prompt / ~15.5 after 120K (256-token replies) | v1.0.24 + prompt-tail skip + fused gfx11 MoE fix, TheRock ROCm 10.2. One request per cell; reserving 262K instead of 64K changes matched-length prompt speed by at most 1.6%. All experts fit at startup, but prompt lending evicts some; decode still fetches from disk. See [E41](EXPERIMENTS.md#e41---strix-long-context-ladder-kept). |
| R9700 + RX 7900 XT | ~690/~756 tok/s at 4K/8K | ~38 tok/s | v1.0.23 integration build, [#14](https://github.com/mw00/project-maya/pull/14) layer split, MTP drafts on the second card, ~76% accepted. |

The test box with the discrete cards has 192 GB of RAM, so experts that don't fit in VRAM come from pinned RAM rather
than the SSD. With less RAM, decode is slower.

> **Correction (2026-10-09).** The single-R9700 decode figure was 28.1 tok/s, and the apparent drop to about 22 on later
> builds was taken as a regression. Neither holds. The 28.1 run's replies averaged about 26 tokens; short replies stay on
> VRAM-resident experts and decode faster (a 32-token reply gives 24.6 tok/s). With 256-token replies, v1.0.16 and v1.0.23
> both give 21.2-21.5 tok/s at 48 GB RAM. No decode regression was found.
>
> **Tips for one R9700 (2026-10-09).** Measured with 256-token replies at 40K context, 48 GB RAM unless noted:
> - `STRATA_GLM_RESERVE_MB=1024`: 82 instead of 75 expert slots per layer, 22.6 tok/s (+5.3%).
> - `STRATA_GLM_RAM_GB=90`: 24.5 tok/s (+14.2%).
> - Both together: 25.0 tok/s (+16.5%). Prompt speed is unaffected (about 800 tok/s at 28K).
>
> **Recommended R9700 settings (2026-10-09).** For everyday use on one R9700 with plenty of host RAM: 128K context
> (`--max-context 131072`) with `STRATA_GLM_KV_INT8=1`, `STRATA_GLM_RAM_GB=90`, `STRATA_GLM_RESERVE_MB=1024`,
> `STRATA_GLM_PROMOTE_MIN=6`. That holds ~25 tok/s decode and ~782 tok/s prompt speed at 8K (~743 at 119K) —
> within 1-2% of the 40K-context speed. The 1M model maximum runs but costs 12% decode and 31-37% prompt speed.

> **Strix context choice (2026-10-09).** The tested integration config uses `--max-context 262144 --prefill 32768`,
> FP16 latents, `STRATA_GLM_PREFILL_SUB=4096`, `STRATA_GLM_PREFILL_MB=12288`, and the gfx1151 hipBLASLt table for
> library version 100500. The pool retains 12,096 expert slots (80.17 GB) with 3.32 GB of state/KV reserved.
> Muse reports 12/12 single-needle and 4/4 multi-hop hits through ~240K; raw logs confirm those request lengths,
> but the reply texts were not retained for independent rescoring. This is a small recall sample, not a general
> quality result. Longer actual prompts still reduce decode speed; the small reservation cost is a separate finding.

Current-base gfx1151 fused-MoE validation (v1.0.26): same-binary prefill
+11.3%/+7.1%/+5.1% at ~4K/~8K/~16K; 48 fused-parity cases pass.
Small sequential samples, provenance gaps and 3/9 changed prefill replies
prevent a quality-equivalence claim. Everyday 262K server restored; no new
default/code push. See [E48](EXPERIMENTS.md#e48---gfx1151-fused-moe-on-v1026-kept).

## Quality sanity check

The shared single-GPU speculation prototype on Strix has an independently
verified4x300 token match and observed21–23 tok/s, but its ON/OFF binaries
differ, two timing-gate cells exceed1.4 and final-build test evidence is
incomplete. It is not the everyday configuration or a claimed R9700 speedup;
see [E47](EXPERIMENTS.md#e47---two-row-decode--single-gpu-speculation-prototype-open).

One R9700, 128K INT8 config: GSM8K 37/40, MMLU-Pro 48/56, HumanEval 30/30,
IFEval 29/40 strict (35/40 loose), tools 11/12. All 183 short-task cases completed
without request errors or truncation. Writing was coherent but missed some explicit
constraints. These are small samples, not full benchmark or AMD/NVIDIA equivalence
claims. See [QUALITY.md](QUALITY.md) for method, grading correction and limitations.

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
| [#38](https://github.com/mw00/project-maya/pull/38) | RDNA4 `wmma2` prompt attention | open; rebased, awaiting maintainer's CUDA check |
| [#39](https://github.com/mw00/project-maya/pull/39) | Fix tier/lending races + invalid-route indexing (crash fix) | open; rebased, awaiting maintainer's CUDA check |
| [#52](https://github.com/mw00/project-maya/pull/52) | HIP mappings for the device queries #44 uses (build fix) | merged |
| [#61](https://github.com/mw00/project-maya/pull/61) | Skip the last layer's unused prompt outputs on one device (+1-1.4%, byte-identical) | open |

The community PRs #24-#26 were merged in v1.0.15; the HIP fix needed by #24 is in upstream too. [#15](https://github.com/mw00/project-maya/pull/15)
is open and on hold while the crash is fixed.

Also see the [roadmap](ROADMAP.md), the [engineering notes](NOTES.md) and the [experiment log](EXPERIMENTS.md).

## Credits

[Project Maya](https://github.com/mw00/project-maya) is by mw00, who reviewed and merged this work and checked every
change against NVIDIA. It grew out of [Strata](https://github.com/Niko1221/Strata) by Niko1221, which several of the
AMD techniques here come from. The AMD work was AI-assisted; see [NOTES.md](NOTES.md#how-this-was-built).
