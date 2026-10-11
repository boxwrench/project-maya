# Experiment log

What was tried on the AMD port of Project Maya (GLM-5.3-Flash on ROCm/HIP), what happened, and what was decided. It is
built only from records that exist: the upstream PR descriptions and comments, the benchmark folders, and the agent
reports. Where a number is a single run, a small run, or noisy, the entry says so. Where no record exists, the entry says
that instead of filling the gap.

Last updated 2026-10-11; E49-E69 consolidate the current working records (source, CPU, build and GPU audits), E70-E77 record the v1.0.32 everyday measurements, E79-E90 record the 2026-10-11 Strix decode/R9700 tuneup work, and E91-E99 the afternoon's verify-fuse, prefill, quality-bar and dual-card results. A machine-readable copy is kept next to the benchmark data (`experiments.jsonl`, same IDs).

## How to read this

Each entry has: **Question** (what we wanted to know), **Setup** (hardware, versions, key settings), **Result** (the
numbers and how many runs), **Decision** (what we did about it), **Evidence** (a PR link or a local file path).
"Local evidence" paths are files on our machines and are not published. "Caveats" lists what limits the result.

| Status | Meaning |
|---|---|
| KEPT | Shipped or merged, or kept as a recorded result |
| KEPT-OPEN | The upstream PR is open |
| DROPPED | No gain, or not worth it |
| FIXED | A bug that was found and fixed |
| OPEN | In progress, not measured yet, or no result on file |

All of this is experimental local work: run counts are small on purpose (see Lessons). Numbers from other projects are
labelled as theirs and were not rerun. All entries are diagnostic records imported after the fact (evidence class D),
not controlled studies.

## Index

| ID | Theme | Experiment | Status |
|---|---|---|---|
| E01 | Prompt speed | hipBLASLt tables, sub-batch and chunk size | KEPT |
| E02 | Prompt speed | rocWMMA prompt attention and FP16 MLA products | KEPT |
| E03 | Prompt speed | wmma2 prompt attention on RDNA4 | KEPT |
| E04 | Prompt speed | Fused WMMA prompt MoE on Strix Halo | OPEN |
| E05 | Prompt speed | Strata's fused int8 WMMA prompt MoE on discrete cards | OPEN |
| E06 | Prompt speed | llama.cpp RDNA4 MMQ patch | DROPPED |
| E07 | Prompt speed | ROCm 10.2 (TheRock nightly) on Strix Halo | KEPT |
| E08 | Prompt speed | amd_iommu=off on Strix Halo | KEPT |
| E09 | Prompt speed | --prefill auto (PR #44) on Strix Halo | OPEN |
| E10 | Prompt speed | Why 256-token prompts are slow | DROPPED |
| E11 | Prompt speed | Terminal-layer prefix skip in single-GPU prefill | KEPT |
| E12 | Decode speed | Faster decode expert kernels on RDNA3/3.5 | KEPT |
| E13 | Decode speed | Decode expert kernels for RDNA4 (R9700) | DROPPED |
| E14 | Decode speed | CPU expert lane plans (two GPUs) | DROPPED |
| E15 | Decode speed | Shared expert on a second stream (Strix) | DROPPED |
| E16 | Decode speed | HIP graphs for decode | DROPPED |
| E17 | Decode speed | Stopping the stale speculative pass early (two GPUs) | DROPPED |
| E18 | Decode speed | Single-GPU speculation on Strix Halo | OPEN |
| E19 | Expert tiers | RAM shadows (STRATA_GLM_RAM_SHADOW) | KEPT-OPEN |
| E20 | Expert tiers | RAM-resident tier and PROMOTE_MIN on one R9700 | KEPT |
| E21 | Expert tiers | Lend budget (STRATA_GLM_PREFILL_MB) 4096 vs 1024 | DROPPED |
| E22 | Expert tiers | Low-RAM decode (48 GB) on one R9700 | OPEN |
| E23 | Two GPUs | Two-GPU layer split with MTP drafting | KEPT |
| E24 | Two GPUs | Split-point sweep and the automatic split | DROPPED |
| E25 | Two GPUs | One hipBLASLt table per architecture on a mixed pair | FIXED |
| E26 | Strix Halo | Unified-memory expert sizing | KEPT |
| E27 | Strix Halo | Host state on the Strix box (GTT use, IOMMU, background services) | FIXED |
| E28 | Stability | HIP doorbell publication and KDA test initialisation | FIXED |
| E29 | Stability | First attribution of the memory fault: interference (wrong) | FIXED |
| E30 | Stability | Root causes of the illegal memory access, and the fix (#39) | KEPT |
| E31 | Stability | Rebasing #38 and #39 onto v1.0.21 and v1.0.23 | KEPT |
| E32 | Stability | #44 breaks the HIP build (#52) | FIXED |
| E33 | Long context | Long prompts on one R9700, before the fix | FIXED |
| E34 | Long context | KV ring (#41) and 64K context on Strix Halo | KEPT |
| E35 | Quality | Quality of Maya on AMD | OPEN |
| E36 | Comparisons with other projects | OpenMOSE Strata-GLM-AMD (their numbers) | OPEN |
| E37 | Comparisons with other projects | Other engines: halogen-flash-server and glm53-flash-offload | OPEN |
| E38 | Prompt speed | R9700 prefill sub-batch 1024, 2048 and 4096 | DROPPED |
| E39 | Decode speed | Single-R9700 decode regression on upstream builds | DROPPED |
| E40 | Long context | R9700 long-context ladder, 40K to 1M | KEPT |
| E41 | Long context | Strix long-context ladder (partial) | OPEN |
| E42 | Quality | Tool-calling 400s were a stale frontend | FIXED |
| E43 | Tooling | DeepSeek Harness (dsh) set up for agent work | OPEN |
| E44 | Quality | NIAH recall garbles exact digits | OPEN |
| E45 | Prompt speed | R9700 transfer-window reuse audit | DROPPED |
| E46 | Runtime | R9700 isolated TheRock runtime comparison | DROPPED |
| E47 | Shared decode | Two-row decode / single-GPU speculation prototype | OPEN |
| E48 | Prompt speed | gfx1151 fused MoE on v1.0.26 | KEPT |
| E49 | Profile | Strix existing cold-prefix phase profile audited | KEPT |
| E50 | CPU preparation | Rotating CPU calibration | OPEN |
| E51 | Source/CPU preparation | GLM applicability and selector oracle | OPEN |
| E52 | Compile preparation | IQ2_XXS BF16 WMMA | OPEN |
| E53 | Reference audit | Halogen/CIRU/Gufo applicability | OPEN |
| E54 | Read-only audit | R9700 health and table-gap leads | OPEN |
| E55 | Documentation | S-v2/M/L quant recipes and untested estimates | OPEN |
| E56 | Upstream audit | Maya/Strata PR acceptance and applicability | OPEN |
| E57 | GPU validation | Strix CIRU-inspired BF16 synthetic comparison | DROPPED |
| E58 | CPU follow-up | R9700 Lt2048/calibration applicability | OPEN |
| E59 | CPU preparation | BF16 grouped64 + cleaned timing | KEPT |
| E60 | Strix GPU benchmark | BF16 v2 faster prototype; native still wins | DROPPED |
| E61 | Public-source research | Prefill commit methodology and tracking baseline | KEPT |
| E62 | Public-source research | Radiance R9700 prefill methodology and applicability | KEPT |
| E63 | Single-card CPU preparation | Radiance TP1 dispatch and native IQ packing leads | KEPT |
| E64 | Public XeStrata audit | Release techniques and focused commit applicability | KEPT |
| E65 | R9700 actual-shape GPU | E58 dispatch/parity and matched timing | KEPT |
| E66 | R9700 model prefill A/B | Original versus one-row-filtered Lt table | DROPPED |
| E67 | Two-PC validation queue | Packed-X qualification and native/Lt2048 refinements | OPEN |
| E68 | Public ROCmFPX audit | Long-context claim, commit mechanisms and comparability | KEPT |
| E69 | Documentation consolidation | Audited public drafts + current 69-ID summary/lead merge | KEPT |
| E70 | Single R9700 v1.0.32 | Restored everyday server on upstream v1.0.32 | KEPT |
| E71 | Prompt speed | Strix v1.0.32 rebase + fused prompt MoE (PR #98) | KEPT-OPEN |
| E72 | Shared decode | Two-row single-GPU speculation on Strix (v1.0.32 rebase) | OPEN |
| E73 | Shared decode | Upstream #96 single-GPU MTP on R9700 | OPEN |
| E74 | Decode speed | Upstream #95 RDNA GEMV on RX 7900 XT | KEPT-OPEN |
| E75 | Profile | R9700 prefill profile: compute-bound, transfers hidden | KEPT |
| E76 | Prompt speed | R9700 GEMM efficiency audit | DROPPED |
| E77 | Prompt speed | Strix prefill disk/tier-stall hypothesis closed | DROPPED |
| E79 | Long context | Strix long-context decode: pinned RAM tier parks borrow-evicted experts | KEPT |
| E80 | Decode speed | Single-R9700 Maya-S config tuneup | KEPT |
| E81 | Decode speed | nimo next everyday: v1.0.32 + fused prompt MoE + PR #96 MTP + RAM_GB=8 | KEPT |
| E83 | Decode speed | R9700 everyday config: RESERVE_MB 1024 -> 512 | ADOPTED |
| E84 | Prompt speed | gfx1151 prompt attention: wave32 WMMA (wmma2) vs f16q | KEPT-OPEN |
| E86 | Prompt speed | nimo everyday: wmma2 prompt attention adopted | ADOPTED |
| E87 | Decode speed | Strix decode excess: ranked table and KDA projection saturation | CLOSED-no-gain |
| E88 | Decode speed | Strix MLA attention split-K retune and hierarchical combine | CLOSED-no-gain |
| E89 | Decode speed | Strix PR #96 MTP config-knob sweep | CLOSED-no-gain |
| E90 | Decode speed | Strix MTP verify-round cost split (C2/T) | MEASURED |
| E91 | Decode speed | Strix #96 verify round: batched MLA `k_b/v_b` + fused union routed MoE | KEPT |
| E92 | Decode speed | nimo everyday: verify-fuse build adopted | ADOPTED |
| E93 | Public-source research | ROCmFPX #37-#52 mapped to Maya GLM-5.3 prefill | LEAD |
| E94 | Prompt speed | Strix prefill ceiling, per-phase excess and three levers | CLOSED-no-gain |
| E95 | Decode speed | Single R9700 PR #103 trunk keep | MEASURED |
| E96 | Prompt speed | Strix ROCmFPX #44 chunk-fastest MMQ activation quant | CLOSED-no-gain |
| E97 | Prompt speed | Strix prompt MoE expert-quant microbench (hypothesis 1h) | CLOSED-no-gain |
| E98 | Quality | Quality bar for approximate speed levers | DESIGN |
| E99 | Two GPUs | Dual R9700 + 7900 XT Maya-S retune on v1.0.33 | MEASURED |

## Prompt speed

### E01 - hipBLASLt tables, sub-batch and chunk size (KEPT)

**Question.** How much of the slow AMD prompt path (~50 tok/s) comes from the 256-token chunk, plain hipBLAS, and the 256-row sub-batch?

**Setup.** AMD Ryzen 7 9800X3D, 192 GB DDR5, RX 7900 XT 20 GB (gfx1100); also Strix Halo 8060S (gfx1151) and R9700 (gfx1201) in the PR table; Maya v1.0.10-1.0.11 based builds, ROCm 7.2.1, hipBLASLt table version 100202, Maya-S IQ2_XXS, 8K context. RX 7900 XT, 8K context, RAM 90 GB. Each step changes one thing: chunk size (256, 512, 1024, engine-sized), then the tuned hipBLASLt table, then sub-batch 1024 with a 4 GB prompt budget.

**Result.** 7900 XT prompt speed, local files (median of 3 unless noted):

| Arm | Prompt tok/s | Prompt size |
|---|---|---|
| chunk 256 | 49.7-50.9 (49.1 at 4K, n=1) | 256-4096 |
| chunk 512 | 83.6 (82.2 at 4K, n=1) | 2K |
| chunk 1024 | 134.3 (131.7 at 4K, n=1) | 2K |
| engine-sized chunk | 198.0 (194.3 at 4K, n=1) | 2K |
| + tuned hipBLASLt table | 259.1 (253.0 at 4K, n=1) | 2K |
| + sub-batch 1024, 4 GB budget | 414.1 | 4K |

PR #7 table (8K context): 7900 XT ~50 -> ~198 -> ~253-259 -> ~413; Strix Halo ~70 -> ~116 -> ~195 -> ~217; R9700 ~490-560 at the last step. Per GEMM, hipBLASLt was 3-7.5x faster than `hipblasGemmEx` on gfx1151 and the 7900 XT, and 1-1.7x on gfx1201.

**Decision.** Merged as #7 (v1.0.11): hipBLASLt tables per architecture, runtime sub-batch, engine-sized chunk.

**Caveats.** Local ladder is on the 7900 XT only and the arm labels are taken from directory names; early arms are single-request at 4K. Strix and R9700 figures are from the PR table only; no raw files found for them. Decode was 12.5-15.3 tok/s across these arms and was not the target.

**Evidence.** [#7](https://github.com/mw00/project-maya/pull/7); local evidence (ram90*, summary.json per arm)

### E02 - rocWMMA prompt attention and FP16 MLA products (KEPT)

**Question.** Does a tensor-core prompt attention and FP16 MLA products speed up prompts on RDNA3/3.5?

**Setup.** Strix Halo 8060S (gfx1151); RX 7900 XT (gfx1100); Maya v1.0.11 vs the #16 branch, ROCm 7.2.x. Same prompts on v1.0.11 and the PR branch. Parity: rocWMMA kernel against the F32 kernel on random data.

**Result.** | | v1.0.11 | #16 |
|---|---|---|
| Strix Halo, ~3.4K tokens, median of 3 | 212 | 247 (+16%) |
| RX 7900 XT, 4K | ~414 | ~432 (+4%) |
| RX 7900 XT, 2K | ~275 | ~276 |

The Strix runs are noisy (one baseline run at 184). A local rerun on the 7900 XT gave 431.6 tok/s at 4K (2 requests) and 275.8 at 2K (1 request). Parity: relative L2 9.8e-5 against the F32 kernel.

**Decision.** Merged as #16 (v1.0.15).

**Caveats.** Strix runs are noisy and n=3. Decode unchanged, not a target.

**Evidence.** [#16](https://github.com/mw00/project-maya/pull/16); local evidence (7900 XT: 431.6 tok/s at 4K, n=2; 275.8 at 2K, n=1)

### E03 - wmma2 prompt attention on RDNA4 (KEPT-OPEN)

**Question.** Is a wave32 WMMA attention kernel faster than #16's f16q on the R9700?

**Setup.** Radeon AI PRO R9700 (gfx1201); Strix Halo for the comparison; Maya v1.0.16 (measurement); PR rebased locally on v1.0.21. Single R9700, v1.0.16, two requests per prompt size, f16q against wmma2.

**Result.** | prompt | f16q (#16) | wmma2 |
|---|---|---|
| ~4K | 680 / 719 | 749 / 801 |
| ~8K | 767 / 764 | 828 / 830 |

On Strix Halo f16q measured 251 tok/s against 237-247 for wmma2 at 4-7K prompts, so gfx11 keeps f16q. The rebase onto v1.0.21 (commit b7b8d75) built `strata` and the tests for gfx1100/1201/1151; GPU parity and INT8 runs are listed as pending.

**Decision.** #38 open. Default wmma2 on gfx12, f16q on gfx11. Maintainer asked for a rebase because of #41's INT8 latent cache; rebased locally on v1.0.21 with an INT8 load path in wmma2, CPU build only (no GPU run yet).

**Caveats.** Two requests per cell. After the rebase the parity numbers have not been re-run on a GPU (build only). Strix comparison range is from the PR text, no raw file found.

**Evidence.** [#38](https://github.com/mw00/project-maya/pull/38); [#41](https://github.com/mw00/project-maya/pull/41); local evidence

### E04 - Fused WMMA prompt MoE on Strix Halo (OPEN)

**Question.** Does a fused WMMA prompt-MoE path speed up prompts on gfx1151, and is its output correct?

**Setup.** Strix Halo 8060S (gfx1151, 128 GB unified); fused branch on v1.0.16 (746ee3f), commit 4b480f7 on exp/fused-moe-gfx1151, ROCm 10.2 build (TheRock 10.2.0a20261009, AMD clang 24), hipBLASLt table gfx1151-glm-hipblaslt-100500. Maya-S v2 IQ2_XXS pack, context 65536, prefill sub-batch 4096, prefill budget 12288 MB. Same binary; only STRATA_GLM_PREFILL_FUSED=1 versus 0 changes. Fresh servers, ON then OFF; prompts of about 4K, 8K and 16K tokens, 32 output tokens, temperature 0, three paired requests per size.

**Result.** Parity is fixed. The gfx11 epilogue lacked the SwiGLU clamp that the gfx12 path has. On the iq2_xxs/q2_0 pair (clamp limit 1.25), fused versus MMQ relative L2 went from 31.05 to 8.7e-8. All 48 fused parity cases pass; fused versus MMQ ranges from 8.7e-8 to 1.0e-4 across them. Prompt A/B, medians of three (fused ON versus MMQ OFF):

| prompt | ON tok/s | OFF tok/s | delta |
|---|---|---|---|
| ~4K | 335.9 | 296.7 | +13.2% |
| ~8K | 340.0 | 316.2 | +7.5% |
| ~16K | 333.7 | 318.5 | +4.8% |

A 556-token prompt with 200-token decode measured 18.1 ON and 18.0 OFF (noise, as expected for a prefill-only change). The earlier run, made before the fix and with parity failing, was one request per cell at +8.5% (4K), +5.8% (8K) and +5.0% (16K); it is not usable as a result.

**Decision.** Fixed and measured on Strix (gfx1151). Rebased to v1.0.26 and freshly validated in E48; at the time there was no code push or PR, but the work was later opened upstream as [#98](https://github.com/mw00/project-maya/pull/98) (see E71). The gfx11 runtime gate admits gfx1151 only. The RX 7900 XT (gfx1100) keeps MMQ; enabling it needs a gfx1100 kernel image, an allowlist entry, and its own parity and A/B runs on that card.

**Caveats.** Three repeats per size, run sequentially, with no confidence interval; prompt sizes differ from the earlier run. Greedy text is not byte-identical between ON and OFF (floating-point and quantization differences). The coherence checks are smoke tests, not a quality evaluation. No gfx1100 or gfx12 hardware run.

**Evidence.** local evidence (earlier run)
### E05 - Strata's fused int8 WMMA prompt MoE on discrete cards (OPEN)

**Question.** Does the port of Strata's fused int8 WMMA MoE help the R9700 or RX 7900 XT?

**Setup.** R9700, RX 7900 XT; not recorded in the files read. Port of Strata's fused int8 WMMA prompt-MoE kernels, RDNA4 path, run on the R9700 and RX 7900 XT.

**Result.** Parity passed; no repeatable end-to-end prompt gain. No numbers survive.

**Decision.** Codex's port of Strata's fused int8 WMMA prompt-MoE (RDNA4 path) passed parity but gave no repeatable end-to-end prompt gain on the R9700 or RX 7900 XT. On Strix (gfx11 path) it gives +5-8.5% but its parity test fails (see E04). Open. The E04 gfx11 parity fix is now measured on Strix (see E04); the discrete-card result has not been re-run.

**Caveats.** Discrete-card result is from the coordinator's notes; no raw numbers or files survived (lost in a reboot). Date is an estimate.

**Evidence.** hub ROADMAP.md, 'Looked at, not worth it'; coordinator session notes, 2026-10-08/09; see E04

### E06 - llama.cpp RDNA4 MMQ patch (DROPPED)

**Question.** Does llama.cpp PR #25940 (RDNA4 MMQ) help Maya's IQ formats?

**Setup.** not recorded; not recorded. llama.cpp PR 25940 applied to Maya's MMQ code for gfx1201.

**Result.** No repeatable benefit. No numbers recorded.

**Decision.** Applied to Maya's MMQ for the R9700: no repeatable benefit for Maya's IQ formats. Dropped.

**Caveats.** No numbers survive (source files lost in a reboot); the result is from the coordinator's notes and the roadmap. Date is an estimate.

**Evidence.** hub ROADMAP.md, 'Looked at, not worth it'; coordinator session notes, 2026-10-08/09; [llama.cpp #25940](https://github.com/ggml-org/llama.cpp/pull/25940)

### E07 - ROCm 10.2 (TheRock nightly) on Strix Halo (KEPT)

**Question.** Does ROCm 10.2 raise prompt speed over ROCm 7.2.2 on Strix Halo?

**Setup.** Strix Halo 8060S (gfx1151), 128 GB unified (nimo); Maya v1.0.16; system ROCm 7.2.2 vs TheRock 10.2.0a20261009 pip wheels in a venv; hipBLASLt 1.5.0, new gfx1151 table version 100500 (72/72 shapes retuned). Same build and prompts on system ROCm 7.2.2 and on the TheRock 10.2 venv, IOMMU on for both.

**Result.** | prompt | ROCm 7.2.2 | ROCm 10.2 | gain |
|---|---|---|---|
| ~4K | 249 | 276 | +10.9% |
| ~8K | 267 | 290 | +8.6% |
| ~16K | 270 | 283 | +4.8% |

Decode 17.8 on both; 6/6 long prompts (8-16K) without errors; completions byte-identical. The 312 / 323 / 325 "everyday" figures are this ROCm 10.2 build with the IOMMU off (E08).

**Decision.** Kept as a local nightly evaluation path (venv, system ROCm untouched); pin the version. Completions were byte-identical to ROCm 7.2.2. The R9700 path is still to do.

**Caveats.** One request per size. Nightly build. The hub's earlier '258-273 tok/s' on 7.2.2 is a different sweep; this entry uses the coordinator's same-prompt comparison.

**Evidence.** hub README.md and NOTES.md; coordinator session notes, 2026-10-08/09; local evidence

### E08 - amd_iommu=off on Strix Halo (KEPT)

**Question.** Does disabling the IOMMU raise Maya's prompt speed on Strix Halo?

**Setup.** Strix Halo 8060S (nimo), 128 GB unified; Maya v1.0.16 on TheRock ROCm 10.2, 64K context, kernel 6.17.0-35-generic. Same build, config and prompts before and after the reboot into amd_iommu=off.

**Result.** | prompt | IOMMU on | IOMMU off | gain |
|---|---|---|---|
| ~4K | 276 | 312 | +13% |
| ~8K | 290 | 323 | +11% |
| ~16K | 283 | 325 | +15% |
| ~30K | - | 325 (TTFT 94 s) | - |

Decode unchanged at 17.2-18.0 (17.8 before). One request per size.

**Decision.** Kept as a local host setting on nimo; not upstream. Trade-off: disables DMA translation machine-wide and the NPU. This reconciles the two Strix figure sets: 276/290/283 = ROCm 10 with IOMMU on; 312/323/325 = ROCm 10 with IOMMU off.

**Caveats.** One request per size; before/after separated by a reboot, so other host state could differ. Raw files for the A/B were lost in a reboot; figures come from the coordinator's notes. Gain here is about +13% (4K), +11% (8K), +15% (16K), computed from the table, in line with the other project's 13-16% claim.

**Evidence.** coordinator session notes, 2026-10-08/09; receipts/everyday-before.json (kernel command line amd_iommu=off, 2026-10-09); hub ROADMAP.md

### E09 - --prefill auto (PR #44) on Strix Halo (OPEN)

**Question.** Does the new automatic chunk sizing match the hand-tuned 32768-token chunks on a unified-memory APU?

**Setup.** Strix Halo 8060S, 128 GB unified, 64K context; upstream v1.0.20 + #41 + #44 merge, ROCm 10.2 build. Everyday server against the merged-PR build, same prompts, back to back.

**Result.** | prompt | 32K chunks / sub 4096 | `--prefill auto` | delta |
|---|---|---|---|
| ~8K | 318.3 | 247.2 | -22% |
| ~16K | 318.7 | 247.5 | -22% |
| ~30K | 323.8 | 245.8 | -24% |
| ~60K | 311.4 | 240.8 | -23% |

Decode and expert pool unchanged (80.17 GB, 288 slots per layer). The maintainer's V100 tables (theirs) on #44 show one GPU wants the large chunks, and `auto` now picks by GPU count.

**Decision.** Reported on #44 (merged as v1.0.21). The comment asked auto to keep configured sub-batch/chunk and size integrated GPUs from system RAM. Whether upstream changed this was not found in the records.

**Caveats.** One request per cell. The PR build also needed a 3-line shim (E32). Maintainer's own #44 tables (V100) show the same direction on one GPU with large chunks.

**Evidence.** [#44](https://github.com/mw00/project-maya/pull/44) (our comment); local evidence

### E10 - Why 256-token prompts are slow (DROPPED)

**Question.** Is short-prompt speed (51-88 tok/s at 256 tokens vs 416-495 at 4K) caused by looping over experts that got no tokens?

**Setup.** AMD Ryzen 7 9800X3D, 192 GB DDR5, RX 7900 XT 20 GB (gfx1100); R9700 + 7900 XT for the 2-GPU numbers; v1.0.11 + local shadow branch (5393fe7). Code reading of the prompt MoE path.

**Result.** No gap found to remove. Prompt speed at 256 tokens stays 51 tok/s (1 GPU) and 88 (2 GPUs).

**Decision.** Suppressed as MG-S005: zero-token experts are not fetched or computed; at 256 tokens nearly every expert gets rows. Remaining cost is system-level (one host sync per MoE layer, staging bytes).

**Caveats.** Source review, no profiler timeline. The per-layer sync cost was not measured.

**Evidence.** local evidence: Maya field-study lead ledger (LEADS.md) / SUPPRESSIONS.md; local evidence

### E11 - Terminal-layer prefix skip in single-GPU prefill (KEPT-OPEN)

**Question.** Is the last layer's attention output projection and FFN wasted work for prefix rows when MTP is not loaded, and can it be skipped without changing output?

**Setup.** Strix Halo 8060S (gfx1151, 128 GB unified), one GPU; upstream v1.0.23 (eadd3b6), ROCm 10.2 build, commit 2de6962 on perf/terminal-layer-skip. Correctness: prompts of 220, 1,122 and 2,682 tokens, context 4096, prefill chunk 512, a fresh model load per comparison with all experts in the GPU pool, 64 greedy steps. Speed: prompts of about 4K, 8K and 16K tokens, context 65536, one greedy output token, one request per size and mode, fresh servers, sub-batch 4096, prefill budget 12288 MB. The skip is on by default; STRATA_GLM_PREFILL_TAIL_SKIP=0 restores the full computation. It is disabled when the trunk is not all on one device, when a NextN block is loaded, and when a seam dump directory is set.

**Result.** Byte-identical with the skip on and off: all 192 greedy token IDs, all 29,736,960 logits, every prefill cache dump, and each saved and restored snapshot, over 21 files per mode (2.13 GB). With a NextN block forced on (control, 220-token prompt, 64 steps), the output, state and snapshot were also byte-identical, which confirms the guard. Prompt speed, one request per size:

| prompt | actual tokens | skip off tok/s | skip on tok/s | gain |
|---|---:|---:|---:|---:|
| ~4K | 4,162 | 302.0 | 305.7 | +1.20% |
| ~8K | 8,315 | 319.5 | 322.9 | +1.06% |
| ~16K | 16,561 | 316.1 | 318.5 | +0.75% |

**Decision.** KEPT-OPEN. The gain is small but in the same direction at all three sizes. A PR is pending an R9700 check. An earlier comparison that reused one loaded model for three prompts differed on prompt 2, first in layer 7's DSA latent cache, before the skipped layer. The fresh-load comparison removed that placement effect without a tolerance.

**Caveats.** One request per size and mode, so these are observations, not an estimate of a stable speedup. Multi-request output is not promised byte-identical when adaptive expert placement is allowed to change. No NVIDIA build or timing: the skip sits in shared host code and so applies to eligible CUDA single-GPU prefill, but its benefit there is unmeasured. Not measured on two GPUs; split exclusion was checked from the guard and both hop consumers.

**Evidence.** local evidence
### E38 - R9700 prefill sub-batch 1024, 2048 and 4096 (DROPPED)

**Question.** Does a larger prefill sub-batch (1024, 2048 or 4096) speed up prompts on one R9700?

**Setup.** R9700 (gfx1201, 32 GB), single GPU, RAM tier 48 GB, PROMOTE_MIN 6, context 40960; integration build (v1.0.23 + #39 + #38) with the gfx1201 hipBLASLt table. Prefill probes: one request each at about 4K, 8K, 16K and 28K prompt tokens, 16 output tokens. Decode probe: 1600 tokens, 256 output, n=2. Sub-batch was the only change.

**Result.** Prompt tok/s, one sample per cell:

| sub-batch | 4K | 8K | 16K | 28K | decode tok/s (n=2) |
|---|---:|---:|---:|---:|---|
| 1024 | 709.4 | 794.5 | 758.2 | 798.9 | 15.5 / 20.6 |
| 2048 | 717.4 | 787.5 | 744.3 | 797.9 | 16.2 / 20.6 |
| 4096 | 728.1 | 686.1 | 763.3 | 757.0 | 16.4 / 20.4 |

**Decision.** DROPPED. 1024 and 2048 are within noise at every size. 4096 reads lower at 8K and 28K. Decode is flat across the three arms.

**Caveats.** One sample per cell. The 4096 dips at 8K and 28K are suggestive, not conclusive. In each decode pair the first value is slower (warm-up).

**Evidence.** local evidence; receipts under local evidence (muse-sub1024, muse-sub2048, muse-sub4096)

## Decode speed

### E12 - Faster decode expert kernels on RDNA3/3.5 (KEPT)

**Question.** Can the routed-expert gate/up and down kernels read weights more efficiently on gfx1100/gfx1151 while staying bit-identical?

**Setup.** RX 7900 XT; R9700 + RX 7900 XT; Strix Halo 8060S; Maya v1.0.15 builds, ROCm 7.2.x. Kernel microbenchmark at the decode shape (8 experts, 4096 x 2048) and full-model A/B with the legacy switch.

**Result.** | kernel (7900 XT) | old | new |
|---|---|---|
| gate/up IQ2_XXS | 93.7 us (369 GB/s) | 66.7 us (519 GB/s) |
| down IQ2_S | 57.8 us (372 GB/s) | 39.9 us (539 GB/s) |
| down IQ3_XXS | 48.3 us (532 GB/s) | 43.5 us (590 GB/s) |

| answers (tok/s) | old | new |
|---|---|---|
| one 7900 XT (median of 4) | 15.1 | 16.4 (+8.6%) |
| R9700 + 7900 XT | 32.4 | 33.6 (+3.9%, noise) |
| Strix Halo (median of 3) | 17.8 | 18.0 (~+1%) |

All parity checks bit-identical.

**Decision.** Merged as #19 (v1.0.15). gfx1100 new kernels; gfx1151 new except the IQ3_XXS down (3% slower there); gfx1201 keeps the old ones.

**Caveats.** Two-GPU +3.9% is within noise per the PR. Strix gain ~+1%; its decode is dominated by dense weights. Small run counts.

**Evidence.** [#19](https://github.com/mw00/project-maya/pull/19)

### E13 - Decode expert kernels for RDNA4 (R9700) (DROPPED)

**Question.** Can the gfx1201 expert kernels be tuned the way #19 did for gfx1100?

**Setup.** R9700 (gfx1201, 32 GB), ROCm 7.2.1 Release build on upstream v1.0.23 (commit f7415b9 on a local branch); single GPU, RAM tier 48 GB, PROMOTE_MIN 6. Selected gfx1201 variants: gate_up<16> direct LDS; down<22> direct LDS with 4 rows; down<18> direct LDS with 2 rows. Only the measured geometry (n_embd 4096, n_ff 2048) and types 16, 22 and 18 changed.

**Result.** Kernel level, bit-identical to the originals (R9700 sweep 620/620 and RX 7900 XT 620/620, max_abs 0):

| kernel | original us/call | selected us/call | speedup |
|---|---:|---:|---:|
| gate_up<16> / down22 | 92.152 | 75.624 | 1.219x |
| down<22> / IQ2_S | 60.428 | 46.821 | 1.291x |
| gate_up<16> / down18 | 91.999 | 75.299 | 1.222x |
| down<18> / IQ3_XXS | 58.853 | 50.005 | 1.177x |

Full-model single-R9700 decode, four measured requests per arm after two warm-ups: optimized 21.98 tok/s mean (21.3 to 22.4) against legacy 21.75 (21.4 to 22.1), about +1.0%.

**Decision.** DROPPED (parked). The per-kernel win does not reach end-to-end decode on the R9700. No push was made.

**Caveats.** The full-model comparison is one pair of four-request arms on one card. The 28.1 tok/s v1.0.16 reference in earlier notes is a measurement artifact (short replies; see E39) and is not used here. gfx1100 dispatch was left unchanged.

**Evidence.** local evidence
### E14 - CPU expert lane plans (two GPUs) (DROPPED)

**Question.** Is the CPU computing part of the RAM-tier experts a net win, and is the automatic split the best?

**Setup.** R9700 (gfx1201, 32 GB, layers 0-23) + RX 7900 XT (gfx1100, 20 GB, layers 24-44 + NextN), Ryzen 7 9800X3D, 192 GB DDR5; v1.0.11 + shadow branch, ROCm 7.2.1, MTP on, RAM_GB 90. Forced CPU plans on the two-GPU split, other settings fixed.

**Result.** | CPU plan | decode median (tok/s) | CPU lane ms/tok | RAM fetches/tok |
|---|---|---|---|
| off | 25.2 | 0 | 62.0 |
| fewer | 32.3 | 7.1 | 35.7 |
| auto | 36.5 | 13.7 | 15.4 |
| more | 32.9 | 20.8 | 2.8 |

The lane's 13.7 ms/token overlaps GPU work and removes PCIe fetches. Baseline counters: 15.3 ms of lane time against a 29.3 ms decode token (35 requests).

**Decision.** Closed: the lane is a net win (about +11 tok/s) and the automatic split is the best of those tried; no change made.

**Caveats.** Four requests per arm. PCIe links were at full speed; the idle-link calibration problem (upstream issue 13) does not show here.

**Evidence.** local evidence: Maya field-study lead ledger (LEADS.md) (MG-L004); local evidence

### E15 - Shared expert on a second stream (Strix) (DROPPED)

**Question.** Does running the shared expert on a side stream help Strix Halo decode?

**Setup.** Strix Halo 8060S; branch amd/hip-shared-stream (7e6b793) on v1.0.x, all experts resident. Shared gate/up/down on a side stream, joined before the routed down; 3 interleaved pairs.

**Result.** Median 17.2 (off) vs 17.9 (on), runs 17.0-18.1. Output byte-identical.

**Decision.** Correct, byte-identical output, but inside noise; not proposed upstream.

**Caveats.** Three pairs only; effect size is smaller than the spread.

**Evidence.** local evidence: Maya field-study lead ledger (LEADS.md); hub ROADMAP.md

### E16 - HIP graphs for decode (DROPPED)

**Question.** Would capturing decode in HIP graphs help Strix Halo?

**Setup.** Strix Halo 8060S; v1.0.15/16. Time breakdown of a Strix decode token (55.9 ms): dense matrix-vector ~50% (already ~91% of memory bandwidth), expert kernels ~27%, attention and small kernels the rest.

**Result.** No experiment; reasoned from the breakdown.

**Decision.** Not pursued: tiny kernels are only ~5% of a Strix token; the time is in reading weights.

**Caveats.** No graph build was tested; the decision rests on the time breakdown. Raw profile files not found.

**Evidence.** hub ROADMAP.md; hub NOTES.md (Where the time goes)

### E17 - Stopping the stale speculative pass early (two GPUs) (DROPPED)

**Question.** After a draft mismatch, can the head GPU's stale pass be aborted to shorten the token?

**Setup.** R9700 (gfx1201, 32 GB, layers 0-23) + RX 7900 XT (gfx1100, 20 GB, layers 24-44 + NextN), Ryzen 7 9800X3D, 192 GB DDR5; v1.0.11 + shadow branch, STRATA_GLM_SPEC_PROF=1. Per-step host laps and per-device pass times from SPEC_PROF.

**Result.** Step 21.4-22.6 ms bound by the tail (22.7 ms/token); head pass ~19 ms with 18-25 ms idle per token.

**Decision.** LOW_LEVERAGE: the stale head pass runs in the head's idle time; the tail sets the pace, so aborting cannot shorten the token. Revisit only if the split is rebalanced so the head becomes critical.

**Caveats.** Five requests; no early-abort code was written, the decision is from the profile. The head is busy about half the time; a different split would change this.

**Evidence.** local evidence: Maya field-study lead ledger (LEADS.md) (MG-L001, MG-S002); local evidence

### E18 - Single-GPU speculation on Strix Halo (OPEN)

**Question.** Would MTP speculation on one card raise decode? Measure its acceptance and draft cost first.

**Setup.** Strix Halo 8060S (gfx1151, 128 GB unified), one GPU; upstream v1.0.23 (eadd3b6) with an opt-in probe (STRATA_GLM_MTP_PROBE=1; commit 718eb10 on exp/mtp-accept-1gpu), ROCm 10.2 build. Four greedy 300-token continuations (chat, code, reasoning, prose) with probe on and off; context 65536; NextN loaded; a fresh engine for each run.

**Result.** Greedy acceptance of the NextN draft: 900 of 1,196 comparisons, 75.25% overall (chat 72.2%, code 78.3%, reasoning 86.6%, prose 63.9%). Draft 3.82 ms against a normal decode step of 54.38 ms (draft overhead 7.0%). All 1,200 probe token IDs match the probe-off run. The two-token verify has not been built or timed. Cost model: if a two-token verify costs 1.2x a normal step, the estimated speedup is 1.38x. Break-even for the verify is 1.68x a normal step (about 91.5 ms). Two serial forwards would give about 0.85x.

**Decision.** OPEN. The shared-weight two-row prototype now exists; E47 records
its observed speed and verified token streams, but also borderline timing
gates, different ON/OFF binaries and incomplete final-test evidence. No default switch.

**Caveats.** Four prompts, one 300-token continuation each: a feasibility sample, not a workload estimate. The 1.2x verify cost is an assumption, based on a read-sharing hypothesis (about 10.2 GB versus 8.5 GB per step), not a measurement. Long-context acceptance is not measured. The earlier 1.1-1.2x estimate remains unverified.

**Evidence.** local evidence; hub ROADMAP.md; local evidence (section 3)
### E39 - Single-R9700 decode regression on upstream builds (DROPPED)

**Question.** Does single-R9700 decode fall from 28.1 tok/s on v1.0.16 to about 20.5 to 22 tok/s on later upstream builds? (Answer: no; the 28.1 was a short-reply artifact.)

**Setup.** R9700 (gfx1201, 32 GB), single GPU, RAM tier 48 GB, PROMOTE_MIN 6. Reference: v1.0.16 (746ee3f), 28.1 tok/s (E22, superseded: a short-reply artifact). Later builds: v1.0.23 integration (+ #39 + #38); #38 alone on b7b8d75; the legacy-kernel arm on the v1.0.23 base; and the sub-batch probe (E38).

**Result.** Single-R9700 decode, tok/s, 256-token replies, five distinct greedy requests per arm (two warm-ups discarded, three scored), 1,600-token prompts, PROMOTE_MIN 6:

| build / setting | context | decode tok/s (mean) |
|---|---|---:|
| v1.0.16, RAM 48 | 8K | 21.23 |
| v1.0.21 + #38, RAM 48 | 8K | 21.47 |
| v1.0.23 integration (+ #39 + #38), RAM 48 | 8K | 21.47 |
| v1.0.16, RAM 48 | 40K | 21.20 |
| v1.0.23 integration, RAM 48 | 40K | 21.43 |
| v1.0.23 integration, RAM 48, `STRATA_GLM_RESERVE_MB=1024` | 40K | 22.57 |
| v1.0.23 integration, `STRATA_GLM_RAM_GB=90` | 40K | 24.47 |
| v1.0.23 integration, RAM 90, reserve 1024 | 40K | 24.97 |
| v1.0.23 integration, RAM 48, 32-token replies (control) | 8K | 24.60 |

Long prompts are unaffected: both reserve-1024 configurations pass a 28,280-token prefill check at 799.0 and 800.7 tok/s.

The earlier 28.1 (v1.0.16) and about 22 (integration) figures in this entry are superseded. The 28.1 run's replies averaged about 26 tokens, and the 32-token control shows short replies decode faster. Other arms recorded here (#38 alone 21.8, legacy kernels 21.75, the E38 probe) are in the same 21-22 range and are kept as recorded.

**Decision.** DROPPED (2026-10-09). No regression between v1.0.16 and v1.0.23 on controlled runs; the gap was a measurement artifact. No engine patch is justified. Settings tip for one R9700: `STRATA_GLM_RESERVE_MB=1024` (82 instead of 75 expert slots per layer, +5.3%) and `STRATA_GLM_RAM_GB=90` (+14.2%); together +16.5% (25.0 tok/s). The reserve does cost about 5% decode (the suspect in the earlier notes), a tuning effect rather than a regression.

**Caveats.** Three scored requests per arm; the RAM 90 and combined arms are single configurations. The 90 GB reference in E22 (25.5) was not re-run; the controlled RAM 90 arm gives 24.5.

**Evidence.** local evidence; E22 (local evidence)

## Expert tiers

### E19 - RAM shadows (STRATA_GLM_RAM_SHADOW) (KEPT-OPEN)

**Question.** Does keeping a RAM copy of promoted experts cut SSD rereads and raise decode?

**Setup.** RX 7900 XT alone; and R9700 + RX 7900 XT; v1.0.11 + shadow branch (76af2e5), ROCm 7.2.1, RAM_GB 90, SUB 1024, MB 4096. Same build, same config, only the shadow flag differs; Codex's cases.json, 2 warm-ups per arm, then 10 cases, greedy, 256 output tokens.

**Result.** | (v1.0.11) | exclusive | shadows |
|---|---|---|
| 1 GPU decode, median | 15.15 (14.4-15.7) | 18.5 (17.8-19.5) |
| 1 GPU SSD per request | 13.2 GB | 4.6 GB |
| 1 GPU disk reads/token | 5.55 | 0.87 |
| 4K prompt | 414 | 416 |
| 2 GPU decode, median | 33.2 (30.2-35.3) | 35.6 (34.0-39.3) |
| 2 GPU disk reads/token | 2.26 | 1.71 |

Stability (shadow, v1.0.11): 12/12 benchmark requests, 7/7 smoke checks, 20/20 stress rounds on one card, 39/39 on two. Earlier v1.0.10 validation: decode median 17.4 (4 stable runs 17.6) vs exclusive 13.66.

**Decision.** #15 is open and on hold: on v1.0.14 with background promotions it crashed (illegal memory access) on the 2nd-4th request. Not in v1.0.15. Gain looks real but is unproven safe; see E30.

**Caveats.** Greedy text diverges between arms (CPU-lane numerics), so quality was compared by coherence only. The v1.0.14 re-measure ended in crashes, so no clean post-promotion A/B exists. The v1.0.10 validation had one 4.5 tok/s outlier (transient CPU-lane slowdown, not reproduced).

**Evidence.** [#15](https://github.com/mw00/project-maya/pull/15); local evidence

### E20 - RAM-resident tier and PROMOTE_MIN on one R9700 (KEPT)

**Question.** Do the community tier PRs (#24 PCIe wake, #25 RAM-resident, #26 PROMOTE_MIN) raise decode on the R9700?

**Setup.** R9700, 192 GB RAM; Maya v1.0.14, RAM_GB 90. Cumulative arms on one card, 4 requests each.

**Result.** 23.4 -> 24.5 -> 25.1 (zero disk reads) -> 25.55 tok/s (+9% overall).

**Decision.** Measurement reported to the maintainer; the PRs were merged by their authors in v1.0.15. No crashes in these runs.

**Caveats.** Four requests per arm. The same RAM-resident mode later crashed on long prompts (E33). The source is our comment on #15; no raw files.

**Evidence.** [#15](https://github.com/mw00/project-maya/pull/15) (our comment, 2026-10-08)

### E21 - Lend budget (STRATA_GLM_PREFILL_MB) 4096 vs 1024 (DROPPED)

**Question.** Do experts dropped when a prompt borrows the pool tail cost decode time through SSD rereads?

**Setup.** R9700 (gfx1201, 32 GB, layers 0-23) + RX 7900 XT (gfx1100, 20 GB, layers 24-44 + NextN), Ryzen 7 9800X3D, 192 GB DDR5; v1.0.11 + shadow branch. Two-GPU arm with the lend budget cut to 1024 MB vs the 4096 default.

**Result.** SSD per request 7.7-9.6 GB -> 3.8-6.8 GB; disk reads/token at 4K 1.84-1.88 -> 1.13-1.23; prefill 492-495 -> 467 at 4K, +3.5% at 2K; decode 34-37 -> 33-35.

**Decision.** Mechanism confirmed (about 60% fewer lend-dropped rereads), no decode gain, prefill -5% at 1K and 4K. Keep 4096.

**Caveats.** Five requests per arm; the RAM tier absorbs the rereads on this 192 GB machine, so SSD-bound setups may differ.

**Evidence.** local evidence: Maya field-study lead ledger (LEADS.md) (MG-L003); local evidence

### E22 - Low-RAM decode (48 GB) on one R9700 (OPEN)

**Question.** How fast is decode when the RAM tier is only 48 GB?

**Setup.** R9700 (HIP GPU 1), 32 GB; Maya v1.0.16 (746ee3f), context 8192, 48 GB RAM. Single R9700, 48 GB RAM tier, 256-token answers.

**Result.** Decode 28.1 / 27.7 tok/s (combined 28.1), VRAM hit 81%, RAM 16.2 per token, disk 1.4 reads per token; the report's 90 GB reference is 25.5.

**Superseded (2026-10-09).** The 28.1 figure is a measurement artifact. The replies averaged about 26 tokens, not the 256 the setup states, and short replies stay on VRAM-resident experts. Controlled 256-token replies on the same build measure 21.2-21.5 tok/s with 48 GB RAM (see E39). The VRAM, RAM and disk figures above come from the same short-reply run and are kept as recorded.

**Decision.** Recorded as a local result only; recommended single-R9700 settings wait for the crash fix. The first attempt crashed once at 1.5K tokens (illegal memory access); the two scored passes were clean.

**Caveats.** Crash on the first attempt, so stability at 48 GB is not established. The 90 GB reference (25.5) and the 3090 references (17.3 / 12.9) are quoted in the report without their own source. Two passes only.

**Evidence.** local evidence

## Two GPUs

### E23 - Two-GPU layer split with MTP drafting (KEPT)

**Question.** Does a second card (layer split, MTP draft on the second card) raise decode and prompts?

**Setup.** R9700 (gfx1201, 32 GB, layers 0-23) + RX 7900 XT (gfx1100, 20 GB, layers 24-44 + NextN), Ryzen 7 9800X3D, 192 GB DDR5; Maya v1.0.11 base + #14, ROCm 7.2.1, RAM_GB 90, 8K context. R9700 first (larger half), 7900 XT second; greedy, 256-token answers.

**Result.** Answers ~15 -> ~33 tok/s (drafts accepted ~76%); 4K prompt ~414 -> ~490 tok/s; 39 back-to-back requests (1.8-4K-token prompts) without error.

**Decision.** Merged as #14 (v1.0.14).

**Caveats.** Hub also quotes ~34-36 tok/s from a v1.0.15 local run (not reconciled with 33 from the PR). One machine, one GPU pair.

**Evidence.** [#14](https://github.com/mw00/project-maya/pull/14); local evidence

### E24 - Split-point sweep and the automatic split (DROPPED)

**Question.** Does moving layers between the two cards raise decode?

**Setup.** R9700 (gfx1201, 32 GB, layers 0-23) + RX 7900 XT (gfx1100, 20 GB, layers 24-44 + NextN), Ryzen 7 9800X3D, 192 GB DDR5; v1.0.11 + shadow branch. Three splits, same requests.

**Result.** | Split | Decode median | Prefill median | Slots/layer GPU0 / GPU1 |
|---|---|---|---|
| 24 | 35.4 | 374.6 | 178 / 88 |
| 26 | 35.9 | 384.1 | 161 / 98 |
| 28 | 34.4 | 391.8 | 146 / 111 |

**Decision.** Default 24 stays: total VRAM for experts is the binding constraint (hit rate flat at 84-85%). The newer split search (#44) was only checked on V100s by the maintainer; the AMD two-GPU run with it is queued and has not been run.

**Caveats.** Differences of 0.5-1.5 tok/s are inside run-to-run spread. Prefill +2.5-4.6% at higher splits is from five requests.

**Evidence.** [#14](https://github.com/mw00/project-maya/pull/14); local evidence: Maya field-study lead ledger (LEADS.md); local evidence

### E25 - One hipBLASLt table per architecture on a mixed pair (FIXED)

**Question.** Does a single tuning table leave one card of a mixed split on plain hipBLAS?

**Setup.** R9700 (gfx1201, 32 GB, layers 0-23) + RX 7900 XT (gfx1100, 20 GB, layers 24-44 + NextN), Ryzen 7 9800X3D, 192 GB DDR5; v1.0.11 + shadow branch. Run loaded only the gfx1201 table.

**Result.** CUDA0 (gfx1201) accepted its 40 rows; the gfx1100 card fell back to hipBLASEx. A per-architecture override was written, then #14 shipped the list form.

**Decision.** Found that the 7900 XT half printed 'tuning architecture mismatch' and used hipBLASEx; fixed by a ':'-separated list, one table per architecture (merged in #14).

**Caveats.** No before/after speed number was recorded for the mismatch.

**Evidence.** [#14](https://github.com/mw00/project-maya/pull/14); local evidence

## Strix Halo

### E26 - Unified-memory expert sizing (KEPT)

**Question.** Can the engine size the expert pool on an APU without hand-set pool and RAM sizes?

**Setup.** Strix Halo 8060S, 128 GB unified; Maya v1.0.15 branch, ROCm 7.2.2. Startup log checks plus three prompts; explicit POOL_GB/RAM_GB caps checked (31.7 GB pool, 4 GB RAM tier).

**Result.** Pool 80.17 GB, 288 slots per layer (12,096 total), RAM tier staging only (0.22 GB, was 1.39 GB). Prompts 213-217 tok/s, answers 16.4-16.6 tok/s, 99.9% pool hits, no errors.

**Decision.** Merged as #17 (v1.0.15). Same speed as the hand-tuned config (~212 / ~16.8).

**Caveats.** Three prompts only.

**Evidence.** [#17](https://github.com/mw00/project-maya/pull/17)

### E27 - Host state on the Strix box (GTT use, IOMMU, background services) (FIXED)

**Question.** Did the machine's state change what the Strix runs measured?

**Setup.** Strix Halo 8060S, 128 GB unified, 4 GB VRAM carve-out; kernel 6.17.0-35-generic, ROCm 7.2.2. receipt.sh before and after each measurement.

**Result.** Hogged: pool 17 GB / 60 slots per layer, decode ~5.3 tok/s. Clean: 80.17 GB / 288 slots, decode ~17-18. Idle receipt: GTT used 18.6 MB of 120.26 GB; server running: 95.0-95.64 GB. nimo's command line had amd_iommu=off in all four receipts. Earlier Strix numbers (before 2026-10-08 evening) came from a non-MMQ build (STRATA_PREFILL_MMQ=OFF in an old cache).

**Decision.** After a reboot, vLLM (user service ciru.service plus an ornith launcher) held GPU memory: GTT used 104.6 of 120 GB. Maya sized its pool from MemAvailable and got 17 GB / 60 slots per layer instead of 80.17 GB / 288; decode collapsed to ~5.3 tok/s (disk-bound). After stopping ciru/opencode/lemond: GTT 27.5 GB, MemAvailable ~90 GB, pool back to 80.17 GB / 288 slots. Check GPU-memory owners before measuring; this is now a receipt step.

**Caveats.** Hog figures are from the coordinator's notes; the logs were lost in a reboot. Receipts only cover 2026-10-09; earlier Strix runs have no host capture.

**Evidence.** coordinator session notes, 2026-10-08/09; local evidence

## Stability

### E28 - HIP doorbell publication and KDA test initialisation (FIXED)

**Question.** Why can AMD host polling stall on the GPU-to-CPU handoff?

**Setup.** RX 7900 XT; R9700 (isolated handoff binaries); Maya 1.3.0 (70e0746), ROCm 7.2.1 / clang 22. ctest on hip_handoff, hip_glm_handoff, glm_kda_parity.

**Result.** 3/3 passed (0.61 s total). The handoff tests also pass as isolated gfx1201 binaries.

**Decision.** Merged as #2: system-scope fence after each signal store (HIP only), volatile ring store, new hip_glm_handoff test, memset of the KDA fixture state.

**Caveats.** Full-engine validation was on gfx1100 only; no throughput claim; CUDA not compile-checked.

**Evidence.** [#2](https://github.com/mw00/project-maya/pull/2)

### E29 - First attribution of the memory fault: interference (wrong) (FIXED)

**Question.** What caused the intermittent 'illegal memory access' seen with RAM shadows and on two GPUs?

**Setup.** R9700 (gfx1201, 32 GB, layers 0-23) + RX 7900 XT (gfx1100, 20 GB, layers 24-44 + NextN), Ryzen 7 9800X3D, 192 GB DDR5; RX 7900 XT alone; v1.0.11 two-GPU; v1.0.14 single card. Sequence: two-GPU fault on v1.0.11 blamed on interference; later a single-card reproduction on v1.0.14 with shadows; bisect with STRATA_GLM_PROMOTE=0 announced.

**Result.** Wrong first conclusion, corrected publicly on #15. Pre-crash shadow speed was ~19-19.5 tok/s against 15.35 exclusive.

**Decision.** The attribution to another process using the GPUs was withdrawn. The fault reproduced on one card on v1.0.14 with shadows, always in the prefill of a new request after a conversation was set aside. Root causes: E30.

**Caveats.** 'It went away for 80 rounds' was treated as evidence of a cause; it was not. The two-GPU fault note (engine log line 896) points at the conversation-slot transition in the MTP path; it was not isolated with STRATA_GLM_SLOTS=0 in the records read. The fix report did not rerun the historical crashes to say which race caused each.

**Evidence.** [#15](https://github.com/mw00/project-maya/pull/15) (our comment, 2026-10-08); local evidence; hub NOTES.md ('A correction')

### E30 - Root causes of the illegal memory access, and the fix (#39) (KEPT-OPEN)

**Question.** What actually caused the crashes on long prompts and RAM-heavy modes, and does the fix hold?

**Setup.** RX 7900 XT and R9700, 192 GB RAM; v1.0.15/16 base 746ee3f, fix commit 251c113, ROCm 7.2.1, gfx1100;gfx1201;gfx1151. Fix: flush waits for the compute stream before pinned reuse; lending holds moves, retires metadata, checks none remain; routing validates IDs and scores and fails the request cleanly. Reproducers on RAM_GB 90 with resident + PROMOTE_MIN 6 for long prompts.

**Result.** | Setup | Before | After |
|---|---|---|
| 7900 XT, resident + PROMOTE_MIN 6, ~1.6K prompts | crash by request 2 | 12/12 ok, 346.9 tok/s prefill, 19.1 decode |
| 7900 XT, default exclusive tiers | occasional | 12/12 ok, 345.3 / 18.2 |
| 7900 XT, resident, all-CPU plan | - | 3/3 ok (3,371 promotions, 2,603 demotions) |
| R9700, resident, ~8K / ~16K / ~28K | 7/13 crashed above 7K, 0/3 at 28K | 9/9 ok, 759.0 / 767.9 / 772.6 prefill |

Invalid-route injection (all and partial NaN/Inf on primary, predicted, lookahead and prompt routes) failed the request with a clear error and the engine recovered.

**Decision.** Three defects confirmed by control flow: (1) pinned table-update buffers rewritten while the async copy was pending; (2) background moves racing prompt lending in resident mode; (3) INT_MAX top-k index with NaN scores. Fix is #39 (open); needs a rebase onto #44/#31, done locally (6b6d520) but not run on GPU.

**Caveats.** The run did not isolate which race caused each historical crash, nor trace the origin of every NaN; the 'before' counts were supplied, not rerun. Medians include the first request; long-prompt decode is 32 tokens. gfx1151 compiled, not hardware-tested; CUDA not built or run (shared code is exposed too). Default calibration used mixed CPU/GPU lanes and started no background moves; the all-CPU plan was needed to exercise race 2.

**Evidence.** [#39](https://github.com/mw00/project-maya/pull/39); local evidence (re-verified on v1.0.23 + #39 + #38, incl. R9700 long-prompt repeats)

### E31 - Rebasing #38 and #39 onto v1.0.21 and v1.0.23 (KEPT-OPEN)

**Question.** Do the two open fixes still apply after #31, #41 and #44 merged, and do they work on GPU once rebased?

**Setup.** First, a CPU build on upstream v1.0.21 (89a1336), with the three HIP device-query mappings cherry-picked first, then 251c113 and cbfb03e reapplied. Then an integration build on upstream v1.0.23 (eadd3b6) with #39 (6b6d520, merged locally as 4e180a4) and #38 (b7b8d75, as a03b840), all five targets, 147/147 steps. GPU checks on the RX 7900 XT (gfx1100) and R9700 (gfx1201), KV FP16 and INT8; dual-GPU arm at layer split 24.

**Result.** The v1.0.21 build succeeded with warnings only. The v1.0.23 integration build succeeded (warnings only). Parity 16/16 pass across both GPUs and both KV types: model error 2.56e-6 to 3.10e-6; wmma2 versus F32 3.4e-7 (gfx1100) and 1.6e-7 (gfx1201). Reproducers all pass: 7900 XT resident 12/12 (about 347 tok/s prefill, about 19 decode); R9700 long prompts 9/9 FP16 (8K about 796, 16K about 808, 28K about 814 tok/s prefill) and 9/9 INT8 (about 785, 799, 806). Dual GPU at split 24: 690 and 756 tok/s prefill at 4K and 8K (about 490 before), decode 34 to 40 tok/s. Against #38 alone on the same protocol, the integration is within 1% on both single-GPU arms.

**Decision.** Both fixes apply after the rebase and pass the reproducers on GPU. #38 and #39 stay open upstream. The rebased PR branches were pushed 2026-10-09 and await a CUDA check.

**Caveats.** CUDA was not built or run for this rebase. The earlier R9700 baselines (800/830 prefill, 28.1 decode) do not reproduce under this protocol even for #38 alone, so they are a stale baseline, not a regression. The 28.1 decode was a short-reply artifact (see E39). The R9700 decode level is a separate open question (see E39). Single requests per long-prompt size.

**Evidence.** local evidence; [#38](https://github.com/mw00/project-maya/pull/38); [#39](https://github.com/mw00/project-maya/pull/39)
### E32 - #44 breaks the HIP build (#52) (FIXED)

**Question.** Does upstream v1.0.20/21 with #41 + #44 build and run on AMD?

**Setup.** Strix Halo 8060S; v1.0.20 + #41 + #44, ROCm 10.2; v1.0.21 + fix on ROCm 7.2.1 for gfx1100;gfx1201;gfx1151. Octopus merge of #41 and #44 on v1.0.20 on nimo.

**Result.** Build failed until the 3-line shim; then glm_layer_parity, glm_model_test and hip_glm_prefill_attention passed.

**Decision.** Three mappings were missing (cudaDeviceGetPCIBusId, cudaDevAttrMemoryClockRate, cudaDevAttrGlobalMemoryBusWidth). Reported on #44 and fixed by #52 (merged 2026-10-09).

**Caveats.** The report on #44 arrived after the merge.

**Evidence.** [#52](https://github.com/mw00/project-maya/pull/52); [#44](https://github.com/mw00/project-maya/pull/44) (our comment); local evidence

## Long context

### E33 - Long prompts on one R9700, before the fix (FIXED)

**Question.** What does a typical user get with long prompts on one R9700?

**Setup.** R9700 (gfx1201, 32 GB), 90 GB RAM-resident; Maya v1.0.16 (746ee3f), context 40960. Prompts of ~7K, ~14K, ~20K, ~28K tokens, 16 output tokens.

**Result.** Successful runs 749-781 tok/s at 8-20K. 32K (~27.8K tokens): 0/3 completed.

**Decision.** Speed was fine when it completed; crashes were E30's bug. Any prefill error wedged the engine until restart.

**Caveats.** One or two requests per size. The RTX 3090 reference is quoted in the report without a source. One of the 32K failures was a planner reject ('planned 1792 of 222488 routed rows'), the others illegal memory access.

**Evidence.** local evidence

### E34 - KV ring (#41) and 64K context on Strix Halo (KEPT)

**Question.** Does the indexer-key ring and 64K context work on AMD, and what does it save?

**Setup.** Strix Halo 8060S, 128 GB unified; v1.0.20 + #41 + #44 (+ shim), TheRock ROCm 10.2, 64K context, Maya-S. Merged-PR build vs everyday server on nimo.

**Result.** State/KV at 64K: 1.60 -> 1.00 GB. No illegal/planned/error messages in either engine log. Maintainer's measurements on V100 (theirs): 3.06 -> 1.78 -> 1.13 GB at 128K, decode 18.1 -> 19.3 -> 19.6 tok/s.

**Decision.** #41 works on Strix (merged in v1.0.21 by upstream). The pool is unchanged here only because all experts already fit; on a 32 GB card the 0.6 GB goes to experts (the R9700 long-prompt repeat on #41/#44 is queued, not run).

**Caveats.** INT8 latent cache was not run. Single pass; R9700 long-prompt repeat not found in the records.

**Evidence.** [#41](https://github.com/mw00/project-maya/pull/41) (our comment); local evidence

### E40 - R9700 long-context ladder, 40K to 1M (KEPT)

**Question.** What is the largest context window on one R9700 that keeps speed, for everyday use, evals and agentic coding?

**Setup.** Single R9700 (gfx1201, 32 GB), integration build (v1.0.23 + #39 + #38), ROCm 7.2.1, Maya-S IQ2_XXS, RAM_GB 90, RESERVE_MB 1024, PROMOTE_MIN 6, SUB 2048, identical usage-profile seed per arm. Model maximum context is 1048576. Arms: 40960, 65536, 131072 with FP16 latents, 131072 with INT8 latents, 1048576 with INT8.

**Result.** One prompt/TTFT request per size; decode is 256-token replies after a ~1.6K prompt (last 3 of 5, all replies verified at 256 tokens), plus one decode after the longest prompt. No illegal/error in any engine log.

| Arm | KV/state GiB | Slots/layer | Decode tok/s (mean of 3) | Prompt tok/s at ~8K / ~31K / ~63K / ~119K |
|---|---|---:|---:|---|
| 40960 FP16 | 0.71 | 82 | 25.33 | 795 / 788 / - / - |
| 65536 FP16 | 1.00 | 81 | 25.20 (-0.5%) | 792 / 790 / 779 / - |
| 131072 FP16 | 1.78 | 78 | 24.83 (-2.0%) | 789 / 790 / 778 / 749 |
| 131072 INT8 | 1.13 | 80 | 25.10 (-0.9%) | 782 / 784 / 772 / 743 |
| 1048576 INT8 | 7.45 | 58 | 22.23 (-12.2%) | 503 / 546 / 568 / 572 |

**Decision.** KEPT: 131072 with INT8 latents is the everyday config (`maya-r9700-long-7900.json`). Each step up from 40960: 65536 costs ~nothing; 131072 FP16 costs 4 slots/layer and ~2% decode; 131072 INT8 costs 2 slots/layer and ~1% decode — the best 128K arm. The 1M maximum fits (58 slots/layer) but fails both bars: decode -12%, prompt -31 to -37%, prefill chunks collapse to 1536.

**Caveats.** Single prompt request per size. RAM_GB 90 needs ~105-111 GB MemAvailable (the box has 186 GB).

**Evidence.** local evidence (logs, configs, receipts, harness)

### E41 - Strix long-context ladder (KEPT)

**Question.** Can nimo's everyday context rise to 128K+ while keeping speed, on current upstream plus our speedups?

**Setup.** Strix Halo 8060S (gfx1151), 128 GB unified, TheRock ROCm 10.2; new build `maya-next` (v1.0.24 + prompt-tail skip + gfx11 fused MoE fix), `--prefill 32768` explicit. Baseline: everyday v1.0.16 64K server.

**Result.** Final 262144 FP16 arm: 342.6/337.4/326.5/314.0 prompt tok/s at ~8K/~32K/~64K/~120K. New 64K arm: 347.9/340.2/324.2 at the first three lengths, so the larger reservation changes speed by -1.5%/-0.8%/+0.7%. At the two measured old-v1.0.16 baseline lengths, gains are +6.1% at 8K and +4.2% at 32K. Short-prompt decode 17.0 vs 17.1 tok/s, falling to 15.5 after ~120K; the five 262K-arm throughput replies reached 256 tokens. Muse reports 12/12 single-needle + 4/4 multi-hop hits through ~240K; preserved logs confirm requests through 239644 prompt tokens.

**Decision.** KEPT: `maya-nimo-256k.json`, 262144 context, FP16 latents, explicit prefill 32768, sub-batch 4096. All 12096 expert slots still fit (80.17 GB pool), with state/KV 3.32 GB; INT8 saves memory but has no capacity benefit on this box and costs about 2% prompt speed.

**Caveats.** One request per cell and one recall pass, no confidence interval. Reply texts were not preserved for independent recall rescoring. Capacity is not continuous residency: the raw 262K log reports 11260/12096 experts resident after the ~120K prompt and 1.26 disk reads/token during that decode, because prompt lending evicts experts. Reserving a larger window is distinct from filling it. The everyday config is temporarily replaced during the two-row job, which must restore it.

**Evidence.** local evidence (long-context report, final and audited), before/after receipts, and preserved config/engine/server logs.

## Quality

### E35 - Quality of Maya on AMD (OPEN)

**Question.** Does the AMD path change output quality compared with NVIDIA?

**Setup.** Single R9700, ROCm 7.2.1, Maya v1.0.23 integration + #38/#39, Maya-S, 131072 context, INT8 latents, RAM 90 GB, reserve 1024 MB. Seed 1234 sampled short tasks, exact prompt preflights and before/after receipts. Earlier AMD kernel parity tests remain separate evidence.

**Result.** 183 cases completed: GSM8K 37/40, MMLU-Pro 48/56, HumanEval 30/30, IFEval prompt strict 29/40 / loose 35/40 (instruction strict 82.5%), tools 11/12, five writing outputs reviewed. Zero request errors, truncations or checker errors. HumanEval corrected from 29/30 after restoring a supplied prompt helper; all saved answers rescored, originals preserved. Writing coherent but some exact constraints missed. Earlier parity: rocWMMA vs F32 rel L2 9.8e-5; wmma2 vs F32 1.6e-7 to 3.4e-7; #19 kernels bit-identical.

**Decision.** AMD short-task baseline measured. OPEN for the original AMD-versus-NVIDIA question: no matched NVIDIA control, AMD perplexity/KL, or controlled long-context precision ablation. This sample does not explain E44's digit garbling.

**Caveats.** Small sampled suites, not full benchmark scores. MMLU samples four per category. Writing has no aggregate score; variable reply lengths are not throughput data. The KL numbers are the maintainer's on V100, not ours. Greedy text differs between AMD runs because expert placement changes rounding; compare by coherence/acceptance unless tiers are pinned. A third party (sociolog, 3090 + 3060) reported a pinned bit-identical KL check; not run by us.

**Evidence.** [QUALITY.md](QUALITY.md); local eval/results-r9700-128k-20261009/ (raw answers, grading audit and receipts); results/codex-quality-r9700-128k.md. [#41](https://github.com/mw00/project-maya/pull/41) (maintainer comment, V100); PR descriptions #16, #19, #38

### E42 - Tool-calling 400s were a stale frontend (FIXED)

**Question.** Why does every OpenAI `tools` request fail with HTTP 400 "malformed tool call"?

**Setup.** Single R9700, Maya-S; 12 tool cases in local evidence run against serve/ from the old `amd/glm-gfx1100` checkout (v1.0.15-era) vs serve/ from upstream main (v1.0.26).

**Result.** Not a parser bug in upstream: the old serve/ only parsed Qwen's `<function=...>` form, so every GLM `NAME<arg_key>..` body was rejected. Stale serve/: 2/12; current serve/: 11/12 (the one miss is the model also calling web_search, not a parse error). Exact raw body captured and pinned in a unit test (`serve/test_tools.py`, 23 tests pass).

**Decision.** FIXED: the server launcher (`start7900.sh`) now runs serve/ from each config's own engine tree instead of the stale checkout, so frontend and engine always match.

**Caveats.** The 11/12 is one 12-case run. The old checkout's serve/ itself was left untouched (it is the user's branch).

**Evidence.** local evidence

### E44 - NIAH recall garbles exact digits (OPEN)

**Question.** Does long-context recall stay exact at 32-122K on the ~2-bit Maya-S quant?

**Setup.** Single R9700, 131072 INT8 arm of E40; needle-in-a-haystack with real varied filler (402 repo sources), 3 needles at ~10/50/90% depth, temperature 0, plus one multi-hop sum question; one run each at ~33K/~65K/~122K.

**Result.** 2/4 at 32.9K, 1/4 at 65.0K, 2/4 at 121.7K; the multi-hop sum is 3/3 with correct addends each time. Misses are noisy, not length-driven: digit transpositions (4817->4481, KQ-2291->KQ-2219) come and go across lengths, and one needle misreads 06:40 as 0600 at all three lengths with no confounder in the filler.

**Decision.** OPEN: quant precision is a hypothesis, not an attribution. The completed E35 short-task baseline does not test long-context digit recall or isolate quant versus KV precision.

**Caveats.** Single run per length; one reply rescored HIT->MISS on a substring-scorer false positive ("44817" contains "4481" but the needle was 4817).

**Evidence.** local evidence

## Comparisons with other projects

### E36 - OpenMOSE Strata-GLM-AMD (their numbers) (OPEN)

**Question.** Which of another project's techniques and results apply to Maya on R9700 and Strix Halo?

**Setup.** Theirs: 2x Radeon Pro W7900 (gfx1100, 48 GB), Ryzen 7 5700X3D, 125 GB DDR4; nothing run on RDNA4 or gfx1151; Strata-GLM-AMD docs/GLM5_NEXT.md, GSQ-RCO 3.0-bit GGUF (not Maya's pack). Read-only review by an agent.

**Result.** Examples (theirs): window 4096 raised 4K prefill 531 -> 764 tok/s; CPU-computed decode misses 14.6 -> 28.4 tok/s; their lookahead prefetch was slower (30.7 tok/s with none vs 27.6 with cap 3).

**Decision.** A ranked list of techniques was extracted. Priorities noted: single-GPU MTP on Strix first; window size 4096 with chunk 2048 and admission/exchange swap on the R9700; windowed DSA only if our own 16-28K curve falls off. Nothing implemented.

**Caveats.** All numbers are theirs, on different hardware, model quant and engine; not comparable to Maya's. The report marks several points UNVERIFIED (e.g. single-card acceptance, RDNA4 builtin variant). Includes their negative results (lookahead prefetch slower; MMQ column-tile tuning spills on wave32).

**Evidence.** local evidence; coordinator session notes, 2026-10-08/09

### E37 - Other engines: halogen-flash-server and glm53-flash-offload (OPEN)

**Question.** How do other engines' published numbers compare with Maya on Strix Halo and a single big-RAM GPU?

**Setup.** Theirs: halogen on Strix Halo; glm53-flash-offload on 1x RTX 3090 + EPYC 7443P; halogen-flash-server: Qwen3.8-Flash-Next (179.55B, 5.53 bpw); glm53-flash-offload: ExLlamaV3/SGLang, EXL3 3.05 bpw (125 GB), NVIDIA only. None run by us.

**Result.** Theirs (not rerun): halogen-flash-server prefill 1,584 / 1,567 / 1,517 tok/s at 8K / 32K / 131K; decode 37.6 serial greedy, 46.0 with a draft head at 32K, 55.7-56.3 with draft head + prompt lookup; reports amd_iommu=off worth 13-16% prefill. glm53-flash-offload (1x RTX 3090 + EPYC 7443P): decode 28.2 tok/s with 218 GiB free RAM, 17.3 with a 55 GiB RAM cap; prefill 710 / 951 tok/s at 8K / 32K. Maya on Strix: 312-325 tok/s prefill (IOMMU off), decode 17.2-18.0.

**Decision.** Used as direction only: fused WMMA MoE (E04), single-GPU speculation (E18), IOMMU (E08, which we then measured). Different models, engines and quants; nothing here is a like-for-like benchmark.

**Caveats.** All numbers are theirs and were not rerun; halogen runs a different model and is Strix-Halo-only; glm53-flash-offload is NVIDIA-only. Figures taken from the coordinator's notes of their READMEs.

**Evidence.** [https://github.com/peonist-ai/halogen-flash-server](https://github.com/peonist-ai/halogen-flash-server); [https://github.com/sybil-solutions/glm53-flash-offload](https://github.com/sybil-solutions/glm53-flash-offload); hub ROADMAP.md; coordinator session notes, 2026-10-08/09

## Tooling

### E43 - DeepSeek Harness (dsh) set up for agent work (KEPT)

**Question.** Can a second agent harness (DeepSeek's) serve as overflow worker and as the driver for "GLM alone" showcase builds?

**Setup.** `dsh` 0.2.0-rc.2 (developer preview) installed globally; one-shot wrapper `dsh/dsh-run.sh <maya|deepseek> <workdir> <brief.md> <log>` (headless, full access, JSON events). `deepseek` route = the DeepSeek API (deepseek-flash); `maya` route = local GLM-5.3-Flash via the Maya server on :8099 (provider added to the dsh settings).

**Result.** Both routes passed a smoke. The fresh local `maya` route created a Python mean function, eight unittest cases and a report; ran its tests through the harness; and completed four local-model steps. Independent file inspection and test rerun: 8/8 pass. No other model/agent or external service used by the worker. Receipts retained; server unchanged.

**Decision.** KEPT: local-Maya edit/tool/test cycle validated; ready for scoped GLM-only showcase work. The showcase rebuilds themselves are not yet demonstrated.

**Caveats.** Preview software. Small arithmetic task, no failed-test repair needed; not evidence of large-project performance or autonomous recovery.

**Evidence.** coordinator session notes, 2026-10-09; local results/dsh-maya-smoke-20261009.md, logs/dsh-maya-smoke-20261009.log.jsonl, before/after receipts.

### E45 - R9700 transfer-window reuse audit (DROPPED)

**Question.** Is separate expert-transfer window sizing versus compute sub-batch
sizing a missing R9700 optimization, as suggested by the other-engine review?

**Setup.** Read-only source audit of the v1.0.23 integration at a03b840 and
the current R9700 startup log:131072 INT8 context, RAM90GB, reserve1024MB,
PREFILL_MB4096, SUB2048, prefill auto. No GPU requests or server changes.

**Result.** The effective prompt chunk is13312 tokens, mixer sub-batches2048,
with4086MB borrowed and160 expert prestage slots. The planner groups token
routes per expert over the chunk; each staged group member's weights are
copied before processing its routed rows. Chunk and compute sub-batch are
already independent.

**Decision.** DROPPED as a missing-feature port: the mechanism already exists.
Do not reduce the window to4096 or repeat E38's sub-batch sweep. Still-larger
chunks via a larger loan would be a separate, unmeasured trade-off involving
expert eviction/prestage capacity and decode recovery. An isolated ROCm10.2
runtime/table comparison completed in E46; no everyday switch.

**Caveats.** Static code and startup evidence, not transfer profiling or a
throughput A/B. Does not establish optimal loan/window size or hidden PCIe cost.

**Evidence.** Local results/r9700-prompt-window-audit.md and its preserved
everyday config/startup log; source src/core/glm_prefill.cu at a03b840.

### E46 - R9700 isolated TheRock runtime comparison (DROPPED)

**Question.** Does an isolated newer ROCm runtime/compiler and freshly tuned
gfx1201 GLM table improve the current single-R9700 configuration?

**Setup.** Same Maya v1.0.23 integration a03b840 and Maya-S-v2 pack, physical
HIP1 R9700 32GB, 131072 INT8 context, RAM90GB/reserve1024MB, SUB2048,
PREFILL_MB4096, PROMOTE_MIN6. ROCm7.2.1 versus pinned TheRock SDK
`10.2.0a20261009` / hipBLASLt100500; isolated install/build, no system changes.
Actual library mappings/hashes verified. Frozen usage seed cloned per arm.
Order: baseline, nightly, baseline repeat. Same token-counted uncached
payloads; two warmups, three scored 256-token decode replies per arm and
three 32-token-output prefill requests at each prompt length.

**Result.** Medians (ranges), tok/s:

| Metric / actual prompt length | ROCm7.2.1 | Nightly | Baseline repeat |
|---|---:|---:|---:|
| Decode /1663–1665 tokens |25.8 (23.6–27.9)|28.0 (27.9–28.0)|26.0 (23.8–27.7)|
| Prefill /8279–8335 tokens |787.1 (781.5–787.9)|537.3 (534.1–538.6)|785.0 (778.9–785.3)|
| Prefill /33051–33143 tokens |783.9 (783.5–785.0)|580.5 (580.4–581.2)|781.9 (781.7–782.9)|

Versus repeated baseline: observed decode +7.7%, prefill −31.6%/−25.8%.
All33 full responses pass independent length/preflight/cache/receipt audit.
All four correctness screens pass without skips. Fresh tuning:24 cases
(12 matrix shapes at1024/2048), finite/no padding writes, worst relative
L2 5.36e-06 / absolute0.002064. Effective chunk/sub/pool sizing matches.

**Decision.** DROPPED as an everyday runtime switch. Keep ROCm7.2.1; original
131072 server health, exact fox smoke and config/engine/table hashes restored.
Do not advertise the observed decode increase as a clean kernel-speed win:
all11 baseline-repeat replies match the original, none of the nightly's11
match. Inspected guide outputs are coherent, not quality-parity evidence.

**Caveats.** Runtime/compiler/table changed together. Nearest calibrated
token bucket is used for actual remainders, which were not separately tuned;
two selected Lt cases lose to hipBLAS and the fresh table is unfiltered.
Automatic CPU-lane split, tier placement and numerical differences are not
isolated; differing text changes routed-expert workload. No full-window
128K prompt, kernel timing attribution or new quality/coding eval. Small
request sample, not a blanket conclusion about all TheRock releases.
Initial fixture-cwd/cache-log-parser harness failures are preserved, not
misreported as numerical failures. No engine source changes.

**Evidence.** Local results/muse-r9700-rocm10.md and its evidence directory:
independent-audit.json, both outcomes, full requests/replies, receipts,
library hashes, configs/CMake caches, parity/tuning logs and restored health.
SDK/build logs under logs/r9700-rocm10-*. No new code push or PR.

### E47 - Two-row decode / single-GPU speculation prototype (OPEN)

**Question.** Can a shared-weight two-token verifier make single-GPU MTP pay,
with portable implementation intended for both Strix Halo and R9700?

**Setup.** Nimo gfx1151 only, v1.0.24-derived `exp/two-row-decode` source
aec1230, isolated TheRock build, context65536 FP16, MTP loaded, CPU lane
disabled, no reuse/slots. Four short prompts (80/76/98/76 actual tokens),
one300-token greedy continuation per mode. Everyday262144 config unchanged.

**Result.** Independent audit confirms all1200 ON/OFF token positions and
response texts match, actual lengths300, zero cache reuse and stable per-run
receipt/config/engine/table hashes. Observed rates:

| Prompt | OFF tok/s | SPEC tok/s | Observed ratio | Accept | C2/T |
|---|---:|---:|---:|---:|---:|
| Chat |18.5|21.1|1.141|66.1%|1.39748|
| Code |18.4|22.9|1.245|80.1%|1.40179|
| Reasoning |18.4|23.4|1.272|85.7%|1.40401|
| Prose |18.4|21.2|1.152|67.0%|1.39312|

Earlier V2 probe logs show299/299 bitwise rows for both positions per prompt,
maxabs0/argdiff0. Saved GEMV2 screen is bit-identical, reported GLM worst
ratio1.042 (all listed formats worst1.200, still below1.25); union-MoE and
32-row synthetic verify logs also pass. These logs predate the final rebuild.

**Decision.** OPEN prototype, not a validated release or default switch.
Do not say "all gates passed": mean C2/T1.39910 is below1.4, but code and
reasoning exceed it. OFF/V2 engine hash e396dfed… differs from final SPEC
fdc5086f…; 14–27% is observed throughput across builds, not a controlled
same-binary speedup. Final-build correctness and same-binary validation would
be separate GPU work, not launched by this follow-up.

**Caveats.** Saved selftests do not substantiate the worker's final-tree
five-test claim; this is missing evidence, not a demonstrated parity failure.
Usage seed preserved but initial per-arm hashes/reseeding history incomplete.
Historical harness preflight uses saved counts; current read-only API token
counts match, but no historical fresh API preflight is preserved. One sample
per mode; short prompts, not long-context or task-quality evidence. No
gfx1201/gfx1100/CUDA execution or measured R9700 gains. Earlier failed/profile-
dependent traces retained; causality of profile-related divergence unisolated.

**Restoration.** Fresh live health262144/version1.0.24, exact everyday
maya-nimo-256k.json path and sole GPU engine verified. Receipt configfb3b0737…,
engine99b0279c… and table2cd69361… match the producer's restart-before receipt.
Fused-v1026 initially failed on quota; its exact queued retry subsequently
finished and is independently audited in E48. No new PR/code push.

**Evidence.** Local results/muse-tworow-decode.md and results/muse-tworow/
independent-audit-20261009.json, raw responses/traces/receipts, fresh restored
receipt and evidence hashes; complete remote harness/profile/failed-run
snapshot and temporary gate logs preserved durably. Original producer report retained.

### E48 - gfx1151 fused MoE on v1.0.26 (KEPT)

**Question.** Does the gfx1151 clamp fix retain parity and prefill gains on
current-base v1.0.26 without changing the binary between arms?

**Setup.** Nimo gfx1151 only; source0035dbe on v1.0.26/7f7890f, clean before/after,
TheRock runtime, HIP ON/CUDA OFF, MMQ/native experts ON. One engine01ecf8ab…,
context65536 FP16, prefill32768, SUB4096, MB12288, S-v2 IQ2_XXS, table100500.
Only fused1/0 plus usage/log destinations differ. Three requests per size,
ON then OFF, temperature0, 32 actual output tokens each; cache0 throughout.

| Actual prompt tokens | ON median (range) tok/s | OFF median (range) tok/s | Gain |
|---|---|---|---|
| 4161–4178 | 333.2 (330.8–334.4) | 299.5 (297.3–299.7) | +11.3% |
| 8286–8334 | 338.3 (336.2–339.1) | 316.0 (314.1–316.2) | +7.1% |
| 16541–16585 | 329.4 (329.0–332.7) | 313.4 (312.7–316.2) | +5.1% |

**Audit.** All22 raw requests match their summaries and regenerated prompt
hashes; full lengths verified (except intentional 2-token fox replies).
48 fused cases pass without SKIP, relative L2 vs MMQ8.67705e-8–1.01027e-4;
MMQ/layer/model/attention gate logs pass. Help output present, rc0 producer-
reported. Config/exe/table hashes and boot state stable across arm receipts.
Prefill text matches6/9; warm-up texts differ despite both reaching200 tokens
at18.1 tok/s. Both fox checks return Fox: smoke, not task-quality parity or
a general decode-equivalence result.

**Caveats.** Saved API counts are one below actual prompt_n; conservative
17000+200 capacity check covers all requests. Historical per-request checks
and fresh-count timing before ON are not established. Seedbd8e285c… preserved,
but identical initial clones are producer-reported without per-arm initial
hash manifests. ldd resolves TheRock; benchmark process maps/library hashes
not saved. Three samples, one order, no CI; no gfx1100/gfx1201/CUDA execution,
long-context/quality rerun, hip_intrinsics build or speculation validation.

**Decision/restoration.** Keep the locally validated gfx1151 fix for PR preparation,
not a new default or upstream merge. It was later opened upstream as
[#98](https://github.com/mw00/project-maya/pull/98); the v1.0.32 rebase is
[E71](#e71---strix-v1032-rebase-and-pr-98-fused-prompt-moe-kept-open). Fresh independent receipt/live checks
verify maya-nimo-256k.json262144/v1.0.24, original configfb3b0737…,
engine99b0279c… and table2cd69361…; engine219105 is sole GPU owner.
Only owned orphan launch shell219100 was closed to release stuck SSH;
detached server survived, producer and queue exited. Both authorized jobs
finished; no GPU job queued behind them and no code push/PR/comment.

**Evidence.** Local results/muse-fused-v1026-validation.md, raw gate/request/
receipt/config/build logs, audit.py, independent-audit-20261009.json,
audit-evidence-hashes-20261009.json, audit-restored-20261009.json and preserved
producer report/profile/harness/CMakeCache files. Evidence class D.

## Later records (E49-E69)

E49-E69 consolidate the current working records: source reviews, CPU/build preparation and a few synthetic GPU runs.
Unless an entry says otherwise, the work was source-, CPU- or build-only, and "KEPT" means the record was retained,
not that the mechanism was adopted. Evidence is local unless a public link is given.

### E49 - Strix existing cold-prefix phase profile audited (KEPT)

**Question.** Where does Strix prefill time actually go, and does profiling change it?

**Setup.** Strix Halo (gfx1151, 128 GB unified), v1.0.24, 262144 context; ~8.1K / ~32.2K-token prompts, 32-token
outputs, cache 0, one request per cell.

**Result.** Profiled 351.0 / 339.4 tok/s at ~8.1K / ~32.2K; non-profile 8K was 347.6. One sample per cell, so no
profiler overhead or speedup is claimed. Event-interval shares: fused MoE 32.7/30.6%, DSA attention 17.9/19.8%, KDA
projections 17.8/19.0%, DSA projections 6.7/8.7%.

**Decision.** Kept as a diagnostic. Prompt work should target the math, not I/O.

**Caveats.** Event intervals are not pure kernel shares: expert intervals include staging waits and combine flushes,
and host disk waits overlap other work.

### E50 - Rotating CPU calibration preparation (OPEN)

**Question.** Does an opt-in rotating CPU calibration patch (the upstream issue #70 idea) prepare safely?

**Setup.** Isolated v1.0.23 patch, opt-in `STRATA_GLM_CPU_CAL_ROTATE=1`, default off; CPU-only checks.

**Result.** 256-call helper/sanitizer/CTest checks and 22 hashes pass; baseline equivalence holds. Leak checking was
disabled under ptrace. HIP/native quant parity, the real working set and paired full-256-token R9700 throughput were
not tested.

**Decision.** OPEN, CPU preparation only. No production integration or measured speedup.

**Caveats.** Literal upstream diff provenance unverified.

### E51 - GLM prefill applicability and selector oracle (OPEN)

**Question.** Do other engines' dense-fusion and selector mechanisms port to GLM prefill?

**Setup.** Pinned public source; CPU oracle regenerated (423 DSA helper cases, 795 wrapper, 6 oracle NaN rejections,
49 QSA comparisons, 29 fixtures).

**Result.** Direct Qwen selector/combine/SwiGLU ports do not match GLM's ordering, tail, clamping and HC contracts.
GLM already has exact linear radix selection and Q8/K-quant vector stores; IQ4_XS is conditional on the deployed
formats. Gufo bounded attention carry and packed routed 16-bit tiles remain source-only.

**Decision.** OPEN. Direct Qwen selector/store ports rejected; no measured AMD gain.

**Caveats.** CPU contract agreement is not GPU arithmetic/concurrency parity.

### E52 - IQ2_XXS BF16 WMMA preparation (OPEN)

**Question.** Can a BF16 WMMA IQ2_XXS gate/up kernel be prepared for gfx1151?

**Setup.** Benchmark-only single-expert kernel, K4096/M2048/N1..256, BF16 tiles, FP32 accumulate; serial gfx1151
compile.

**Result.** 391,680 rounding cases, 1,024 complete blocks, 16,777,216 decoder values, tail/layout checks and 87 hash
entries pass. Resource metadata: LDS 6656, VGPR 86, spills 0. No GPU run.

**Decision.** OPEN, CPU/compile preparation only. The measured successors E57/E60 lose to native INT8.

**Caveats.** Resource metadata is not measured occupancy/throughput; the candidate redecodes each 16-row tile.

### E53 - Strix Halogen/CIRU/Gufo applicability audit (OPEN)

**Question.** Which techniques from other Strix engines transfer to Maya?

**Setup.** Read-only reports; no weights or reference executables run.

**Result (theirs, not rerun).** Halogen 1502/1499 prefill tok/s at 8K/32K. CIRU 1251.39 tok/s at 64K (repeat 1267.40)
and 1002.96 at 256K, generating one token. Gufo 1700.52 fresh pp2048; 25.95 autoregressive decode, 31.32 mixed MTP,
60.39 repetitive MTP; its cached 128K row is about 2048 appended tokens, not cold 128K prefill.

**Decision.** Conditional leads only: packed routed 16-bit WMMA tiles, shape-selective dense projections, bounded
sparse-attention carry. Gufo F16 is not BF16; CIRU GDN is not GLM KDA.

**Caveats.** Different model, quant, active computation and protocol; no attainable GLM speed factor follows.

### E54 - R9700 health and table-gap lead audit (OPEN)

**Question.** Did the local control drift, and which table leads remain?

**Setup.** Read-only snapshot, v1.0.23 integration, 131072 context, INT8, RAM 90 GB.

**Result.** Config, engine and table hashes unchanged. The table has 40 Lt rows covering 64-1024 tokens and no 2048
bucket; nearest-1024 reuse still needs an independent resolver/dispatch check.

**Decision.** OPEN. Missing-2048 tuning, an admission sweep and speculation qualification remain future gated work.

**Caveats.** Historical receipt equality is not current live-state or exclusive-ownership proof.

### E55 - S-v2/M/L quant recipe inventory (OPEN)

**Question.** What is in the S/M/L quants, and are M/L measured?

**Result.** Documented in [QUANTS.md](QUANTS.md): sizes 96.5/116.0/156.3 GB; S/M/L gate-up IQ2_XXS/IQ2_S/IQ3_S;
ordinary down IQ2_S/IQ3_XXS/IQ4_XS; first/last four down IQ3_XXS/IQ3_S/Q5_K. The conversational ~21 tok/s M and
~15.5 tok/s L figures were inverse-file-size estimates, not measurements.

**Decision.** OPEN. No M/L download, speed or quality experiment.

**Caveats.** File size is not runtime-pack size; M/L cannot reuse S's 90 GB tier blindly.

### E56 - Maya/Strata PR acceptance and applicability audit (OPEN)

**Result.** Public metadata confirms Maya #38/#39/#61 merged on 2026-10-09 in v1.0.27. At the pinned Strata #1749
head, the HIP VMM resolver is enclosed by a guard excluded in supported HIP builds (static source only). Strata
#1752 recovery is not a healthy-server 10x claim; Maya already uses `O_DIRECT`. Maya #73 Windows numbers are not
Linux/AMD evidence.

**Decision.** OPEN. Merge acceptance recorded; no gains counted twice and no live control upgraded.

**Caveats.** Static source only; current PR states were not re-fetched.

### E57 - Strix CIRU-inspired BF16 GPU validation (DROPPED)

**Question.** Does the simple BF16 gate/up prototype beat native INT8 on gfx1151?

**Setup.** One synthetic GPU run, K4096/M2048, N1..256, five warmups and 20 repeats.

**Result.** BF16 cost 10.92-17.96x native INT8 by row; N64 2.290 vs 0.133 ms; N256 6.889 vs 0.383 ms. Nine sizes pass
the numerical/finite gates.

**Decision.** DROPPED for this one-wave prototype (redecode per 16-token tile, two projection launches), not for all
CIRU-inspired designs.

**Caveats.** Synthetic projection adapter, not fused/down/model quality; event spans include host and launch gaps.

### E58 - R9700 Lt2048 and calibration follow-up (OPEN)

**Question.** Is the missing 2048-token hipBLASLt bucket a real R9700 gap?

**Setup.** CPU/source audit of the production resolver; dedicated Lt driver built (compile/link only).

**Result.** The table has 40 rows, buckets 64/128/256/512/1024; T2048 maps to the nearest 1024/512/128 by shape.
66 manifest entries, 12 shapes x 5 lengths and calibration helpers pass on CPU. GPU support, parity and speed are
untested. GLM uses a 16 MiB workspace vs the tuner default 32 MiB; a row must beat hipBLAS by >=2% to be kept.

**Decision.** OPEN, build-only. Tune at 2048 only after exclusive GPU admission, same-binary table-only A/B.

**Caveats.** No measured R9700 gain; only the nearest solution was tried.

### E59 - BF16 grouped64 and cleaned timing preparation (KEPT)

**Question.** Can the BF16 v2 grouped decoder be prepared and verified on CPU?

**Result.** 591,872 blocks / 151,519,232 decoded values match the oracle bitwise; all 65,536 half scales, 524,288
code/sign/subscale blocks and 393,216 rounding boundaries are checked. ASan/UBSan pass on 69,632 blocks (leak checking
disabled under ptrace). 28 kernel hashes match before/after.

**Decision.** KEPT as CPU preparation, not production adoption. The GPU gates completed in E60.

**Caveats.** LDS 16000 is source accounting, not compiled resource or occupancy.

### E60 - BF16 v2 GPU qualification: native INT8 still wins (DROPPED)

**Question.** Does grouped64 BF16 v2 beat native INT8 on gfx1151?

**Setup.** One synthetic run, same binary, three balanced cyclic orders, five warmups and 20 repeats.

**Result.** v2 is 4.02-7.45x faster than clean-v1 BF16, but X+gate/up still costs 2.26-3.25x native INT8. N256 medians:
v1 6.89077 ms, v2 0.997555 ms, native 0.381516 ms. All nine N cases pass the parity/guard gates.

**Decision.** DROPPED. Keep native INT8; retain the prototype gain as research evidence.

**Caveats.** Single-buffer synthetic expert gate/up only; no actual weights, down/fusion or full-model quality.

### E61 - Public prefill commit methodology and tracking baseline (KEPT)

**Question.** What reusable prefill method and cross-engine baseline can be extracted from public work?

**Result.** One public-only worker checked three pinned HEADs, 26 commit objects and eight pivotal parent diffs. The
reusable method: define the numeric/state contract, profile the whole workload, pick one mechanism, build an
oracle-backed reproducer, inspect dispatch and VGPR/LDS/scratch, change one thing, alternate matched runs, then require
an end-to-end gain. Author results (theirs, not ours): Gufo `98641a6` reports +5.1% Qwen pp2048 from a
compiler-conditioned spill fix; Strata `a45783c` reports +31.9% on a 7900 XTX from missing small-T shapes, while
`3512797` reports ~0.8% whole-prompt despite 1.15x GEMM on R9700.

**Decision.** KEPT as methodology and a tracking baseline. No direct GLM port; no repeat of rejected tests.

**Caveats.** Author numbers were not rerun; the worker scan had date/precision errors, corrected by the coordinator.

### E62 - Radiance R9700 prefill methodology (KEPT)

**Question.** Which prefill methods in the public Radiance forks apply to a single R9700?

**Result.** Source-only audit of magiccodingman/vllm-radiance (six focused diffs, 22 hashes). Method notes: separate
decode/prefill shape policies, one-time FP8 preshuffle, avoid physical activation-scale transposes, collect actual
GEMM signatures then tune offline. Single-card correction: GGZ14/vllm-mxfp4 has a TP1 profile (author ~2.5-2.8K
prefill tok/s at 210 W; actual 47,014-token prompt 2480); SlyBase/vllm-sly-radiance explicitly targets one R9700 at
262144 context (author 3132/3136/3105/2949/2635 tok/s at 1.5K/6K/12K/24K/47K). These are author measurements, not our
tests and not a 128K claim.

**Decision.** KEPT as methodology. Paged attention is not MLA/DSA; FP8/MXFP4 is not IQ2.

**Caveats.** Sly kernel internals unaudited; different model and quant.

### E63 - Single-card Radiance dispatch and native IQ packing leads (KEPT)

**Question.** Are the single-card dispatch and packing ideas applicable?

**Setup.** CPU-only workers, independently audited.

**Result.** 23 worker + 28 Radiance + 12 Maya hashes and six parent diffs pass; 13 native IQ CPU tests cover 77
geometry cases; the default-off packed-X patch applies in check-only mode (quantizes X once/token, gathers 144-byte D4
blocks into existing scratch). Radiance's installed path supports conditional load-storage reuse and unsharded shape
dispatch. Illustrative T2048/R8T traffic 328 -> 185 MiB is not timing.

**Decision.** KEPT as CPU preparation, not a production patch. Halo's native fused path already reuses X, so this
targets the discrete MMQ fallback.

**Caveats.** No GPU numeric parity or matched whole-chain timing; external library internals unverified.

### E64 - XeStrata technique and commit audit (KEPT)

**Result.** Release and HEAD pinned. 117 manifest entries, 46 files vs Git, 21 parent diffs and 136/72/1 first-parent
intervals verified. A source-only conditional lead map; author Qwen/discrete AMD/Intel rates are not Maya gains, and
the APU implementation is explicitly untested in the release notes.

**Decision.** KEPT. No GPU, quality or performance gate follows.

### E65 - R9700 dispatch/parity and matched component timing (KEPT)

**Question.** Does the one-row hipBLASLt filter help at actual shapes?

**Setup.** Three synthetic GPU invocations (baseline, matched timing, one-row filter); 131072 context, 16 MiB
workspace, ROCm 7.2.1 / Lt 100202.

**Result.** All 66 actual-shape gates pass; 26 nonzero cases at T2048/37, 1,040 raw samples, balanced AB/BA rounds.
14 cells beat hipBLAS by >=2%, 8 lose by >=2%, 4 are near equal. The narrow loser is the dense-down second slice
N1366/K12288: at T2048 Lt 0.995 ms vs hipBLAS 0.663 ms, about 33.4%/32.0% of that synthetic operation (not of
prefill). The one-row-filter candidate passes all 66 gates (61 Lt, 5 intentional fallback).

**Decision.** KEPT as diagnostic and parity evidence. The model A/B followed as E66.

**Caveats.** Synthetic FP16, not IQ expert weights or whole-prefill throughput.

### E66 - Same-engine filtered-table model prefill A/B (DROPPED)

**Question.** Does removing the one losing row improve model prefill?

**Setup.** Original vs filtered table, same engine/model/quant/runtime, 131072 context, ABBA four fresh arms at
~8K/~32K, eight requests each, full 32-token replies, cache 0.

**Result.** A/B medians 787.3/784.5 and 799.65/798.0 tok/s: -0.36% / -0.21%. No model-level benefit.

**Decision.** DROPPED. Keep the original table.

**Caveats.** Two samples per arm/size and drift; no quality-equivalence claim.

### E67 - Two-PC validation and refinement queue (OPEN)

**Result.** A finite gated queue across Strix Halo and R9700 source/CPU preparation, one GPU job per host. Packed
activation reuse (H1) failed its first gfx1151 device run after 48/525 packed cases (byte mismatch); the follow-up was
stopped. A Halo raw-half cache (H2) compiled but is on static hold: the ON build also changed an unintended IQ4_XS
sibling and no gain was measured. The R9700 2048-token Lt work (N2) passed 66 geometry cases and CPU/source checks,
but its backend stayed uncompiled/unlinked with a trusted loader/runner missing. No new GPU/model run or measured gain;
the original servers were restored.

**Decision.** OPEN. Do not repeat a failed candidate without a new sealed version.

**Caveats.** Source/CPU completion is not GPU readiness; many server observations are dated, not fresh ownership.

### E68 - Public ROCmFPX long-context claim audit (KEPT)

**Question.** Is the author's 8.84% Gorgon / custom-3.66BPW / 128K claim applicable to Maya?

**Result.** Source-only audit. The claim is external, with unresolved revision, backend, actual-length and quality
provenance; a located public 3.84BPW Vulkan card is not the same proof. Five conditional source leads were recorded
(row-locality, few-row projection dispatch, producer fusion, contiguity, routed tiles); none is a demonstrated Maya
bottleneck.

**Decision.** KEPT as a source audit. No port or new GPU task follows.

**Caveats.** No locally measured gain.

### E69 - Documentation consolidation (KEPT)

**Result.** A public-only documentation pass audited 48 public experiment IDs/output hashes and merged 69 current
IDs/statuses into consolidated index and roadmap views. 39 draft mechanisms were not admitted as 39 new jobs;
negatives and provenance limits were retained.

**Decision.** KEPT. Documentation only; no speed result and no publication authority inferred.

## 2026-10-10 v1.0.32 results (E70-E77)

These entries record the measurements behind the current README table and the efficiency section. The controls are a
single R9700 at 128K INT8 (RAM tier 90 GB, reserve 1024 MB) and Strix Halo at 262144 FP16.

### E70 - Single R9700 everyday server on upstream v1.0.32 (KEPT)

**Question.** Do the single-R9700 everyday numbers survive the v1.0.32 merges (#38, #39, #52, #61)?

**Setup.** Upstream v1.0.32 (origin/main `2908669`), ROCm 7.2.1, R9700 (gfx1201), 131072 context, INT8 latents, RAM
tier 90 GB, reserve 1024 MB, PROMOTE_MIN 6, Maya-S v2. HIP release with MMQ/native experts/tests on.

**Result.** 130 serve unit tests pass (3 skipped); model test worst relative error 3.10e-06; layer parity passes on
gfx1100 and gfx1201. Decode 24.8 tok/s (v1.0.23 was ~25.1). Prefill 799.1 tok/s at 8,075 tokens and 796.4 at 32,269
(previous ~782/~784). Five 256-token replies all reached 256 tokens. The tools eval is 11/12; the miss is an extra
`web_search`, the same model behavior as before.

**Decision.** KEPT. The v1.0.32 everyday server is left running.

**Caveats.** Single runs of small samples; decode is ~1% below v1.0.23, inside run-to-run noise.

### E71 - Strix v1.0.32 rebase and PR #98 fused prompt MoE (KEPT-OPEN)

**Question.** Does the gfx1151 fused prompt-MoE + SwiGLU clamp still win on v1.0.32?

**Setup.** Strix Halo (gfx1151), v1.0.32 (`2908669`), TheRock ROCm 10.2, `amd_iommu=off`, 262144 context; fused vs
MMQ-off in the same binary, two runs per size.

**Result.** ~4K 339.6/336.9 fused vs 299.3/296.6 = +13.5%; ~8K 344.5/343.9 vs 318.0/318.7 = +8.1%; ~16K 333.7/340.8 vs
323.6/318.9 = +5.0%. Opened upstream as [#98](https://github.com/mw00/project-maya/pull/98).

**Decision.** KEPT-OPEN, pending upstream review.

**Caveats.** Small sequential samples; other architectures untested; quality equivalence not claimed.

### E72 - Two-row single-GPU speculation on Strix, v1.0.32 rebase (OPEN)

**Question.** Does the two-row verify still pay after the v1.0.32 rebase?

**Setup.** Strix Halo (gfx1151), branch `perf/two-row-decode-1gpu-spec` (`e7463ad`), four prompts x 300 greedy
tokens, a fresh server per request, ON vs OFF.

**Result.** chat 21.3 vs 18.8 (+13.3%), code 22.6 vs 18.8 (+20.2%), reasoning 22.7 vs 18.8 (+20.7%), prose 19.5 vs
18.6 (+4.8%); token streams identical (ndiff 0). Draft/accepted: 174/126, 166/134, 165/134, 190/109.

**Decision.** OPEN. Unmerged and not the everyday configuration.

**Caveats.** Token identity holds per fresh session; back-to-back prompts drift through the usage profile. The opt-in
`GEMV2_PERF_GATE` fails ~1.40 vs 1.25 on microsecond-scale shapes, while real GLM shapes are ~1.00-1.03 (1.10 for
IQ2_XXS). Overlaps upstream #76/#96.

### E73 - Upstream #96 single-GPU MTP on R9700 (OPEN)

**Question.** Does upstream's single-GPU MTP help a lone R9700?

**Setup.** v1.0.32, Maya-S, 128K INT8, five 256-token greedy replies, median of the last three.

**Result.** The gate keeps it off: setup reports "CUDA0 VRAM holds ~28% of the experts", below the one-third gate;
at 8192 context it is still ~29% (83 slots/layer). Natural-on equals no-spec. Forced (`STRATA_GLM_MTP_DRAFT=3`): chat
27.0 (+5.5%), code 24.9 (+3.3%), reasoning 29.5 (+12.6%), prose 24.8 (-4.6%); acceptance 73.8/81.5/85.3/58.1%, 1.6-2.7
tokens/round. Cost: a 173 MB block, pool 80 -> 77 slots/layer (3360 -> 3311; RAM tier 63.65 -> 66.29 GB). A
layout-matched control shows 0/-3.7/-2.3/-1.5%, so the block's slot/read cost lands on code and prose. Prefill is
effectively unchanged (~794 vs 793 natural, 781 forced at 8K).

**Decision.** OPEN. The gate makes this not applicable to a single R9700 today; forced gains are per-prompt-type and
need a gate decision. Posted on upstream [#96](https://github.com/mw00/project-maya/pull/96).

**Caveats.** The forced arm changes memory layout; separate engine launches already vary greedy text.

### E74 - Upstream #95 RDNA GEMV on RX 7900 XT (KEPT-OPEN)

**Question.** Does the RDNA GEMV path help RDNA3 decode?

**Setup.** RX 7900 XT, 32K INT8, five 256-token greedy replies, median of the last three.

**Result.** Mean decode 20.1 -> 20.3 tok/s (+1.1%): chat +1.9%, code +1.1%, reasoning +1.0%, prose +0.5%. Prefill
unchanged (338-346 tok/s). `glm_mv_parity` passes 333/333 bit-exact (max_abs 0).

**Decision.** KEPT-OPEN as a measurement on upstream [#95](https://github.com/mw00/project-maya/pull/95); worth
enabling on gfx11.

**Caveats.** Small samples; the bit-exact parity is the strong evidence, not cross-launch text.

### E75 - R9700 prefill profile: compute-bound, transfers hidden (KEPT)

**Question.** Is single-R9700 prefill limited by transfers or by compute?

**Setup.** v1.0.32, gfx1201, 131072 context, INT8 KV, RAM tier 90 GB, prompts ~8.1K / ~32.2K, PREFILL_SUB 2048,
cache 0. Profile runs match the 799.1/796.4 baseline.

**Result.** The copy lane is under 30% busy; the last staged copy lands 1-2 ms before the layer ends and prestage
starts 60-670 ms early. All 12,096 experts fit in VRAM+RAM (zero disk waits); RAM_GB 120 is a no-op. Phase shares
(8K/32K): DSA attention 20.7/22.7%, KDA projections 19.1/18.5%, MoE total 28.4/26.9%, the rest dense projections and
glue. Achieved GEMM throughput: MoE routed 48.9-58.0 TFLOPS, dense FFN 81.2-83.0, shared 88.6-93.5, about 17% of the
corrected FP16 peak. Chunk 13312 -> 32768 tokens at 32K cut total H2D copies 193 -> 67 GB and gained +4.1% (792.6 ->
824.4 tok/s).

**Decision.** KEPT. There is no gfx1201 fused prompt-MoE path in v1.0.32 (the fused path is the gfx1151 #98); ranked
levers are attention/dense projections, then MoE grouping plus a bigger chunk.

**Caveats.** One profile; phase intervals are not pure kernel time; the chunk measurement is single-run.

### E76 - R9700 GEMM efficiency audit (DROPPED)

**Question.** How much is left in hipBLASLt GEMM selection on R9700 prefill?

**Setup.** v1.0.32, gfx1201 at 2350 MHz, 64 CUs. Measured peak FP16 154 TFLOPS (best measured 138, 90%); INT8/FP8
about 308 TOPS.

**Result.** Prompt projections average ~101 TFLOPS = 66% of FP16 peak, but GEMMs are only ~11.4% of prefill wall time
(0.142 of 1.251 ms/token). Re-picking the best testable GEMM is +1.0%; flooring all at >=60% is +1.4%; a floor of 85%
is +2.6%; a perfect table is +3.9%. The worst shape is KDA attention-out N=4096/K=8192 at 35% of peak. FP8 E4M3
reaches ~316 TFLOPS (~2x FP16) but needs a pack/dequant path.

**Decision.** DROPPED (parked, not disproven). Reopen if the GEMM share rises above ~20%, a pack allows FP8
projections, or a new ROCm/hipBLASLt arrives.

**Caveats.** Synthetic GEMM shapes, not IQ expert weights; the peak is a clock/vendor calculation.

### E77 - Strix prefill disk/tier-stall hypothesis closed (DROPPED)

**Question.** Do Strix Halo prefill stalls come from disk/tier borrowing?

**Setup.** Strix v1.0.32 + PR #98, 262144 context, `--prefill 32768`, PREFILL_MB 12288, SUB 4096, a fresh server per
request; 11 arms.

**Result.** Baseline 32K 340.6 tok/s with 10.4-14.0% host disk wait, 791-806 disk experts and 1,148-1,149 reads. None
of the 11 arms beat baseline: LAND=96 336.8, RAM_GB=4 338.0, RAM_GB=8 335.4, RAM_GB=12 328.1, PREFILL_MB=2048 307.7,
`--prefill 16384` 333.9, LEND_PCT=50 338.9, PREFILL_SUB=1024 323.7. Overlap proof: RAM_GB=8 copies 15 ms vs baseline
25,581 ms, and the compute-stream layer end is within 0.3 s. Decode at 8K: baseline 16.9 vs RAM_GB=8 17.5.

**Decision.** DROPPED. Strix prefill is compute-bound; the disk/tier-stall hypothesis is closed. No code change.

**Caveats.** One request per arm/cell; dated server state.

## Lessons

- **Confirm a root cause before attributing a crash.** The first memory-fault was put down to another process using the GPUs because 80 clean rounds followed. It reproduced on one card later. Keep the failing configuration and bisect (E29, E30).
- **A fix that passes is not the same as a cause that is shown.** #39 passed 33/33 reproducers plus 3 supplemental, but the report did not rerun the historical crashes to say which race caused which (E30).
- **Record the host state with every run.** The IOMMU setting changed Strix prefill by 11-15% (E08), and a vLLM service holding GPU memory cut decode to ~5.3 tok/s (E27). Receipts exist from 2026-10-09 on; check GPU-memory owners before measuring.
- **Record the build, not just the code.** nimo had an old cache with `STRATA_PREFILL_MMQ=OFF`, so Strix prompt numbers before the evening of 2026-10-08 came from a different path. A mixed GPU pair silently ran one card on plain hipBLAS (E25).
- **Bench only as deep as the decision needs.** Most entries here are 2-5 requests per arm. That was enough to drop ideas that were inside noise (E15, E17, E24) and not enough to settle small gains (E04, E12 on two GPUs). The IOMMU A/B (E08) is one request per size..
- **Check the request fits before you send it.** One benchmark round was refused because an 8,030-token prompt plus 200 output tokens exceeded the 8,192 context (a harness bug); the preflight script now exists for this.
- **Defaults follow the machine.** `--prefill auto` was 22-24% slower than hand-tuned chunks on Strix (E09); on AMD the sub-batch matters as much as the chunk (E01).
- **Measure on every architecture you claim.** gfx1151 was compiled but not run for the #39 fix; #38's wmma2 loses to f16q on Strix; #19 left gfx1201 alone (E03, E12, E30).
- **Keep dropped ideas written down.** E05, E06, E15-E17 and E24 record why not, so they are not re-run.
- **Say what you did not measure.** A sampled AMD short-task baseline exists (E35), but no matched NVIDIA control or AMD KL/perplexity. E40/E41 cover current long-context configs; the two-GPU #44 run remains pending (E24).
- **Gate kernel changes with speculation off.** With #96 MTP speculation on, end-to-end decode tracks acceptance, which moves with tiny numerical changes (an MLA rounding change moved acceptance 58% -> 68% and decode ~19 <-> ~22 tok/s), so spec-on e2e is not a valid gate; gate kernel work with `STRATA_GLM_NO_SPEC=1` (or GPU-busy ms) and gate the spec policy separately with acceptance reported (E88, E90).
- **A spec-sheet peak is not a measurement.** Strix's assumed "theoretical peak" of 59 TFLOPS was the 2x-FP32/WMMA spec; the WMMA f16 figure is ~30 TFLOPS and the measured hipBLASLt f16 peak is 24.2 TFLOPS (20.9 at the prefill clock). At the prefill clock Strix runs ~2.47 GHz at 85 W in the stock "balanced" power mode. Against the measured ceiling the engine is at ~50%, not 21% (E94).
- **KL is a diagnostic, not the decision.** Maya's own KL(FP8||Maya-S) = 0.43 and Maya-M = 0.33 with equal 5-task accuracy (80.7 vs 80.8): lower KL did not mean better tasks. KL and top-1 agreement measure the volume of divergence, not its direction, so use them as a screen/tripwire and decide on paired task cases (E95, E98).
- **Sub-gate wins are bundled, not dropped.** A lever that misses a per-arm gate can still stack: `PREFILL_SUB=8192` was +1.7% at 32K (E94), the MMQ chunk-fastest quant +0.6-0.8% on the fallback path (E96), and wmma2 on the second card +2.9/+5.7% (E99). Combine and re-measure the bundle rather than discarding each for missing a gate alone (E94, E96, E99).

### E78 - Strix: upstream PR #96 MTP vs our two-row spec loop (KEPT-OPEN: support #96; our loop DROPPED as primary)

**Question.** On Strix Halo (gfx1151, 128 GB unified) does upstream #96's natural MTP decode beat our
`perf/two-row-decode-1gpu-spec` (`STRATA_GLM_SPEC_1GPU=1`), and could our bit-identical two-column GEMV
(`gf::mv2`) add to #96?

**Setup.** nimo `build-rocm10` (TheRock ROCm 10.2, gfx1151, Release, tests ON): v1.0.32 `2908669`
spec-off (`STRATA_GLM_NO_SPEC=1`); PR #96 `3486f6d` (on #76 `aac3d49`) natural + `MTP_TRACE`; ours
`e7463ad` `STRATA_GLM_SPEC_1GPU=1`. Fresh `serve.server` per request, a copy of the same usage profile per
run, 300 greedy tokens, the four sol-mtp prompts, one ~8K prompt per arm, one T=0.7 #96 run. `receipt.sh`
±each arm; preflight 15/15; `glm_layer_parity --selftest` and `glm_model_test --selftest` PASS on all three.

**Result.** tok/s chat/code/reasoning/prose: v1.0.32 19.1/19.0/19.0/19.0; #96 21.4/22.6/25.6/21.3; ours
21.5/22.8/22.4/21.1. #96 acceptance 62-70%, 1.68-2.91 tokens/round, verify 74.5-103.9 ms/round; ours
acceptance 68.5-81.8%, 1.69-1.82 tokens/round (verify not exposed). 8K prefill 322.8/319.1/316.7 tok/s.
Sampled #96 19.8 tok/s, 50% accepted.

**Decision.** KEPT-OPEN in favour of #96: support it with data; drop our standalone spec loop as the
primary path. `gf::mv2` remains an optional follow-up only behind a profile showing an exposed dense-GEMV
share; current evidence does not justify it (#96's verify is already row-amortised).

**Caveats.** One request per cell (no repeats); differences <=1% are noise. Restore: everyday
`maya-nimo-256k.json`, `/health` ok, sha `fb3b07...`/`99b02...` unchanged. Report:
`results/ds-strix-pr96-vs-ours.md`.

### E79 - Strix Halo long-context decode: a pinned RAM tier parks the prompt borrow's evicted experts (KEPT: `RAM_GB=8` cuts SSD re-reads 2.79 -> 0.04/token and lifts 64K decode 15.40 -> 16.50 tok/s (+7.1%), 1.6K +3.4%, 64K prompt flat)

**Question.** Why does Strix Halo decode fall after a long prompt, and can a pinned RAM tier recover it?

**Setup.** nimo, gfx1151, `maya-v1032/build-rocm10` (`cb70490`, v1.0.32 + fused prompt MoE), 256-token replies after ~1.6K / ~64K / ~120K tokens from the same copied usage profile, `STRATA_GLM_PROF=1` + `STRATA_DECODE_TIMING=1` + `STRATA_GLM_TIER_DIAG=1`; `RAM_GB` 0/4/6/8/10 plus `PROMOTE_MIN=6`, `PROMOTE_FILL=96` and `RAM_RESIDENT=1`; fresh server per arm, receipts + preflight, greedy.

**Result.** 64K decode 15.40 -> **16.50** tok/s (two-run mean, +7.1%), 1.6K 17.66 (+3.4%), 64K prompt 327.1 -> 327.0 t/s (-0.03%); SSD reads 2.79 -> **0.04**/token, disk wait 4.59 -> **0.12** ms/token, VRAM hit 99.14%, 1207 slots (421 parked). 4/6 GB only partly cover the borrow (15.89/16.23; reads 1.50/0.75); 10 GB covers it but is no better (16.24, reads 0.00) and pins 2 GB more. ~119K prompt: ram8 316.0 t/s / 16.02 tok/s, ram6 315.2 / 15.70. `PROMOTE_MIN=6` is harmful (11.01 tok/s, 19.74 reads/token); `PROMOTE_FILL=96` 16.60 and `RAM_RESIDENT=1` 15.44 are no-ops. Upstream PR #15 (`STRATA_GLM_RAM_SHADOW`) is absent from v1.0.32; it would do the same borrow work more adaptively (no fixed 8 GB pin) but on Strix still needs ~8 GB of tier, so it would not beat `RAM_GB=8` here.

**Decision.** Adopt `STRATA_GLM_RAM_GB=8`; written to `project-maya/maya-nimo-256k-ram8.json` (sha256 `562b8e01...`); everyday `maya-nimo-256k.json` restored, `/health` ok.

**Caveats.** One request per arm/cell; dated server state; all `maya-nimo-*.json` were externally rewritten to greedy (`temperature 0.0`) during the session. Report: `results/ds-strix-longctx-decode.md`, evidence `results/ds-strix-longctx-decode/`.

### E80 - Single-R9700 Maya-S config tuneup (v1.0.32) (KEPT: only `RESERVE_MB 512` helps, 26.30 -> 26.66 tok/s pooled (+1.35%, 80 -> 82 slots/layer); `PREFETCH_N=1` -20%)

**Question.** Which single-knob config change cheaply improves R9700 decode without a cost?

**Setup.** `/ai/github/maya-main/build-multi` upstream v1.0.32 (`2908669`), HIP device 1, gfx1201, 131072 ctx, INT8 KV, RAM_GB 90, PROMOTE_MIN 6, frozen usage profile; every arm a fresh server + receipt + preflight, 3x256-token warm-up, then four prompt types x 4 x 256 greedy (last-3 mean), one ~8.1K and one ~32.2K prompt (final arm also ~119K), per-request `glm stat`.

**Result.** Baseline (`RESERVE_MB 1024`) 26.18 / 26.43 tok/s (pooled 26.30, 70.6% hit, 34.6 PCIe experts/token, 63.6 CPU-lane, 80 slots). `RESERVE_MB 512` 26.70 / 26.62 (**+1.2..+2.0%**, pooled **+1.35%**), 82 slots/layer, hit 71.4%, 33.8 PCIe experts/token, prompt 8K/32K/119K 804.0 / 789.4 / 746.8 (within 0.3%, no OOM). `--prefill 32768` decode-flat 26.27 and +1.8% at 32K, but the engine auto-halves the chunk 32768 -> 16384 once decode fills the pool. `PROMOTE_MIN 4` 25.59 (-2.7%), `PROMOTE_MIN 8` 26.52, `VRAM_EVICT=lru` 26.36, `CPU_LANE=12` 26.42, `CPU_CAL` 26.50, `65536 ctx` 26.66 (+1.3%, 82 slots, halves context), combos <=+0.9%. `PREFETCH_N=1 AT=fetch BLOCKS=4 RANK=2` 21.05 (**-20%**) despite hit 74.6%: the prefetch steals the PCIe5 x16 demand fetch (#84 warning confirmed on gfx1201).

**Decision.** Adopt `STRATA_GLM_RESERVE_MB=512`; do not enable `PREFETCH_N=1`; keep 131072 ctx. Deliverable `project-maya/maya-r9700-tuned-7900.json`.

**Caveats.** One run per arm (baseline and `RESERVE_MB 512` repeated); CPU-lane start-up calibration varied 0.169-0.201 ms/expert, so single-arm deltas below ~2% are noise. Report: `results/ds-r9700-tuneup.md`, evidence `results/ds-r9700-tuneup/`.

### E81 - nimo next everyday: v1.0.32 + fused prompt MoE (#98) + PR #96 MTP + `RAM_GB=8` (KEPT: merge `3fca883` accepted and switched; 4-type decode 18.48 -> 22.39 tok/s (+21.2%), decode after ~64K +25.8%, prompt -1.2..-1.5%)

**Question.** Does merging upstream PR #96 MTP onto the v1.0.32 + fused-prompt-MoE everyday build beat the everyday config?

**Setup.** nimo, gfx1151, worktree `maya-next2`, branch `nimo/next2`, merge `3fca883` = PR #96 head `3486f6d` (on #76, base v1.0.30) merged onto `cb70490` (v1.0.32 + #98 fused prompt MoE), TheRock ROCm 10.2, exe `0fb53741`; baseline `maya-nimo-256k.json` (`maya-next` v1.0.24, exe `99b0279c`); fresh `serve.server`, pristine copied usage profile, receipt + preflight, 3x256 warm-up then 256-token greedy replies; all four required tests plus `glm_mv_bench --parity-only`.

**Result.** Decode tok/s chat 18.50 -> 21.40 (+15.7%), code 18.50 -> 22.80 (+23.2%), reasoning 18.45 -> 24.20 (+31.2%), prose 18.45 -> 21.15 (+14.6%), mean 18.48 -> **22.39** (**+21.2%**). Prompt 8340 338.9 -> 333.7 (-1.53%), 33117 330.5 -> 325.6 (-1.48%), 66211 324.0 -> 320.2 (-1.17%). Decode after ~64K 15.50 -> **19.50** (+25.8%); `RAM_GB=8` parks the borrow (disk 2.26 -> 0.02 reads/token, RAM tier 544/1198). Tests PASS: `glm_layer_parity`, `glm_model_test`, `hip_prefill_fused_parity`, `hip_prefill_mmq_parity`, `glm_mv_bench --parity-only` (differ=0).

**Decision.** Switch nimo everyday to `maya-nimo-next.json` (262144 ctx, `temperature 0.0`, `STRATA_GLM_RAM_GB=8`); keep `maya-nimo-256k.json` (sha `25f5da27`) for rollback; `/health` ok.

**Caveats.** One request per cell; six merge conflicts resolved in the commit. Report: `results/ds-nimo-next-everyday.md`, evidence `results/ds-nimo-next-everyday/`.

### E82 - R9700: Maya-M IQ2_S vs Maya-S IQ2_XXS, greedy (KEPT-OPEN: M is a modest quality win, not a long-context one)

**Question.** How much better is Maya-M (`GLM-5.3-Flash-Maya-M-IQ2_S`, 109 GiB, `RAM_GB 110`) than Maya-S
(IQ2_XXS, `RAM_GB 90`) on one R9700 (HIP device 1), same build `/ai/github/maya-main/build-multi` v1.0.32 and
settings (131072 ctx, INT8 KV, RESERVE_MB 1024, PROMOTE_MIN 6, temperature 0)?

**Setup.** Fresh `r9700-m`/`r9700-long` server per pack; `receipt.sh` + `preflight.py` around each; 3x256 warmup
then 3x256 decode and one ~8K prompt; brief eval math 40 / MMLU-Pro 56 / HumanEval 30 / IFEval 40 / tools 12,
re-run greedily for both packs; NIAH from `results/muse-r9700-longctx/` re-run at 32K/64K with that harness but
an exact-match scorer; pagoda showcase with thinking off. A concurrent tuneup agent restarted the server mid-S;
pagoda S was saved and S perf/eval/NIAH were re-run cleanly.

**Result.** Weighted quality S 87.6% -> M 92.1% (+4.5 pp): MMLU-Pro 78.6->89.3, IFEval 77.5->82.5, tools
91.7->100, math 100->97.5, HumanEval 100=100. NIAH exact 4/4 and 4/4 for both packs (the old v1.0.23 S 2/4,
1/4 was build/tier noise, and the 44817 substring false positive is gone). M costs decode 25.0->21.9 tok/s
(-12.4%), ~8K prompt 789->750 tok/s (-4.9%), RAM tier 63.65->81.22 GiB pinned, 80->66 slots/layer; MemAvailable
with M loaded 87.4 GB (>=24). Both pagodas: 0 JS errors, S 3m01s / M 3m42s.

**Decision.** KEPT-OPEN: M is a real but modest quality win, worth ~12% decode where MMLU-Pro/IFEval/tool-call
accuracy matters; it is not a long-context upgrade. Server restored to `r9700-long` (S), `/health` ok, temp 0.0.

**Caveats.** n=12-56 per suite carries +/-8-15 pp; single pinned-tier load per pack. Report:
`results/ds-r9700-m-vs-s.md`, evidence `results/ds-r9700-m-vs-s/`.

### E83 - R9700 everyday config: RESERVE_MB 1024 -> 512 (ADOPTED: `RESERVE_MB 512` is now the everyday r9700-long setting, +1.35% pooled decode, 80 -> 82 slots/layer, prompt within 0.3%)

**Question.** Should the `RESERVE_MB 512` tuneup win (E80) be made the everyday r9700-long setting?

**Setup.** Applies E80's `results/ds-r9700-tuneup.md` to `project-maya/maya-r9700-long-7900.json`; recorded in the 2026-10-11 R9700-everyday update.

**Result.** `maya-r9700-long-7900.json` now uses `STRATA_GLM_RESERVE_MB=512` (identical to `maya-r9700-tuned-7900.json`); server restarted, HEALTH-OK, temp 0.0, 131072 ctx. Backup of the 1024 version: `config-backups/temp-20261010/maya-r9700-long-7900.json.reserve1024`. Maya-S stays the everyday pack; Maya-M remains the opt-in `maya-r9700-m-7900.json` (E82).

**Decision.** ADOPTED.

**Caveats.** Config change traced to E80's two runs; no new benchmark. Report: `results/ds-r9700-tuneup.md` (E80).

### E84 - Strix (gfx1151) prompt attention: wave32 WMMA (wmma2) vs rocWMMA (f16q) (KEPT-OPEN: wmma2 is the gfx1151 default on the branch)

**Question.** On Strix Halo (gfx1151, 128 GB unified) is the gfx11 wave32 WMMA
prompt-attention kernel `wmma2` faster than the rocWMMA `f16q` path, and at what
numerical error, so that `STRATA_GLM_PREFILL_ATTN=wmma2` can become the default?

**Setup.** nimo, worktree `maya-wmma2-gfx11`, branch `exp/wmma2-gfx11` from the
everyday base `nimo/next2` `3fca883`, TheRock ROCm 10.2, gfx1151, Release. The
gfx11 branch of `mla2_wmma` was already present from upstream #38 (`b7b8d75`);
this run changed only the arch tag. Gate 1 `hip_glm_prefill_attention`; gate 2 a
new kernel-alone bench (`tests/hip/glm_prefill_attn_bench.cpp`, H=64, KV=512,
n_sel->2048, flat and 4096-token chunked); gate 3 wmma2 vs f16q on the same
binary (sha `16d821a0...`), fresh `serve.server` per arm/cell, copied usage
profile, `receipt.sh` + preflight per cell, 3x256 warm-up, 2 runs per prompt cell.

**Result.** Gate 1 PASS, wmma2 rel L2 vs F32 3.42e-07 FP16 / 3.35e-07 INT8
(matches gfx1100 3.42e-07/3.35e-07, inside 1e-7..1e-6); f16q 9.78e-05/9.99e-05.
Gate 2 PASS: wmma2 1.356x/1.350x f16q at 8K flat/chunked and 1.411x/1.328x at
32K. Gate 3: prompt 346.6/334.3 tok/s at ~8K (+3.69%), 339.5/327.1 at ~32K
(+3.79%), 333.5/319.9 at ~64K (+4.25%); decode flat within scatter;
`glm_layer_parity` and `glm_model_test` PASS. Greedy 3x64 output differs across
the two kernels at the first token they disagree on (chat char 139, code 222,
reasoning 160); both arms are deterministic on rerun, and wmma2 is the more
accurate kernel, so this is not a quality regression.

**Decision.** KEPT-OPEN: `mla_wmma2_arch()` tags gfx1151 (code 4) and
`mla_wmma2_default()` selects wmma2 on gfx12 and gfx1151, on
`exp/wmma2-gfx11` `7e78792` (not merged into the everyday `maya-next2` build).
gfx1100/1101 need the same tag plus gate-1 and 8K/32K speed checks on the 7900 XT.

**Caveats.** Strix only; one APU and one microbench binary; end-to-end n=2 per
cell. Report: `results/ds-nimo-wmma2-gfx11.md`, evidence
`results/ds-nimo-wmma2-gfx11/` and nimo `~/maya-wmma2-gfx11-results/`.

## E85. Strix (gfx1151) decode kernel levers: hyper-connections and MoE down

**Question.** The Strix decode roofline (E77, `results/ds-decode-roofline.md`) puts
decode at 16.7 tok/s against a 24.0 tok/s bandwidth floor and names three weak
classes: hyper-connections `hc` 1.51 ms/token (41% of 229 GB/s), MoE down 4.81 ms
(72%) and the DSA indexer scan 0.57 ms (66%). Can a kernel change in those classes
close part of the gap?

**Setup.** nimo, gfx1151, TheRock ROCm 10.2, worktree `maya-decode-kernels` on
branch `exp/decode-kernels-gfx1151` from the everyday base `nimo/next2` `3fca883`
(v1.0.32 + #96 + fused prompt MoE), `STRATA_GLM_RAM_GB=8`, frozen usage profile,
greedy (temp 0). Per-class profile: fresh `serve.server` with `STRATA_GLM_PROF=1` +
`STRATA_DECODE_TIMING=1` + `STRATA_GLM_NO_SPEC=1`, `receipt.sh` and preflight,
one 1.6K-token prompt, 49 decode steps, all 12096 experts resident (99.26% VRAM
hit, 0 disk reads/token). Kernel microbenchmark: a new `tests/hip/hc_bench.cu`
(HIP graph, 60 calls, median) and the existing `hip_expert_kernel_bench --sweep`.

**Result.** Profile: token 54.14 ms (18.47 tok/s); hc 1.514 ms = 2.80% of the token
(hc_attn 0.768 + hc_ffn 0.740) over 141.6 MB = 93.5 GB/s (40.8% of 229); MoE down
4.821 ms = 8.90% over 797 MB = 165.3 GB/s (72.2%); DSA indexer 0.575 ms = 1.06%
over 87.4 MB = 152 GB/s (66.4%). hc microbench: hc1 26.60 us, hc2 21.32, hc3
(default) 9.17 us/call with the 786 KB weights L2-resident (85.7 GB/s weight-only),
so hc is latency-bound, not DRAM-bandwidth-bound; the serial 20-iteration Sinkhorn
(GGUF value 20) is 2.15 us = 23%, two grid barriers 0.86 us, an empty kernel
1.84 us. A bit-identical mode 4 (Sinkhorn on warp 4 concurrent with the
normalise/q8 pass; x/pre/comb memcmp equal to mode 3) is only 1.047x. MoE down
sweep: `down<22>` (IQ2_S) baseline 113.6 us, best `direct_lds_rows4` 109.0 us =
1.042x (production 0.983x); `down<18>` (IQ3_XXS) baseline 132.4 us, best
`direct_lds` 129.6 us = 1.021x. Wider loads (`vector_load`), more rows/wave
(`rows8`/`direct_rows4`) and LDS staging (`direct_lds`) are already in the sweep.
All parity tests PASS; `hip_expert_kernel_bench --parity-only --sweep` is
620/620 bit-identical.

**Decision.** CLOSED-no-gain. No change met the >=1.15x microbench gate, so none
advanced to the >=2% end-to-end gate; the gate is arithmetically out of reach for
these classes (hc 2.80% share, MoE down 8.90% share at best 1.042x, indexer 1.06%).
The bit-identical hc mode 4 and the hc_bench diagnostic are on the branch; the
production default is unchanged. The larger surviving Strix decode levers are KDA
projections (20.3 ms, 37% of the token) and launch-gap/tail overhead.

**Caveats.** Strix only; one APU; end-to-end n=2 per cell (chat 19.3/19.7,
code 20.5/20.1, reasoning 26.0/25.9, prose 19.4/19.5; mean 21.3 tok/s).
Report: `results/ds-nimo-decode-kernels.md`, evidence
`results/ds-nimo-decode-kernels/`, logged E85.

### E86 - nimo everyday: wmma2 prompt attention adopted (ADOPTED: `maya-nimo-next.json` gains `STRATA_GLM_PREFILL_ATTN=wmma2`, +3.7/+3.8/+4.3% prompt, decode flat, parity PASS; upstream PR #113 opened 2026-10-11)

**Question.** Make the validated gfx1151 wmma2 prompt path (E84) the everyday default, and offer the one-line default upstream?

**Setup.** nimo, gfx1151, everyday `maya-next2` binary (exe `0fb53741`), config `maya-nimo-next.json`; set env `STRATA_GLM_PREFILL_ATTN=wmma2`; backup `maya-nimo-next.json.bak-prewmma2`; server restarted, engine env verified.

**Result.** Prompt +3.7 / +3.8 / +4.3% at ~8K/32K/64K; kernel-alone 1.33-1.43x; wmma2 rel L2 3.42e-07 FP16 / 3.35e-07 INT8 vs f16q 9.78e-05 / 9.99e-05; decode flat; greedy tokens change vs f16q (wmma2 is the more accurate kernel); `glm_layer_parity` and `glm_model_test` PASS. The upstream candidate (the one-line gfx1151 default in `mla_wmma2_arch`, branch `exp/wmma2-gfx11` `7e78792`) was opened as upstream PR #113 on 2026-10-11.

**Decision.** ADOPTED on nimo.

**Caveats.** Strix only; end-to-end n=2 per cell; gfx1100/1101 remain f16q until gate-1 plus 8K/32K checks on the 7900 XT. Report: `results/ds-nimo-wmma2-gfx11.md`, evidence `results/ds-nimo-wmma2-gfx11/`.

### E87 - Strix (gfx1151) decode: ranked excess table and KDA projection saturation (CLOSED-no-gain: KDA projections are bandwidth-saturated, `kda_proj` 224.3 GB/s = 98.0% and `kda_out` 93.3% of the 229 GB/s ceiling; no variant met the 1.15x gate)

**Question.** Where is the remaining Strix decode excess, and is the biggest class (KDA projections) fixable?

**Setup.** nimo, gfx1151, worktree `maya-kda-gap`, branch `exp/kda-gap-gfx1151` from everyday base `nimo/next2` `3fca883`, TheRock ROCm 10.2, `RAM_GB=8`, all 12096 experts resident (99.2% VRAM hit, disk 0), frozen usage, greedy. Per-class profile `STRATA_GLM_PROF=1` + `NO_SPEC=1`, one 1.6K prompt, 49 steps; `rocprofv3 --kernel-trace`; `glm_mv_bench` isolated and in situ with `STRATA_GLM_MV_RDNA=0`; receipts before/after.

**Result.** Token 54.09 ms GPU-stream (`glm stat` 55.44, 18.04 tok/s). Ranked excess over the 229 GB/s floor (ms/token): MLA 4.290, KDA+SSM 2.819, hc 1.192, MoE gate_up 1.146, router+shared 0.943, MoE route 0.816, MoE down 0.734, fetch/wait 0.582, DSA indexer 0.578, DSA projections 0.519, dense FFN 0.114. KDA: `kda_proj` 12.914 ms / 2896.7 MB = 224.3 GB/s (98.0%), `kda_out` 4.378 / 935.9 = 213.8 (93.3%), `kda_prep` 1.898 @ 82.8, `kda_rec` 1.051. Isolated `glm_mv_bench`: `kda_proj` 375.6 us / 226.9 GB/s vs RDNA-off 393.7 / 216.4, `kda_out` 125.9 / 218.6 vs 132.8 / 207.2; all 37 rows/unroll/waves/LDS variants stay below 1.15x (best 1.055x). `STRATA_GLM_MV_RDNA=0` costs 3.4% (55.93 vs 54.09). `rocprofv3`: 606.3 launches/token, 53.01 ms busy of 55.79 wall = 2.78 ms gap (5.0%). Fresh baseline 4-type mean 22.2 tok/s (n=2/cell). `glm_layer_parity`, `glm_model_test`, `glm_mv_bench --parity-only` PASS.

**Decision.** CLOSED-no-gain for the KDA GEMVs (already at 98% of the ceiling, no 1.15x variant); everyday config unchanged (keep `STRATA_GLM_MV_RDNA` default). Next lead MLA, then MoE gate_up / router.

**Caveats.** Strix only, one APU, one profile; end-to-end baseline n=2/cell. Report: `results/ds-nimo-kda-gap.md`, evidence `results/ds-nimo-kda-gap/`.

### E88 - Strix (gfx1151) decode MLA attention split into kernels: split-K retune and hierarchical combine (CLOSED-no-gain: `MLA_CHUNK` 32->16 gives split 1.46x/1.54x and with a hierarchical combine MLA 5.670 -> 5.116 ms (-9.8%), but NO_SPEC decode moves only -0.5%, below the 2% gate)

**Question.** Which MLA kernel carries the class excess, is it bandwidth- or latency-bound, and can a parity-clean change reach the 1.15x kernel / 2% e2e gates?

**Setup.** nimo, gfx1151, worktree `maya-mla-decode`, branch `exp/mla-decode-gfx1151` from everyday base `nimo/next2` `3fca883`, TheRock ROCm 10.2, `RAM_GB=8`, wmma2, frozen usage, greedy; `rocprofv3 --kernel-trace`, 120 decode tokens, `NO_SPEC=1`, after 1.6K / 32K / 64K. Dims: n_head 64, qk_nope 256, kv_lora 512, v_head 256, 11 DSA layers, `MLA_CHUNK=32`, `MLA_HPB=16`.

**Result.** Base 1.6K: `headwise_gemv` k_b 0.994 ms @ 185.7 GB/s, v_b 1.557 @ 118.6, `mla_split` 2.753 @ 27-29, `mla_combine` 0.366 @ 211.8, MLA total **5.670**; 32K 6.582; 64K 6.712. The kda-gap 62.5 GB/s was 369 MB / whole-class time, not a kernel rate; the two BF16 absorb matrices are 2.551 ms = 145 GB/s aggregate. `mla_split` is latency-bound: 34.8 KB of the 64 KB LDS/CU (40 CUs) = 1 block/CU, grid (53,4) = 212 blocks. `MLA_CHUNK` 32->16: split **1.891 (1.46x)** at 1.6K / **2.299 (1.54x)** at 32K, but combine grows 0.366 -> 1.024; adding a hierarchical combine (8 chunk-groups/head + merge, same online softmax) gives combine **0.669** and MLA **5.116** (**-0.55 ms, -9.8%**), `glmf` busy 53.68 -> 53.38 (-0.30, -0.56%), NO_SPEC decode 55.52 -> 55.25 (-0.5%). chunk8 regressed (MLA 6.716/8.721); the `mv_rdna` BF16 headwise path is 1.00-1.02x, below gate. Spec-on e2e is unusable: decode flips 19.2 <-> 22 tok/s tracking MTP acceptance (base 58.1% vs chunk16 67.8%, a rounding change). `glm_layer_parity`, `glm_model_test`, `glm_mv_bench --parity-only` PASS.

**Decision.** CLOSED-no-gain for the >=2% e2e gate; everyday config unchanged; keep the split/combine co-design as a foundation for a graph/fused follow-up. Not shipped.

**Caveats.** Strix only; deterministic NO_SPEC -0.5%; spec-on numbers are not a gate. Report: `results/ds-nimo-mla-decode.md`, evidence `results/ds-nimo-mla-decode/`.

### E89 - Strix (gfx1151) PR #96 single-GPU MTP speculation: config-knob sweep (CLOSED-no-gain: keep the everyday default `DRAFT=3` + ADAPT; MTP is +17.7% over NO_SPEC (19.19 -> 22.57 tok/s) and adaptive beats fixed 2/3 by 6-9 points, but no arm clears the pre-registered bar)

**Question.** Can any single MTP config knob beat the everyday default past the pre-registered bar (>=3% on the 4-type mean and >2x the default run-to-run spread)?

**Setup.** nimo, gfx1151, same everyday binary `maya-next2/build-rocm10/strata` exe `0fb53741`, scratch copies of `maya-nimo-next.json` (config sha `73cf17b3`, never edited), `RAM_GB=8`, wmma2, greedy; 11 arms, each a fresh `serve.server` + receipt + preflight + copied usage profile, 3x256 warm-up, then chat/code/reasoning/prose x2 x256 plus one 256-token decode after ~33K; per reply parsed the engine's `glm stat` / `glm spec` / `glm mtp` lines.

**Result.** 4-type mean tok/s: NO_SPEC **19.19**, default **22.57**, DRAFT=1 22.52, DRAFT=2 22.56, DRAFT=4 22.54, DRAFT=5 20.05, D2/ADAPT=0 21.23, D3/ADAPT=0 20.43, FIXED=1 22.81, FIXED=2 21.20, default2 22.57. MTP-on is **+17.7%** over NO_SPEC (1.177x). ADAPT settles on 1 draft (verify 71.8 ms/round, acceptance 63.6/75.3/84.8/69.3% chat/code/reasoning/prose); fixed 2/3 lose 6-9%; DRAFT=5 locks ADAPT onto length 3 (46% accept, verify 107.6 ms/round, 2.38 tok/round) -> -11.2%, 17.3 on 32K. Default chat 6 runs [21.4, 21.3, 18.7, 22.1, 22.2, 22.2]; the 18.7 is a post-32K length-3/0 transient, 5 settled runs stdev 0.40 / range 0.90; default2 reproduces the 4-type mean to 0.0%. FIXED=1 +1.1% fails the 2x-spread / 3% bar. Greedy: default2 2/2 all types, NO_SPEC differs all, others cross-layout.

**Decision.** CLOSED-no-gain for config tuning; everyday config unchanged. DRAFT=2 is only a stability guard at -0.9 tok/s on 32K.

**Caveats.** Strix only; 11 arms measured 03:51-04:45 with no errors; everyday server restored, `/health` ok. Report: `results/ds-nimo-mtp-sweep.md`, evidence `results/ds-nimo-mtp-sweep/`.

### E90 - Strix (gfx1151) PR #96 MTP verify-round cost split: why C2/T = 1.3-1.4 (MEASURED: a T=2 verify round costs 1.43x a normal step (78.2 vs 54.7 ms, 1.6K prompt, FIXED=1, 84% accept); routed MoE and MLA run per row, and a fused union read plus shared-down once recovers ~4.7 ms/round -> C2/T 1.34 (+6% decode))

**Question.** Why does a T=2 MTP verify round cost 1.3-1.4x a normal step, and where is the removable excess?

**Setup.** nimo, gfx1151, `maya-next2` exe `0fb53741`, config sha `73cf17b3`, `RAM_GB=8`, wmma2, greedy, one 1.6K-prompt request; fresh server + receipt + preflight per arm. Arms: normal/verify (`STRATA_GLM_PROF=1`, T=1 vs FIXED=1 T=2), the pair without the profiler, `STRATA_GLM_ROUTE_LOG`, and `rocprofv3 --kernel-trace` for both. No code changed.

**Result.** NO_SPEC normal 54.7 ms/step (18.3 tok/s); FIXED=1 verify **78.17 ms/round** (139 rounds, 84.1% accept, 1.84 tok/round) -> C2/T **1.429** (the sweep's 1.31 is at shorter prompts). Per class normal -> verify (ratio): MoE gate_up+shared down 8.745 -> 17.185 (1.97), MLA+indexer 6.539 -> 13.124 (2.01), MoE down 4.813 -> 9.106 (1.89), KDA+SSM 20.255 -> 22.132 (1.09), route 0.814 -> 1.346 (1.65), router+shared 3.946 -> 4.449 (1.13), DSA projections 5.226 -> 5.504 (1.05), hc 1.513 -> 1.623 (1.07), dense FFN 1.737 -> 1.678 (0.97), head 2.532 -> 2.264 (0.89), fetch/wait 0.500 -> 0.460 (0.92). Ranked excess: routed MoE +13.2, MLA +6.6, KDA +1.9, router +0.5, DSA +0.3 ms. `fast_moe_rows` runs routed experts per row (`glm_mtp.cu:523-540`, 2x8, no union) and reads shared-down once per row (`glm_fast.cu:2818`); `STRATA_GLM_ROUTE_LOG` over 33 T=2 rounds x 42 layers: union 13.671/16 (14.6% overlap) => routed 4779 -> 4084 MB, shared-down 578 -> 289 MB. `mv_rows` reads weights once for NT rows (`glm_fast.cu:1022-1026`); `glm_mv_bench` nt=1 -> nt=2 1.01-1.08x (bit-exact). Counterfactual: routed union + shared-down once -4.7 ms => C2/T 1.343 (+6% decode); + batched MLA k_b/v_b (2.551 ms) => 1.297 (+9%); the literal matched-single-row 1.223 is not physical. `perf/two-row-decode-1gpu-spec` `MoePlan2` unions plan/fetch but `moe_gate_up2_kernel` still reads weights per row, so on all-resident Strix it captures almost none of the 696 MB.

**Decision.** MEASUREMENT ONLY (no kernel change). Recommend a truly fused union read (idea B) reusing the union plan, gated on NO_SPEC busy-ms; cheaper first step is batching MLA k_b/v_b. Everyday server restored, `/health` ok. Do not post.

**Caveats.** Strix only; 33-round route log; one 1.6K prompt. Report: `results/ds-nimo-verify-cost.md`, evidence `results/ds-nimo-verify-cost/`.

### E91 - Strix (gfx1151) PR #96 verify round: batched MLA `k_b/v_b` (A) + fused union routed MoE (B) (KEPT: A+B cuts the T=2 verify round 78.09 -> 73.94 ms, C2/T 1.43 -> 1.35, bit-identical with no NO_SPEC regression)

**Question.** E90 predicted that batching the absorbed MLA `k_b`/`v_b` GEMVs and fusing the routed MoE union read would remove ~4.7 ms from the T=2 MTP verify round. Do the two bounded prototypes actually deliver it?

**Setup.** nimo, gfx1151, worktree `maya-verify-fuse` (branch `exp/verify-fuse-gfx1151`), base `nimo/next2` `3fca883`, commit `d119b72`, exe `6136fe7d`; TheRock ROCm 10.2, `RAM_GB=8`, wmma2, greedy; fresh server + `receipt.sh` + preflight per arm, one 1.6K-prompt request, FIXED=1 (139 rounds, 83.5% accept, 1.84 tok/round, the same prompt for every arm); scratch config copies only (`maya-nimo-next.json` sha `73cf17b3` untouched). (A) `mla_kb_rows`/`mla_vb_rows` read each absorbed `k_b`/`v_b` weight row once and dot every row's activation (`rows_dot_bf16_ptrs`), with the attention split out as `mla_attention`. (B) `fast_moe_rows` routes both rows, saves the plans, builds a device `MoePlan2` union, and fused union gate/up (IQ2_XXS) and down (IQ3_XXS) kernels read each union expert's weight once for both rows; the shared expert's down rides the union gate/up launch, and row 1 gets its own 8 scratch slots.

**Result.** Verify round: base 78.09 ms, p0 (in-binary per-row) 78.13, **A** 75.61 (-2.52), **B** 76.48 (-1.65), **A+B** **73.94** (-4.19 vs p0). `glm stat` decode 45.37 -> 44.03 / 44.52 / **43.14** ms/token; C2/T 1.428 -> **1.352**; NO_SPEC step flat (54.70 -> 54.71). `v_mla` profile 13.048 -> 10.402. B nets -1.65, not the predicted -4.7: the union read saving is partly offset by the union kernels, and reading the shared down once was traded for overlapping it in the union gate/up launch. Default-config e2e (informational) 4-type mean 22.70 -> **23.68** (+4.3%), 33K +5.4%; acceptance matches exactly on the four short types. Parity PASS: `glm_layer_parity`, `glm_model_test`, `glm_mv_bench --parity-only`, `hip_expert_kernel_bench --parity-only`; first-window logits + head hidden byte-identical (max_abs 0) for p0 vs A vs A+B; greedy FIXED=1 base vs A+B identical text on chat/code/reasoning x128.

**Decision.** KEPT: A+B clears the >=3 ms gate with no NO_SPEC regression and is bit-identical, so switch the everyday Strix build to this branch after shared-code validation. Offer A upstream as a clean follow-up to #96; offer B separately (new kernels + the 16-slot scratch) and record that it nets -1.65 ms, not -4.7. Do not post.

**Caveats.** Strix only, gfx1151 and the Maya-S IQ2_XXS/IQ3_XXS pack; one 1.6K prompt; `v_moe_rows` stream intervals drift ~2 ms so they cannot resolve B. Report: `results/ds-nimo-verify-fuse.md`, evidence `results/ds-nimo-verify-fuse/`, nimo `~/verify-fuse-2026/`.

### E92 - nimo everyday: verify-fuse build adopted (ADOPTED: `maya-nimo-next.json` now runs the verify-fuse exe `6136fe7d`; default decode +4.3%, 33K +5.4%)

**Question.** Should the nimo everyday server switch to the verify-fuse build (E91)?

**Setup.** nimo, gfx1151; `maya-nimo-next.json` exe/cwd -> `maya-verify-fuse` (`exp/verify-fuse-gfx1151` `d119b72`, exe `6136fe7d`); `wmma2` and `RAM_GB=8` kept; backup `maya-nimo-next.json.bak-preverifyfuse`.

**Result.** The verify round gains batched MLA `k_b/v_b` and fused union routed MoE; bit-identical, verify 78.1 -> 73.9 ms; default decode +4.3% (33K +5.4%); parity PASS (E91).

**Decision.** ADOPTED.

**Caveats.** The branch is gfx1151 plus the Maya-S pack, so shared-code changes still need R9700 validation later. Report: `results/ds-nimo-verify-fuse.md`.

### E93 - ROCmFPX TheTom PRs #37-#52 mapped to Maya GLM-5.3 prefill (LEAD: none of the 16 PRs targets GLM; only #44 lands on an identical Maya kernel)

**Question.** Do TheTom's ROCmFPX #37-#52 (Qwen3.8-Flash-Next HC-Q8 on Strix Halo) contain prefill levers transferable to Maya GLM-5.3, and do they explain the flat-to-128K claim?

**Setup.** Public-source research only: `gh pr view`/`gh pr diff` for ROCmFPX/ROCmFPX #37-#52 (merged 2026-10-09/10) plus Maya v1.0.32 source at `/ai/github/maya-main`. No GPU, build, server, request, post or push.

**Result.** All 16 PRs are qwen4exp / Qwen3.8-Flash-Next HC-Q8 on gfx1151 + ROCm 7.2.1 (plus one CUDA build fix, #47); **none** targets GLM-5.3, Gorgon 8065S, or the GLM KDA/DSA path. The only gated-delta-net PR (#45, chunked GDN WMMA, C=16) dispatches scalar gate only and explicitly falls back for KDA/vector gates — which is what Maya's GLM KDA is (`glm_batch.cu:280`). Maya's KDA prompt recurrence is serial per token (`kda_rec_kernel`, one block/head, F32 FMA) and `kda_rec` is only 4.9-5.0% of Strix prefill. Only **#44** lands on an identical Maya kernel: the vendored `ggml-cuda/quantize.cu` `quantize_mmq_q8_1` still grids token=`blockIdx.x` / chunk=`blockIdx.y`, called per MoE layer from `prefill/moe_mmq.cu:117`; #44 makes the chunk fastest (author +1.6..4.6% at ub2048-8192). HC fusions #43/#48/#50 are largely already present in Maya (`hc_update`+`hc_finish` fuse combine/gates/Sinkhorn/joint RMS); #48's Q8_1 copy is N/A (Maya dense is f16, not MMQ); #41's BF16-WMMA 4-bit path matches Maya's prior negative (E57/E60). The X-post flat-to-128K curve is not explained by these PRs: ours drops ~8-10% via the DSA indexer (`glm_dsa_select` O(n_vis^2)) while qwen4exp uses a bounded QSA window (#51). Author gains: #44 ub8192 +4.6%, #45 pp4096 ub1024 +3.7%, #48 ub8192 +3.6%, #50 ub8192 +3.8%, #49 decode +10.6%, #52 decode +11.4%.

**Decision.** Record as LEAD (public source map). Ranked Maya-applicable levers: (1) port #44's chunk-fastest `quantize_mmq_q8_1` — exact same vendored kernel; (2) #45-style chunked KDA recurrence — high effort, <=5% cap, needs a vector-gate derivation; (3) #37-style few-row path for the N=24 HC mix GEMM; (4) #43/#50 HC fusion audit. Confirm each with `STRATA_GLM_PROF` prefill at 8K/32K/64K plus in-kernel time, not with author numbers on a different model/GPU. Do not credit #37-#52 for the GLM flat-to-128K claim.

**Caveats.** No GLM-5.3/Gorgon model among the PRs; author numbers not independently reproduced; the X-post context axis is not a proven prompt-token count (E68); #45 would need a new vector-gate derivation and parity; the #44 fix is a vendored dependency patch. Report: `results/ds-rocmfpx-tom-map.md`.

### E94 - Strix (gfx1151) prefill push: measured f16 ceiling, per-phase excess and three levers (CLOSED-no-gain: the assumed 59 TFLOPS is 2.4x too high; no lever clears +3%)

**Question.** What is the real Strix prefill ceiling, where is the per-phase excess, and can a sub-batch change, a hipBLASLt retune or a MoE kernel change clear the +3% gate?

**Setup.** nimo, worktree `maya-prefill-push` (branch `exp/prefill-push-gfx1151` `d119b72` = everyday verify-fuse exe `6136fe7d`), TheRock ROCm 10.2, `RAM_GB=8`, wmma2, greedy, copied pristine usage profile; every arm a fresh server + receipt + preflight, 2 runs, `cache_n=0`.

**Result.** (1a) Prefill `sclk` median **2458 MHz** at 8K / **2469** at 32K (min 1685, max 2754; package power median **85 W**, max 116 W). Measured hipBLASLt f16 GEMM peak **24.17** (8192³) / **24.24** (12288³) / 20.05 TFLOPS (16384³) at ~2868 MHz, so the assumed 59 TFLOPS is 2.4x too high; the prefill-clock ceiling is ~20.9 TFLOPS. (1b) Fresh profile `cache_n=0`: 8K 8321 tok / 23.71 s / 350.9 t/s; 32K 33177 tok / 96.84 s / 342.6 t/s; 1181 TFLOP at 32K = 12.2 TFLOPS = **50% of the measured peak** (59% of clock-scaled). 32K excess (ms): `moe_fused` 17629 (32.2%, 18.1 TF [E-i8]), `dsa_attn` 10144 (15.2%, 6.5), `hc` 7123 (7.5%), `dsa_proj+dsa_qabs+dsa` 5985 (10.8%, 7.4), `kda_proj+out` 4491 (20.1%, 16.0 = 77% of clock peak), `kda_rec` 4502 (4.9%), `dsa_index` 1703, `route` 1548, `shexp` 1106, `dense_ffn` 292. (1c) hipBLASLt 72 rows load, 1015 (8K)/3049 (32K) launches, fallbacks 0, 26 keys; T=4096 dispatches for N=2048/K=16384, N=2730/K=12288 and N=1366/K=12288 use the stale 2048-bucket solution 2541, **1.44/1.57/1.23x** slower than 2542. (2) Levers, 2 runs each: base 8K 351.1, 32K 340.8, decode 22.36; `PREFILL_SUB=8192` + `PREFILL_MB=24576` 8K 354.4 (+0.95%), 32K 346.6 (**+1.72%**), decode 22.28; retuned table (84 rows, +12 T=4096 rows all 2542) 8K 353.4 (+0.67%), 32K 343.5 (+0.79%), decode 22.46. Neither clears +3%. MoE int8 kernel microbench best 1.056-1.065x < 1.15x gate. Parity PASS (`glm_layer_parity`, `glm_model_test` 2.56e-6/0/3.10e-6, `hip_prefill_fused_parity` 24 pairs, `hip_prefill_mmq_parity`).

**Decision.** CLOSED-no-gain for both tested levers. The dominant finding is the ceiling: the engine is at ~50% of the measured peak, not 21%, and the dense GEMM classes are already at 77% of the clock-scaled ceiling. `PREFILL_SUB=8192` + `PREFILL_MB=24576` is a consistent partial (+1.72% at 32K, +0.95% at 8K, decode flat) and the recommended candidate for a future lower-bar or combined build. The largest-excess lever (routed int8 MoE) gate-failed on the microbench. Everyday config unchanged; committed `830f909`; server restored, `/health` ok; no push.

**Caveats.** Strix only; 2 runs per arm, `cache_n=0`; the MoE excess column assumes int8 = 2x f16. Report: `results/ds-nimo-prefill-push.md`, evidence `results/ds-nimo-prefill-push/`, nimo `~/prefill-push-2026/`.

### E95 - single R9700: PR #103 trunk keep (MEASURED: +6..+16% decode, but KL 0.025-0.062 exceeds the 0.02 gate; no everyday MAP)

**Question.** Does PR #103 trunk keep (`STRATA_GLM_TRUNK_KEEP`) give a usable single-R9700 decode gain at acceptable quality on Maya-S and Maya-M?

**Setup.** `/ai/github/maya-v1033` at `528d2b4` (v1.0.33-2, the #103 merge; `origin/main` has since advanced to v1.0.34/#108/#110, so the merge was pinned), `build-multi` Release gfx1100+gfx1201, ROCm 7.2.1, tests on; HIP device 1. Config = copy of `maya-r9700-long-7900.json` (`RESERVE_MB 512`, `RAM_GB 90`, `PROMOTE_MIN 6`, INT8 KV, 131072) with exe/cwd -> `maya-v1033`; 82 slots/layer, RAM tier 63.09 GB. Every arm a fresh server + frozen copied usage + receipt + preflight, 3x256 warm-ups, 4 types x2 x256 greedy, then one 32.2K prompt + 256 greedy. KL: CPU lane off (`STRATA_GLM_CPU_LANE=0`), 1024-token held-out prose/code texts, first 32 unscored, vs the unset full model; unset self-repeat KL 0.000000. Maya-M repeated via a copy of `maya-r9700-m-7900.json`.

**Result.** Maya-S steady decode unset 26.43 (mean 25.34): upstream MAP mean **27.76** (+9.55%) / steady 28.77 (+8.85%), KL 0.03832/0.03772 (mean 0.03802), top-1 91.3/92.6%; milder MAP 27.21 (+7.38%) / 28.25, KL 0.02485/0.02521 (mean 0.02503); stronger MAP **28.45** (+12.27%) / 29.38, KL **0.06413/0.05887** (mean 0.06150), top-1 87.5/89.3%; uniform `KEEP=7` 26.91 (+6.20%), mean KL 0.05803. Decode after 32.2K: unset 21.8, upstream 24.9, milder 23.3, stronger 25.4, `KEEP=7` 23.5; prompt 791-794 flat. v1.0.33 unset 26.43 vs v1.0.32 26.66 (-0.9%, in noise). Eval, same 178 items as `ds-r9700-m-vs-s`: unset 86.5%, upstream **88.2%** (+1.7 pp), stronger **88.8%** (+2.2 pp) — did not drop. Maya-M: unset 21.68 mean / 22.73 steady, upstream +14.11%/+12.27%, stronger +16.37%/+14.47%.

**Decision.** Do not set `STRATA_GLM_TRUNK_KEEP` on the everyday R9700 config: the speed gain is real (+6..+16%) but KL 0.025-0.062 exceeds the 0.02 gate; eval did not drop, so KL is the only blocker. Optional faster-than-exact mode only. Everyday `r9700-long` restored, `/health` ok.

**Caveats.** One run per arm; `skipped/tok` is derived and the keep arms' hit% is inflated (the stat counts a skipped expert as a VRAM hit); single gfx1201 card. Report: `results/ds-r9700-trunk-keep.md`, evidence `results/ds-r9700-trunk-keep/`.

### E96 - Strix (gfx1151) prefill: ROCmFPX #44 chunk-fastest `quantize_mmq_q8_1` on HIP (CLOSED-no-gain: kernel 1.35-1.46x and byte-identical, but the everyday fused path dispatches it zero times)

**Question.** Does porting ROCmFPX #44's chunk-fastest work order to Maya's vendored `quantize_mmq_q8_1` speed up Strix prefill?

**Setup.** nimo, worktree `maya-mmq-quant-order` (branch `exp/mmq-quant-order-gfx1151` `ae0c348`, base `d119b72`), TheRock ROCm 10.2, gfx1151, `RAM_GB=8`, wmma2, greedy; fresh server + copied pristine usage + receipt + preflight per cell, `cache_n=0` everywhere. Vendored `ggml-cuda/quantize.cu` (llama.cpp `3cf03257`) and ported the HIP remap (`flat = blockIdx.y*gridDim.x + blockIdx.x; row = flat/gridDim.y; col = flat%gridDim.y`).

**Result.** Q8_1 output byte-identical: 11 shapes/types, 319,297,536 B, base and patched raw dumps both sha256 `eb534822ffb4...`; `hip_prefill_mmq_parity` / `hip_prefill_fused_parity` / `glm_layer_parity` / `glm_model_test` all PASS. In-kernel `quantize_mmq_q8_1` (rocprofv3, MMQ path `PREFILL_FUSED=0`): 266.5 -> 197.8 ms at ~4168 tokens (**1.35x**), 591.8 -> 404.0 ms at ~8300 (**1.46x**). e2e MMQ fallback (2 runs): 8K 329.05 -> 328.30 (-0.23%), 32K 328.40 -> 330.25 (+0.56%), 64K 322.20 -> 324.90 (+0.84%). Everyday fused path (`rdna4::native_*`, quantizer `moe_fused_rdna4.cu:680 quant_act_nat_kernel`) dispatches `quantize_mmq_q8_1` **zero** times over a 4168-token prefill, so its e2e is flat: 8K 349.30 -> 349.55 (+0.07%), 32K 341.75 -> 341.15 (-0.18%), 64K 333.95 -> 334.45 (+0.15%). The kernel is only ~2% of the non-fused MMQ prompt, so the 1.35-1.46x kernel win is <1% e2e.

**Decision.** CLOSED-no-gain for the everyday path. Keep branch `exp/mmq-quant-order-gfx1151` (`ae0c348`) as the correct MMQ-path port; do not switch the everyday config; upstreaming is optional because it only affects the non-fused MMQ path. Everyday server restored (`maya-nimo-next.json`, exe `6136fe7d`, temp 0.0), `/health` ok. The second arm (few-row N=24 HC mix GEMM) was not attempted; `hc` is 7.5% of prefill (E94).

**Caveats.** Strix only; 2 runs per cell. A first dump showed an `ids!=null` mismatch caused by the harness's null-stream `hipMemcpy` racing a non-blocking stream; a device sync made base and patched identical and deterministic — not a kernel difference. Report: `results/ds-nimo-mmq-quant-order.md`, evidence `results/ds-nimo-mmq-quant-order/`, nimo `~/mmq-order-2026/`.

### E97 - Strix (gfx1151) prompt MoE expert-quant microbench: is IQ2_XXS dequant the prefill limiter? (CLOSED-no-gain: hypothesis 1h rejected; the bigger-but-simpler ROCmFPX-like pair is 1.07-1.13x slower)

**Question.** Is the everyday fused gfx1151 prompt MoE IQ2_XXS-dequant-bound, and would a bigger-but-simpler ROCmFPX-like quant pair (IQ3_S gate/up, IQ4_XS down) be faster?

**Setup.** nimo, worktree `maya-quant-prefill` (branch `exp/quant-prefill-gfx1151` `38cb924`, base `d119b72` = everyday exe `6136fe7d`), TheRock ROCm 10.2, gfx1151; synthetic GGML blocks at the real GLM geometry (E=4096, FF=2048), 16384 routed rows, tokens/expert 32/128/512/2048, 2 runs (ratios repeat to <0.6%). The fused #98 kernel gains a measurement-only skip-decode template and down IQ3_S/IQ4_XS dispatch; `hip_prefill_fused_parity` PASS on all 30 GU/down pairs. No server arm, no request; everyday server untouched.

**Result.** Fused combined ms, iq2_xxs/iq3_xxs vs iq3_s/iq4_xs: 52.38/59.16 (T=32), 35.26/38.62 (128), 34.30/36.83 (512), 33.93/36.19 (2048) -> ratio **1.129/1.095/1.074/1.067**, the bigger pair 7-13% slower. Per kernel (T=128, full/skip-decode ms): gate/up iq2_xxs 23.31/20.19 (decode 3.13), iq3_xxs 25.24/20.88 (4.36), iq3_s 26.87/21.26 (5.61), iq4_xs 26.42/21.48 (4.95); down iq3_xxs 11.05/10.21, iq3_s 11.52/10.47, iq2_s 11.85/10.70. Decode is 11-15% of the fused MoE phase; ~85% is the WMMA+load pipeline. MMQ gate/up+down at T=128: iq4_xs **30.71** vs iq2_xxs 33.96 / iq3_xxs 34.71 / iq3_s 34.56 / q4_K 34.52 (IQ4_XS 0.896x = ~+3.5% overall); at T=32 q4_K 43.01 vs 61.71-70.42. `sclk` median 2482 MHz (p90 2549, max 2654), 85 W, 69 C.

**Decision.** Hypothesis 1h REJECTED on the everyday fused path: expected prefill with IQ3_S/IQ4_XS experts is ~2-3% slower, because their decode is not cheaper and they read more bytes. A decode-once-per-expert reuse scheme is bounded by ~+4.0% overall and cannot close the 1.36x Gorgon yardstick (zeroing all MoE is <=1.475x; 1.36x from MoE alone needs 5.62x), leaving hypothesis 1g (clock/power/hardware, engine-wide effects). On the MMQ fallback the order flips (IQ4_XS ~+3.5% overall; Q4_K ~parity at T>=128, much faster at T=32), but MMQ only runs where the stock fused kernel is ineligible. Measurement only, no everyday change; everyday server untouched, `/health` ok.

**Caveats.** Synthetic random weight blocks, not the real pack; uniform tokens/expert; the skip-decode variant perturbs register pressure/scheduling for iq2_s gate/up and iq4_xs down, so those deltas are not clean; MMQ and fused comparisons are within-path only; single GPU. Report: `results/ds-nimo-quant-prefill.md`, evidence `work/ds-nimo-quant-prefill/`, nimo `maya-quant-prefill` `exp/quant-prefill-gfx1151` `38cb924`.

### E98 - quality bar for approximate speed levers (DESIGN: replace "KL <= 0.02 + <=1.5 pp on 178 items" with a divergence screen + behaviour canaries + speed gate)

**Question.** What is a defensible quality gate for approximate speed levers, replacing the borrowed `KL <= 0.02` plus `<=1.5 pp on 178 items` rule?

**Setup.** Research + design only, no GPU, server or model run; public literature/community anchors plus existing local eval JSONL and paired statistics.

**Result.** External anchors: Dutta 2024 (an accuracy delta <=2% hides flips up to 13.6%; KLD-vs-flips Spearman 0.981); llama.cpp LLaMA-3-8B WikiText-2 `q4_K_M` KLD 0.0313 / 91.9% same-top, `q5_K_M` 0.0108, `q3_K_M` 0.1019 / 71.1%, `q2_K` 0.4451; Unsloth `Q4_K_XL` 0.0237 / `Q3_K_XL` 0.0806; vLLM W4A16-GS32 0.0487 / GS128 0.073 / NVFP4 0.09-0.11; Nikolic 2026 (full-cohort rho(KLD, composite) -0.72/-0.86 but collapses to ~0 near baseline; silent-zone tau 0.064/0.027); Intel's default relative `tolerable_loss` 0.01; AMD/NVIDIA publish no numeric KL bar. MoE: Mixtral dropping 2/8 experts -3.4 pts, 4/8 -8.0 pts. Local paired stats: 8-16/178 discordant (4.5-9.0%); MDE@178 at 80% power 4.4-6.3 pp; N* 881-1764 for 2 pp and 3530-7062 for 1 pp; power@178 6.5-9.1% (1 pp) / 14.2-24.0% (2 pp); pairing ~2.15x vs unpaired; MMLU subject clustering can inflate N* ~31x. Proposal: **Class 0** exact -> parity + greedy token-id identity + KL exactly 0; **Class 1** approximate -> **Gate A** divergence screen (mean KL <= 0.05 adopt / > 0.10 hard fail, top-1 >= 90% / < 85%, p99 <= 25x mean; 8-16 x 1024-token held-out texts) + **Gate B** behaviour canaries (tool-call structural exactness n>=48, coding pass@1 + first-divergent-token, IFEval strict, NIAH 32/64/128K; the 178-item suite demoted to a 3 pp non-inferiority breakage guard) + **Gate C** speed. Under Gate A the PR #103 upstream MAP (KL 0.038, top-1 91.3-92.6%) passes and the stronger MAP (0.061, 87.5-89.3%) fails; Gate A is a screen only.

**Decision.** Adopt the two-gate bar as the proposal for approximate levers. The user approved a v2 merge on 2026-10-11 that supersedes the Gate A "adopt" thresholds: KL/top-1 become a diagnostic plus a gross-damage tripwire (reject above mean KL 0.10 or below top-1 85%; always report direction, reference, corpus, truncation), and acceptance moves to a paired task suite — 64 development cases plus 160 locked confirmation cases (40 tools, 40 coding+instruction, 40 long-context, 40 multi-turn), with reference-right->candidate-wrong and wrong->right counted separately, exact McNemar and prompt-level bootstrap CIs. Pass = repeatable speed gain AND zero reproducible critical right->wrong AND one-sided 95% CI of overall paired quality >= -2 pp AND no domain worse than -3 pp; inconclusive -> more cases, not pass. The 178-item weighted score is demoted from a loss gate to a gross-breakage guard. No GPU, server, model run, post or push.

**Caveats.** Research+design only; thresholds are anchored to external quants and local evidence, not independently revalidated on Maya; KL is not comparable across models, corpora, tokenizers or implementations; p99 per-token KL is not yet emitted by `generate.cpp`; McNemar N* assumes IID items, and MMLU subject clustering can inflate it. Report: `results/ds-quality-bar.md`; GAP-REGISTER.md "QUALITY BAR ADOPTED" and "QUALITY BAR v2".

### E99 - dual GPU (R9700 + RX 7900 XT) Maya-S retune on v1.0.33 (MEASURED: decode 35.80 -> 40.30 tok/s (+12.6%), 32K prompt 1049.6 -> 1128.9 (+7.6%))

**Question.** Can the dual-card v1.0.32 config be retuned on v1.0.33, and which knobs actually help?

**Setup.** R9700 + RX 7900 XT, Maya-S, v1.0.33 `build-multi` (`maya-v1033` `528d2b4`), `gpu [1,0]`, auto layer split K=24 ([0,24) R9700 / [24,45) 7900 XT), MTP block on the 7900 XT, greedy. Frozen copied usage + receipt + preflight per arm, 3x256 warm-ups, 4 types x2 x256 greedy, then ~8K and ~32K prompts (`cache_n=0`). 17 arms, one change at a time.

**Result.** Current v1.0.32 config decode **35.80**, 8K 696.5, 32K 1049.6, decode-after-32K 34.2, accept 77.8%, 172/82 slots; v1.0.33 base is a wash (35.91 / 695.1 / 1048.3). Winners kept cumulatively: `RESERVE_MB 3072->1024` (**+8.1%** decode, prompt flat, 186/96 slots); `PREFILL_ATTN=wmma2` (+2.9% 8K / +5.7% 32K; only the 7900 XT changes, gfx1201 already defaults to it); `PREFILL_SUB 2048` / `PREFILL_MB 8192` (32K +2.5%); `KV_INT8=1` (+3.7% decode on top, -0.7% prompt, 189/98 slots). Best exact config decode **40.30** (repeat 40.21), 8K **721.4**, 32K **1128.9**, decode-after-32K **38.2**, accept **80.2%**: +12.6% decode / +3.6% 8K / +7.6% 32K vs the current config. Versus one R9700 everyday (26.66 / 804 / 789): +51% decode, +43% at 32K, but -10% at 8K (a single 8K chunk cannot overlap the two cards). Rejected: `RESERVE_MB 512` (-4.3%, unstable, 190/99 slots); split K=22 (-7.2%); split K=26 (+3.6% decode alone but 32K -3.1..-3.7% on top, fails the no-harm gate); `MV_RDNA=1` (+1.4% < 2% gate); 262144 ctx (-4.5% decode); trunk keep not shipped (`ds-r9700-trunk-keep` recommends against it, KL > 0.02).

**Decision.** Ship `project-maya/maya-2gpu-best-7900.json` = v1.0.33 `build-multi` (`528d2b4`) + `RESERVE_MB 1024` + `PREFILL_ATTN=wmma2` + `PREFILL_SUB 2048`/`PREFILL_MB 8192` + `KV_INT8=1`, 131072 ctx, auto split, temp 0.0; server left running (`start7900.sh 2gpu-best`, `/health` ok). `project-maya` untouched.

**Caveats.** The serve fast path pipelines one token per round, so MTP tokens/round is ~1.008 by construction (accept = a draft hit avoiding a head redo), not the batch-verify 1.75; MTP off vs on was not measured; single-arm deltas below ~2% are within the CPU-lane calibration noise. Report: `results/ds-dual-v1033.md`, evidence `results/ds-dual-v1033/`.
