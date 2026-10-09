# Experiment log

What was tried on the AMD port of Project Maya (GLM-5.3-Flash on ROCm/HIP), what happened, and what was decided. It is
built only from records that exist: the upstream PR descriptions and comments, the benchmark folders, and the agent
reports. Where a number is a single run, a small run, or noisy, the entry says so. Where no record exists, the entry says
that instead of filling the gap.

Last built 2026-10-09; E40-E44 added in the hand-off refresh. A machine-readable copy is kept next to the benchmark data (`experiments.jsonl`, same IDs).

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
| E03 | Prompt speed | wmma2 prompt attention on RDNA4 | KEPT-OPEN |
| E04 | Prompt speed | Fused WMMA prompt MoE on Strix Halo | OPEN |
| E05 | Prompt speed | Strata's fused int8 WMMA prompt MoE on discrete cards | OPEN |
| E06 | Prompt speed | llama.cpp RDNA4 MMQ patch | DROPPED |
| E07 | Prompt speed | ROCm 10.2 (TheRock nightly) on Strix Halo | KEPT |
| E08 | Prompt speed | amd_iommu=off on Strix Halo | KEPT |
| E09 | Prompt speed | --prefill auto (PR #44) on Strix Halo | OPEN |
| E10 | Prompt speed | Why 256-token prompts are slow | DROPPED |
| E11 | Prompt speed | Terminal-layer prefix skip in single-GPU prefill | KEPT-OPEN |
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
| E30 | Stability | Root causes of the illegal memory access, and the fix (#39) | KEPT-OPEN |
| E31 | Stability | Rebasing #38 and #39 onto v1.0.21 and v1.0.23 | KEPT-OPEN |
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

**Evidence.** [#7](https://github.com/mw00/project-maya/pull/7); local evidence: /ai/github/Maya-data/benchmarks/20261007-7900xt-ram-comparison/ (ram90*, summary.json per arm)

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

**Evidence.** [#16](https://github.com/mw00/project-maya/pull/16); local evidence: /ai/github/Maya-data/benchmarks/20261008-v1011-prompt2/p2/summary.json (7900 XT: 431.6 tok/s at 4K, n=2; 275.8 at 2K, n=1)

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

**Evidence.** [#38](https://github.com/mw00/project-maya/pull/38); [#41](https://github.com/mw00/project-maya/pull/41); local evidence: /ai/github/Maya-data/agent-tools/results/sol-rebase-38-39.md; local evidence: /ai/github/Maya-data/agent-tools/results/muse-local-integration.md

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

**Decision.** Fixed and measured on Strix (gfx1151). Not yet PR'd: the commit needs a rebase onto current upstream. The gfx11 runtime gate admits gfx1151 only. The RX 7900 XT (gfx1100) keeps MMQ; enabling it needs a gfx1100 kernel image, an allowlist entry, and its own parity and A/B runs on that card.

**Caveats.** Three repeats per size, run sequentially, with no confidence interval; prompt sizes differ from the earlier run. Greedy text is not byte-identical between ON and OFF (floating-point and quantization differences). The coherence checks are smoke tests, not a quality evaluation. No gfx1100 or gfx12 hardware run.

**Evidence.** local evidence: /ai/github/Maya-data/agent-tools/results/sol-fused-gfx11.md; local evidence: /ai/github/Maya-data/agent-tools/results/muse-fused-nimo.md (earlier run)
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

**Evidence.** hub README.md and NOTES.md; coordinator session notes, 2026-10-08/09; local evidence: /ai/github/Maya-data/agent-tools/results/muse-fused-nimo.md

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

**Evidence.** [#44](https://github.com/mw00/project-maya/pull/44) (our comment); local evidence: /ai/github/Maya-data/agent-tools/results/muse-nimo-pr41-44.md

### E10 - Why 256-token prompts are slow (DROPPED)

**Question.** Is short-prompt speed (51-88 tok/s at 256 tokens vs 416-495 at 4K) caused by looping over experts that got no tokens?

**Setup.** AMD Ryzen 7 9800X3D, 192 GB DDR5, RX 7900 XT 20 GB (gfx1100); R9700 + 7900 XT for the 2-GPU numbers; v1.0.11 + local shadow branch (5393fe7). Code reading of the prompt MoE path.

**Result.** No gap found to remove. Prompt speed at 256 tokens stays 51 tok/s (1 GPU) and 88 (2 GPUs).

**Decision.** Suppressed as MG-S005: zero-token experts are not fetched or computed; at 256 tokens nearly every expert gets rows. Remaining cost is system-level (one host sync per MoE layer, staging bytes).

**Caveats.** Source review, no profiler timeline. The per-layer sync cost was not measured.

**Evidence.** local evidence: Maya field-study lead ledger (LEADS.md) / SUPPRESSIONS.md; local evidence: /ai/github/Maya-data/benchmarks/20261008-v1011-2gpu/summary.md

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

**Evidence.** local evidence: /ai/github/Maya-data/agent-tools/results/codex-mgl005.md
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

**Evidence.** local evidence: /ai/github/Maya-data/agent-tools/results/muse-r9700-subsweep.md; receipts under /ai/github/Maya-data/agent-tools/results/receipts/ (muse-sub1024, muse-sub2048, muse-sub4096)

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

**Evidence.** local evidence: /ai/github/Maya-data/agent-tools/results/luna-rdna4-experts.md; local evidence: /ai/github/Maya-data/agent-tools/briefs/luna-rdna4-experts.md
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

**Evidence.** local evidence: Maya field-study lead ledger (LEADS.md) (MG-L004); local evidence: /ai/github/Maya-data/benchmarks/20261008-v1011-2gpu/

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

**Evidence.** local evidence: Maya field-study lead ledger (LEADS.md) (MG-L001, MG-S002); local evidence: /ai/github/Maya-data/benchmarks/20261008-v1011-2gpu/2gpuprof

### E18 - Single-GPU speculation on Strix Halo (OPEN)

**Question.** Would MTP speculation on one card raise decode? Measure its acceptance and draft cost first.

**Setup.** Strix Halo 8060S (gfx1151, 128 GB unified), one GPU; upstream v1.0.23 (eadd3b6) with an opt-in probe (STRATA_GLM_MTP_PROBE=1; commit 718eb10 on exp/mtp-accept-1gpu), ROCm 10.2 build. Four greedy 300-token continuations (chat, code, reasoning, prose) with probe on and off; context 65536; NextN loaded; a fresh engine for each run.

**Result.** Greedy acceptance of the NextN draft: 900 of 1,196 comparisons, 75.25% overall (chat 72.2%, code 78.3%, reasoning 86.6%, prose 63.9%). Draft 3.82 ms against a normal decode step of 54.38 ms (draft overhead 7.0%). All 1,200 probe token IDs match the probe-off run. The two-token verify has not been built or timed. Cost model: if a two-token verify costs 1.2x a normal step, the estimated speedup is 1.38x. Break-even for the verify is 1.68x a normal step (about 91.5 ms). Two serial forwards would give about 0.85x.

**Decision.** OPEN. Worth a K=1 verifier prototype if the two-token verify can share weight reads (dense and routed-expert reads). The prototype is being built; there is no speed result yet.

**Caveats.** Four prompts, one 300-token continuation each: a feasibility sample, not a workload estimate. The 1.2x verify cost is an assumption, based on a read-sharing hypothesis (about 10.2 GB versus 8.5 GB per step), not a measurement. Long-context acceptance is not measured. The earlier 1.1-1.2x estimate remains unverified.

**Evidence.** local evidence: /ai/github/Maya-data/agent-tools/results/codex-mtp-accept.md; hub ROADMAP.md; local evidence: /ai/github/Maya-data/agent-tools/results/openmose-next.md (section 3)
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

**Evidence.** local evidence: /ai/github/Maya-data/agent-tools/results/sol-r9700-decode-regress.md; local evidence: /ai/github/Maya-data/agent-tools/results/muse-local-integration.md; local evidence: /ai/github/Maya-data/agent-tools/results/luna-rdna4-experts.md; local evidence: /ai/github/Maya-data/agent-tools/results/muse-r9700-subsweep.md; E22 (local evidence: /ai/github/Maya-data/benchmarks/r9700-typical/REPORT.md)

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

**Evidence.** [#15](https://github.com/mw00/project-maya/pull/15); local evidence: /ai/github/Maya-data/benchmarks/20261008-v1011-shadow-ab/summary.md; local evidence: /ai/github/Maya-data/benchmarks/20261008-v1011-2gpu/summary.md; local evidence: /ai/github/Maya-data/benchmarks/20261008-v1010-ram-shadow/validation-summary.json

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

**Evidence.** local evidence: Maya field-study lead ledger (LEADS.md) (MG-L003); local evidence: /ai/github/Maya-data/benchmarks/20261008-v1011-2gpu/2gpulend

### E22 - Low-RAM decode (48 GB) on one R9700 (OPEN)

**Question.** How fast is decode when the RAM tier is only 48 GB?

**Setup.** R9700 (HIP GPU 1), 32 GB; Maya v1.0.16 (746ee3f), context 8192, 48 GB RAM. Single R9700, 48 GB RAM tier, 256-token answers.

**Result.** Decode 28.1 / 27.7 tok/s (combined 28.1), VRAM hit 81%, RAM 16.2 per token, disk 1.4 reads per token; the report's 90 GB reference is 25.5.

**Superseded (2026-10-09).** The 28.1 figure is a measurement artifact. The replies averaged about 26 tokens, not the 256 the setup states, and short replies stay on VRAM-resident experts. Controlled 256-token replies on the same build measure 21.2-21.5 tok/s with 48 GB RAM (see E39). The VRAM, RAM and disk figures above come from the same short-reply run and are kept as recorded.

**Decision.** Recorded as a local result only; recommended single-R9700 settings wait for the crash fix. The first attempt crashed once at 1.5K tokens (illegal memory access); the two scored passes were clean.

**Caveats.** Crash on the first attempt, so stability at 48 GB is not established. The 90 GB reference (25.5) and the 3090 references (17.3 / 12.9) are quoted in the report without their own source. Two passes only.

**Evidence.** local evidence: /ai/github/Maya-data/benchmarks/r9700-typical/REPORT.md; local evidence: /ai/github/Maya-data/benchmarks/r9700-typical/b-ram48.jsonl

## Two GPUs

### E23 - Two-GPU layer split with MTP drafting (KEPT)

**Question.** Does a second card (layer split, MTP draft on the second card) raise decode and prompts?

**Setup.** R9700 (gfx1201, 32 GB, layers 0-23) + RX 7900 XT (gfx1100, 20 GB, layers 24-44 + NextN), Ryzen 7 9800X3D, 192 GB DDR5; Maya v1.0.11 base + #14, ROCm 7.2.1, RAM_GB 90, 8K context. R9700 first (larger half), 7900 XT second; greedy, 256-token answers.

**Result.** Answers ~15 -> ~33 tok/s (drafts accepted ~76%); 4K prompt ~414 -> ~490 tok/s; 39 back-to-back requests (1.8-4K-token prompts) without error.

**Decision.** Merged as #14 (v1.0.14).

**Caveats.** Hub also quotes ~34-36 tok/s from a v1.0.15 local run (not reconciled with 33 from the PR). One machine, one GPU pair.

**Evidence.** [#14](https://github.com/mw00/project-maya/pull/14); local evidence: /ai/github/Maya-data/benchmarks/20261008-v1011-2gpu/summary.md

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

**Evidence.** [#14](https://github.com/mw00/project-maya/pull/14); local evidence: Maya field-study lead ledger (LEADS.md); local evidence: /ai/github/Maya-data/benchmarks/20261008-v1011-2gpu/split26; local evidence: /ai/github/Maya-data/benchmarks/20261008-v1011-2gpu/split28

### E25 - One hipBLASLt table per architecture on a mixed pair (FIXED)

**Question.** Does a single tuning table leave one card of a mixed split on plain hipBLAS?

**Setup.** R9700 (gfx1201, 32 GB, layers 0-23) + RX 7900 XT (gfx1100, 20 GB, layers 24-44 + NextN), Ryzen 7 9800X3D, 192 GB DDR5; v1.0.11 + shadow branch. Run loaded only the gfx1201 table.

**Result.** CUDA0 (gfx1201) accepted its 40 rows; the gfx1100 card fell back to hipBLASEx. A per-architecture override was written, then #14 shipped the list form.

**Decision.** Found that the 7900 XT half printed 'tuning architecture mismatch' and used hipBLASEx; fixed by a ':'-separated list, one table per architecture (merged in #14).

**Caveats.** No before/after speed number was recorded for the mismatch.

**Evidence.** [#14](https://github.com/mw00/project-maya/pull/14); local evidence: /ai/github/Maya-data/benchmarks/20261008-shadow-budget90/two-gpu-fault-note.md

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

**Evidence.** coordinator session notes, 2026-10-08/09; local evidence: /ai/github/Maya-data/agent-tools/results/receipts/; local evidence: /ai/github/Maya-data/agent-tools/README.md

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

**Evidence.** [#15](https://github.com/mw00/project-maya/pull/15) (our comment, 2026-10-08); local evidence: /ai/github/Maya-data/benchmarks/20261008-shadow-budget90/two-gpu-fault-note.md; hub NOTES.md ('A correction')

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

**Evidence.** [#39](https://github.com/mw00/project-maya/pull/39); local evidence: /ai/github/Maya-data/agent-tools/results/codex-fix-result.md; local evidence: /ai/github/maya-ramcrash/verification/gpu-crash-final/summary.json; local evidence: /ai/github/Maya-data/agent-tools/results/sol-rebase-38-39.md; local evidence: /ai/github/Maya-data/agent-tools/results/muse-local-integration.md (re-verified on v1.0.23 + #39 + #38, incl. R9700 long-prompt repeats)

### E31 - Rebasing #38 and #39 onto v1.0.21 and v1.0.23 (KEPT-OPEN)

**Question.** Do the two open fixes still apply after #31, #41 and #44 merged, and do they work on GPU once rebased?

**Setup.** First, a CPU build on upstream v1.0.21 (89a1336), with the three HIP device-query mappings cherry-picked first, then 251c113 and cbfb03e reapplied. Then an integration build on upstream v1.0.23 (eadd3b6) with #39 (6b6d520, merged locally as 4e180a4) and #38 (b7b8d75, as a03b840), all five targets, 147/147 steps. GPU checks on the RX 7900 XT (gfx1100) and R9700 (gfx1201), KV FP16 and INT8; dual-GPU arm at layer split 24.

**Result.** The v1.0.21 build succeeded with warnings only. The v1.0.23 integration build succeeded (warnings only). Parity 16/16 pass across both GPUs and both KV types: model error 2.56e-6 to 3.10e-6; wmma2 versus F32 3.4e-7 (gfx1100) and 1.6e-7 (gfx1201). Reproducers all pass: 7900 XT resident 12/12 (about 347 tok/s prefill, about 19 decode); R9700 long prompts 9/9 FP16 (8K about 796, 16K about 808, 28K about 814 tok/s prefill) and 9/9 INT8 (about 785, 799, 806). Dual GPU at split 24: 690 and 756 tok/s prefill at 4K and 8K (about 490 before), decode 34 to 40 tok/s. Against #38 alone on the same protocol, the integration is within 1% on both single-GPU arms.

**Decision.** Both fixes apply after the rebase and pass the reproducers on GPU. #38 and #39 stay open upstream. The rebased PR branches were pushed 2026-10-09 and await a CUDA check.

**Caveats.** CUDA was not built or run for this rebase. The earlier R9700 baselines (800/830 prefill, 28.1 decode) do not reproduce under this protocol even for #38 alone, so they are a stale baseline, not a regression. The 28.1 decode was a short-reply artifact (see E39). The R9700 decode level is a separate open question (see E39). Single requests per long-prompt size.

**Evidence.** local evidence: /ai/github/Maya-data/agent-tools/results/sol-rebase-38-39.md; local evidence: /ai/github/Maya-data/agent-tools/results/muse-local-integration.md; [#38](https://github.com/mw00/project-maya/pull/38); [#39](https://github.com/mw00/project-maya/pull/39)
### E32 - #44 breaks the HIP build (#52) (FIXED)

**Question.** Does upstream v1.0.20/21 with #41 + #44 build and run on AMD?

**Setup.** Strix Halo 8060S; v1.0.20 + #41 + #44, ROCm 10.2; v1.0.21 + fix on ROCm 7.2.1 for gfx1100;gfx1201;gfx1151. Octopus merge of #41 and #44 on v1.0.20 on nimo.

**Result.** Build failed until the 3-line shim; then glm_layer_parity, glm_model_test and hip_glm_prefill_attention passed.

**Decision.** Three mappings were missing (cudaDeviceGetPCIBusId, cudaDevAttrMemoryClockRate, cudaDevAttrGlobalMemoryBusWidth). Reported on #44 and fixed by #52 (merged 2026-10-09).

**Caveats.** The report on #44 arrived after the merge.

**Evidence.** [#52](https://github.com/mw00/project-maya/pull/52); [#44](https://github.com/mw00/project-maya/pull/44) (our comment); local evidence: /ai/github/Maya-data/agent-tools/results/muse-nimo-pr41-44.md

## Long context

### E33 - Long prompts on one R9700, before the fix (FIXED)

**Question.** What does a typical user get with long prompts on one R9700?

**Setup.** R9700 (gfx1201, 32 GB), 90 GB RAM-resident; Maya v1.0.16 (746ee3f), context 40960. Prompts of ~7K, ~14K, ~20K, ~28K tokens, 16 output tokens.

**Result.** Successful runs 749-781 tok/s at 8-20K. 32K (~27.8K tokens): 0/3 completed.

**Decision.** Speed was fine when it completed; crashes were E30's bug. Any prefill error wedged the engine until restart.

**Caveats.** One or two requests per size. The RTX 3090 reference is quoted in the report without a source. One of the 32K failures was a planner reject ('planned 1792 of 222488 routed rows'), the others illegal memory access.

**Evidence.** local evidence: /ai/github/Maya-data/benchmarks/r9700-typical/REPORT.md; local evidence: /ai/github/Maya-data/benchmarks/r9700-typical/a-results.json

### E34 - KV ring (#41) and 64K context on Strix Halo (KEPT)

**Question.** Does the indexer-key ring and 64K context work on AMD, and what does it save?

**Setup.** Strix Halo 8060S, 128 GB unified; v1.0.20 + #41 + #44 (+ shim), TheRock ROCm 10.2, 64K context, Maya-S. Merged-PR build vs everyday server on nimo.

**Result.** State/KV at 64K: 1.60 -> 1.00 GB. No illegal/planned/error messages in either engine log. Maintainer's measurements on V100 (theirs): 3.06 -> 1.78 -> 1.13 GB at 128K, decode 18.1 -> 19.3 -> 19.6 tok/s.

**Decision.** #41 works on Strix (merged in v1.0.21 by upstream). The pool is unchanged here only because all experts already fit; on a 32 GB card the 0.6 GB goes to experts (the R9700 long-prompt repeat on #41/#44 is queued, not run).

**Caveats.** INT8 latent cache was not run. Single pass; R9700 long-prompt repeat not found in the records.

**Evidence.** [#41](https://github.com/mw00/project-maya/pull/41) (our comment); local evidence: /ai/github/Maya-data/agent-tools/results/muse-nimo-pr41-44.md; local evidence: /ai/github/Maya-data/agent-tools/results/receipts/pr4144-before.json

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

**Evidence.** local evidence: /ai/github/Maya-data/agent-tools/results/muse-r9700-longctx.md; local evidence: /ai/github/Maya-data/agent-tools/results/muse-r9700-longctx/ (logs, configs, receipts, harness)

### E41 - Strix long-context ladder (KEPT)

**Question.** Can nimo's everyday context rise to 128K+ while keeping speed, on current upstream plus our speedups?

**Setup.** Strix Halo 8060S (gfx1151), 128 GB unified, TheRock ROCm 10.2; new build `maya-next` (v1.0.24 + prompt-tail skip + gfx11 fused MoE fix), `--prefill 32768` explicit. Baseline: everyday v1.0.16 64K server.

**Result.** Final 262144 FP16 arm: 342.6/337.4/326.5/314.0 prompt tok/s at ~8K/~32K/~64K/~120K. New 64K arm: 347.9/340.2/324.2 at the first three lengths, so the larger reservation changes speed by -1.5%/-0.8%/+0.7%. At the two measured old-v1.0.16 baseline lengths, gains are +6.1% at 8K and +4.2% at 32K. Short-prompt decode 17.0 vs 17.1 tok/s, falling to 15.5 after ~120K; the five 262K-arm throughput replies reached 256 tokens. Muse reports 12/12 single-needle + 4/4 multi-hop hits through ~240K; preserved logs confirm requests through 239644 prompt tokens.

**Decision.** KEPT: `maya-nimo-256k.json`, 262144 context, FP16 latents, explicit prefill 32768, sub-batch 4096. All 12096 expert slots still fit (80.17 GB pool), with state/KV 3.32 GB; INT8 saves memory but has no capacity benefit on this box and costs about 2% prompt speed.

**Caveats.** One request per cell and one recall pass, no confidence interval. Reply texts were not preserved for independent recall rescoring. Capacity is not continuous residency: the raw 262K log reports 11260/12096 experts resident after the ~120K prompt and 1.26 disk reads/token during that decode, because prompt lending evicts experts. Reserving a larger window is distinct from filling it. The everyday config is temporarily replaced during the two-row job, which must restore it.

**Evidence.** /ai/github/Maya-data/agent-tools/results/muse-nimo-longctx.md (final, audited); /ai/github/Maya-data/agent-tools/results/muse-nimo-next262k-before.json and -after.json; preserved config/engine/server logs in /ai/github/Maya-data/agent-tools/results/muse-nimo-longctx-evidence/

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

**Setup.** Single R9700, Maya-S; 12 tool cases in /ai/github/Maya-data/eval/tools_cases.json run against serve/ from the old `amd/glm-gfx1100` checkout (v1.0.15-era) vs serve/ from upstream main (v1.0.26).

**Result.** Not a parser bug in upstream: the old serve/ only parsed Qwen's `<function=...>` form, so every GLM `NAME<arg_key>..` body was rejected. Stale serve/: 2/12; current serve/: 11/12 (the one miss is the model also calling web_search, not a parse error). Exact raw body captured and pinned in a unit test (`serve/test_tools.py`, 23 tests pass).

**Decision.** FIXED: the server launcher (`start7900.sh`) now runs serve/ from each config's own engine tree instead of the stale checkout, so frontend and engine always match.

**Caveats.** The 11/12 is one 12-case run. The old checkout's serve/ itself was left untouched (it is the user's branch).

**Evidence.** local evidence: /ai/github/Maya-data/agent-tools/results/sonnet-toolcall-fix.md; local evidence: /ai/github/Maya-data/eval/results-smoke/tools.jsonl

### E44 - NIAH recall garbles exact digits (OPEN)

**Question.** Does long-context recall stay exact at 32-122K on the ~2-bit Maya-S quant?

**Setup.** Single R9700, 131072 INT8 arm of E40; needle-in-a-haystack with real varied filler (402 repo sources), 3 needles at ~10/50/90% depth, temperature 0, plus one multi-hop sum question; one run each at ~33K/~65K/~122K.

**Result.** 2/4 at 32.9K, 1/4 at 65.0K, 2/4 at 121.7K; the multi-hop sum is 3/3 with correct addends each time. Misses are noisy, not length-driven: digit transpositions (4817->4481, KQ-2291->KQ-2219) come and go across lengths, and one needle misreads 06:40 as 0600 at all three lengths with no confounder in the filler.

**Decision.** OPEN: quant precision is a hypothesis, not an attribution. The completed E35 short-task baseline does not test long-context digit recall or isolate quant versus KV precision.

**Caveats.** Single run per length; one reply rescored HIT->MISS on a substring-scorer false positive ("44817" contains "4481" but the needle was 4817).

**Evidence.** local evidence: /ai/github/Maya-data/agent-tools/results/muse-r9700-longctx.md

## Comparisons with other projects

### E36 - OpenMOSE Strata-GLM-AMD (their numbers) (OPEN)

**Question.** Which of another project's techniques and results apply to Maya on R9700 and Strix Halo?

**Setup.** Theirs: 2x Radeon Pro W7900 (gfx1100, 48 GB), Ryzen 7 5700X3D, 125 GB DDR4; nothing run on RDNA4 or gfx1151; Strata-GLM-AMD docs/GLM5_NEXT.md, GSQ-RCO 3.0-bit GGUF (not Maya's pack). Read-only review by an agent.

**Result.** Examples (theirs): window 4096 raised 4K prefill 531 -> 764 tok/s; CPU-computed decode misses 14.6 -> 28.4 tok/s; their lookahead prefetch was slower (30.7 tok/s with none vs 27.6 with cap 3).

**Decision.** A ranked list of techniques was extracted. Priorities noted: single-GPU MTP on Strix first; window size 4096 with chunk 2048 and admission/exchange swap on the R9700; windowed DSA only if our own 16-28K curve falls off. Nothing implemented.

**Caveats.** All numbers are theirs, on different hardware, model quant and engine; not comparable to Maya's. The report marks several points UNVERIFIED (e.g. single-card acceptance, RDNA4 builtin variant). Includes their negative results (lookahead prefetch slower; MMQ column-tile tuning spills on wave32).

**Evidence.** local evidence: /ai/github/Maya-data/agent-tools/results/openmose-next.md; local evidence: /ai/github/Maya-data/agent-tools/results/openmose-readme.md; coordinator session notes, 2026-10-08/09

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
