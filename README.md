# Project Maya on AMD

This branch collects the work to run **[Project Maya](https://github.com/mw00/project-maya)** on AMD GPUs. Maya runs
GLM-5.3-Flash, a 321-billion-parameter mixture-of-experts model, on ordinary PCs. The code lives upstream: everything
here has been sent there as pull requests, and most of it is merged. This page is the overview: what works, how fast
it is, how to run it, and what's next.

> **Status: experimental.** Linux with ROCm 7, text only. One GPU, or two discrete GPUs split by layers.
>
> **Known issue:** on a single RX 7900 XT (20 GB) with a 90 GB RAM tier, the RAM-resident tier (`STRATA_GLM_RAM_RESIDENT`)
> and RAM shadows ([#15](https://github.com/mw00/project-maya/pull/15)) crash with "illegal memory access" within 2-4
> requests. Don't use either on a 7900 XT. The default tiers are stable. Under investigation.

## What works

| Hardware | Architecture | Status |
|---|---|---|
| Radeon RX 7900 XT / XTX | gfx1100 (RDNA3) | Supported since Maya v1.0.11 |
| Radeon AI PRO R9700 / RX 9070 | gfx1201 (RDNA4) | Supported since v1.0.11 |
| Two of the above (layer split + MTP drafting) | mixed is fine | Supported since v1.0.14 |
| Strix Halo: Ryzen AI Max+ 395, Radeon 8060S | gfx1151 (RDNA3.5, unified memory) | Supported since v1.0.15 |

## How fast

Maya-S quant (IQ2_XXS experts), 8K context, greedy, measured on our machines. Prefill uses 4K-token prompts; decode
is the answer speed.

| Setup | Prefill | Decode | Notes |
|---|---|---|---|
| RX 7900 XT (20 GB) | ~415 tok/s | ~15 tok/s | 16.4 with faster expert kernels ([#19](https://github.com/mw00/project-maya/pull/19)); ~19 with RAM shadows ([#15](https://github.com/mw00/project-maya/pull/15), on hold) |
| AI PRO R9700 (32 GB) | ~500-620 tok/s | ~23-25.5 tok/s | 25.5 with RAM-resident tier + PROMOTE_MIN (v1.0.15) |
| Strix Halo (128 GB unified) | ~251 tok/s (v1.0.15) | ~17.8 tok/s | every expert fits in GPU memory |
| R9700 + RX 7900 XT | ~490 tok/s | ~34-36 tok/s | MTP drafts on the second card, ~76% accepted |

The test box with the discrete cards has 192 GB of RAM, so experts that don't fit in VRAM come from pinned RAM rather
than the SSD. With less RAM, decode is slower.

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
notes.

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

Also see the [roadmap](ROADMAP.md) and the [engineering notes](NOTES.md).

## Credits

[Project Maya](https://github.com/mw00/project-maya) is by mw00, who reviewed and merged this work and checked every
change against NVIDIA. It grew out of [Strata](https://github.com/Niko1221/Strata) by Niko1221, which several of the
AMD techniques here come from. The AMD work was AI-assisted; see [NOTES.md](NOTES.md#how-this-was-built).
