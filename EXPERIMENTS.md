# Experiment log

What was tried on the AMD port of Project Maya (GLM-5.3-Flash on ROCm/HIP), what happened, and what was decided. It is
built only from records that exist: the upstream PR descriptions and comments, the benchmark folders, and the agent
reports. Where a number is a single run, a small run, or noisy, the entry says so. Where no record exists, the entry says
that instead of filling the gap.

Last updated 2026-10-10; E49-E69 consolidate the current working records (source, CPU, build and GPU audits), and E70-E77 record the v1.0.32 everyday measurements. A machine-readable copy is kept next to the benchmark data (`experiments.jsonl`, same IDs).

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
| E84 | Prompt speed | gfx1151 prompt attention: wave32 WMMA (wmma2) vs f16q | KEPT-OPEN |

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
