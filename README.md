# Project Maya on AMD

This branch collects the work to run **[Project Maya](https://github.com/mw00/project-maya)** on AMD GPUs. Maya runs
GLM-5.3-Flash, a 321-billion-parameter mixture-of-experts model, on ordinary PCs. The code lives upstream: everything
here has been sent there as pull requests, and most of it is merged. This page is the overview: what works, how fast
it is, how to run it, and what's next.

> **Status: experimental.** Linux with ROCm 7, text only. One GPU, or two discrete GPUs split by layers. ROCm 10.2
> TheRock nightlies have been evaluated on Strix Halo; the R9700 path is still to do.
>
> **Known issue:** Maya v1.0.15/16 can still crash with "illegal memory access" on AMD. It happens most often with
> long prompts and RAM-heavy modes (`STRATA_GLM_RAM_RESIDENT` and RAM shadows [#15](https://github.com/mw00/project-maya/pull/15)),
> and occasionally with the default tiers. Review found an async table-update race, a resident-mode background-promotion
> race, and a router path that indexes `INT_MAX` after NaN scores. A fix is being verified. Until then, on one R9700 or
> RX 7900 XT keep prompts short and use the default tiers. Strix Halo has run 8-30K-token prompts cleanly.

## What works

| Hardware | Architecture | Status |
|---|---|---|
| Radeon RX 7900 XT / XTX | gfx1100 (RDNA3) | Supported since Maya v1.0.11 |
| Radeon AI PRO R9700 / RX 9070 | gfx1201 (RDNA4) | Supported since v1.0.11 |
| Two of the above (layer split + MTP drafting) | mixed is fine | Supported since v1.0.14 |
| Strix Halo: Ryzen AI Max+ 395, Radeon 8060S | gfx1151 (RDNA3.5, unified memory) | Supported since v1.0.15 |

## How fast

Maya-S quant (IQ2_XXS experts), greedy, measured on our machines. These are setup-specific results: prompt lengths,
ROCm versions and run counts differ, so the notes say when a number is a small or single run.

| Setup | Prefill | Decode | Notes |
|---|---|---|---|
| RX 7900 XT (20 GB) | ~415 tok/s | ~15-16.4 tok/s | Local v1.0.15 baseline; [#19](https://github.com/mw00/project-maya/pull/19) reports ~16.4 with faster RDNA3/3.5 kernels. RAM shadows [#15](https://github.com/mw00/project-maya/pull/15) are on hold. |
| AI PRO R9700 (32 GB) | ~749-830 tok/s | 28.1 tok/s | v1.0.16. [#38](https://github.com/mw00/project-maya/pull/38) measured ~749-801 at ~4K and ~828-830 at ~8K in two-request checks; the typical-user report measured 749-781 at 8-20K. Decode is two clean scored passes with 48 GB RAM and `PROMOTE_MIN=6`, so treat it as a local result. |
| Strix Halo (128 GB unified) | 258-273 tok/s (ROCm 7.2.2); 276/290/283 tok/s at 4/8/16K (ROCm 10.2 nightly) | 17.2-17.8 tok/s | v1.0.16 sweep, best `SUB=4096`. One ROCm 10.2 comparison was stable and added 5-11% to prefill; decode was unchanged. 64K setup is in progress. |
| R9700 + RX 7900 XT | ~490 tok/s | ~34-36 tok/s | Local two-GPU v1.0.15 run; [#14](https://github.com/mw00/project-maya/pull/14), MTP drafts on the second card, ~76% accepted. |

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
notes. The ROCm 10.2 Strix test used a TheRock nightly in a virtual environment; it did not change the system ROCm
install. See [NOTES.md](NOTES.md#hip--rocm-lessons) for that evaluation path.

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
| [#38](https://github.com/mw00/project-maya/pull/38) | RDNA4 `wmma2` prompt attention | open; R9700 verification complete |

The community PRs #24-#26 were merged in v1.0.15; the HIP fix needed by #24 is in upstream too. [#15](https://github.com/mw00/project-maya/pull/15)
is open and on hold while the crash is fixed.

Also see the [roadmap](ROADMAP.md) and the [engineering notes](NOTES.md).

## Credits

[Project Maya](https://github.com/mw00/project-maya) is by mw00, who reviewed and merged this work and checked every
change against NVIDIA. It grew out of [Strata](https://github.com/Niko1221/Strata) by Niko1221, which several of the
AMD techniques here come from. The AMD work was AI-assisted; see [NOTES.md](NOTES.md#how-this-was-built).
