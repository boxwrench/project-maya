# Engineering notes

Things we learned porting Maya to AMD, written for whoever picks this up next.

## How this was built

This port was **AI-assisted, and we say so openly.** Most of the code, debugging and benchmarking was done by AI coding
agents:
- **Claude Code:** coordination, debugging, reviews, PRs.
- **OpenAI Codex:** larger kernel and engine changes.
- **Gemini:** research.
- **Smaller Claude agents:** benchmark runs.

They ran on our own hardware. A person (boxwrench) directed the work, chose what to pursue, ran the machines, and
reviewed what went upstream.

Every PR says what was and wasn't tested. The Maya maintainer reviews each one and checks that NVIDIA output is
unchanged (the same tokens on V100s) before merging. Nothing was merged on an AI's word alone.

## Test machines

- **Desktop:** Ryzen 7 9800X3D, 192 GB DDR5, Radeon RX 7900 XT 20 GB (PCIe 4 x16) + Radeon AI PRO R9700 32 GB
  (PCIe 5 x16), Ubuntu, ROCm 7.2.1.
- **"nimo":** Ryzen AI Max+ 395 (Strix Halo), Radeon 8060S, 128 GB unified memory, ROCm 7.2.2.

## How we test

- **Parity first.** Every kernel change runs Maya's parity tests on each GPU: layer, FFN, DSA, the model test, MMQ and
  attention. New kernels get their own comparison against the old ones; most are bit-identical.
- **Then the full model:** a 7-check smoke test (arithmetic, code, a two-turn conversation, long answers), then
  benchmarks. We use the same requests for every arm, run them back to back, and switch features with an environment
  flag in the same binary where possible.
- **Only as much benchmarking as the decision needs.** This is experimental work on local hardware, so the default is
  a handful of requests per arm. We run longer stress tests only to chase an intermittent fault.

## HIP / ROCm lessons

- **CUDA headers shimmed for HIP:** Maya builds its CUDA code for HIP through `include/strata/hip_compat`.
  - The shim's shuffle macros clash with HIP's own bf16/fp8 headers when rocWMMA is included. Wrap that include in
    `push_macro` / `#undef` / `pop_macro`.
  - Inline PTX (e.g. `%globaltimer`) doesn't compile for AMD. Use `wall_clock64()`, a constant 100 MHz counter on RDNA.
- **Wave32 and 64 KiB of LDS:** RDNA runs 32-wide waves, and a workgroup gets at most 64 KiB of shared memory. Kernels
  sized for NVIDIA's larger shared memory need smaller tiles or FP16 staging.
- **Memory ordering:** the GPU-to-CPU doorbells needed a system-scope fence after the store on HIP (#2). Without it the
  host could see a stale signal.
- **Link order:** lld (used by the HIP toolchain) doesn't care about static-library order, but GNU ld (used on the
  maintainer's CUDA box) does. Link `strata_engine` before `strata_prefill`.
- **hipBLASLt tables:** they're per architecture and per hipBLASLt version (e.g. 100202). A mismatched table is
  silently ignored and falls back to plain hipBLAS, which on RDNA3 is several times slower for the prompt projections.
- **Build flag:** since Maya v1.0.12 a HIP build needs `STRATA_PREFILL_MMQ=ON`. Setup always sets it; hand-made builds
  must too.
- **Strix Halo memory:** the "VRAM" the runtime reports on an APU is just the carve-out. Pinned host memory and the GPU
  pool come out of the same RAM, so size the pool from `MemAvailable` and don't keep a second pinned copy of the
  experts (#17).

## Where the time goes

- **Strix Halo decode** (55.9 ms per token): dense matrix-vector work ~50% (already ~91% of memory bandwidth), expert
  kernels ~27%, attention and small kernels the rest. Tiers don't matter: 99.9% of experts are hit in GPU memory.
- **One discrete card:** decode depends on how many experts are in VRAM and how misses are served. The CPU computes
  part of the RAM-tier experts while PCIe carries the rest, and that split is worth ~11 tok/s on two GPUs.
- **Prompts:** MoE work ~45%, attention ~20% (less after #16), dense GEMMs ~20%. Host planning is negligible.

## Things that looked like bugs and weren't

- **An intermittent GPU memory fault on two GPUs.** It happened while another process was using the cards and never
  again in ~80 clean rounds. Check `rocm-smi --showpids` before blaming the engine.
- **Greedy output that differs between runs on HIP.** Expert placement decides which experts the CPU computes, and that
  changes floating-point rounding. Compare runs by coherence and acceptance, not exact text, unless you pin the tiers.

## Optimization tracking

Leads are tracked privately with a lens called "direct enumeration of contributors": what work runs that can't affect
the result? Each lead records its candidate and effectful work, a negative control, a correctness check, and the
cheapest test that would kill it. Rejected leads are kept too. The "not worth it" table in the
[roadmap](ROADMAP.md) is where those landed.
