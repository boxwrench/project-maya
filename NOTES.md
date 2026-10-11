# Engineering notes

Things we learned porting Maya to AMD, written for whoever picks this up next.

## How this was built

This port was **AI-assisted, and we say so openly.** Most of the code, debugging and benchmarking was done by AI coding
agents:
- **Claude Code:** coordination.
- **OpenAI Codex, Muse Spark and Gemini:** implementation, review and research.
- **Smaller Claude agents:** benchmark runs.

They ran on our own hardware. A person (boxwrench) directed the work, chose what to pursue, ran the machines, and
reviewed what went upstream.

Every PR says what was and wasn't tested. The Maya maintainer reviews each one and checks that NVIDIA output is
unchanged (the same tokens on V100s) before merging. Nothing was merged on an AI's word alone.

## Test machines

- **Desktop:** Ryzen 7 9800X3D, 192 GB DDR5, Radeon RX 7900 XT 20 GB (PCIe 4 x16) + Radeon AI PRO R9700 32 GB
  (PCIe 5 x16), Ubuntu, ROCm 7.2.1.
- **"nimo":** Ryzen AI Max+ 395 (Strix Halo), Radeon 8060S, 128 GB unified memory, ROCm 7.2.2.
- **ROCm 10 evaluation:** nimo also ran TheRock 10.2.0a20261009 in a virtual environment; the system ROCm install
  was left unchanged.

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
- **ROCm 10 nightlies:** TheRock ships as pip wheels into a virtual environment, so it does not require a system
  change. The hipBLASLt table must be retuned for each library version; the 10.2 nimo run used hipBLASLt table
  version **100500**. A 100202 table is not a substitute.
- **Build flag:** since Maya v1.0.12 a HIP build needs `STRATA_PREFILL_MMQ=ON`. Setup always sets it; hand-made builds
  must too.
- **Strix Halo memory:** the "VRAM" the runtime reports on an APU is just the carve-out. Pinned host memory and the GPU
  pool come out of the same RAM, so size the pool from `MemAvailable` and don't keep a second pinned copy of the
  experts (#17).
- **Pinned update buffers:** an async H2D copy from a pinned table-update buffer must finish before that buffer is
  rewritten. Synchronizing a separate copy stream is not enough when the update is queued on the nonblocking stream;
  the next prompt-lending boundary can otherwise change the source while the DMA is still reading it.
- **IOMMU on Strix:** the setting affects prompt speed. On the Strix box, `amd_iommu=off` measured +13%/+11%/+15% prefill
  at ~4K/~8K/~16K versus IOMMU on, with decode unchanged (one request per size; before/after separated by a reboot, so
  other host state could differ). A separate Strix engine reported a similar 13-16% gain. It disables DMA translation
  machine-wide (and the NPU), so it is a host setting, not an upstream default. See E08.

## Where the time goes

- **Strix Halo decode** (~54.4 ms per token in one profile): dense GEMV and projections 29.78 ms (~55%), routed expert
  kernels 13.59 ms (~25%), attention 7.21 ms (~13%), the remaining small kernels the rest. The chip's memory bandwidth
  is the wall.
- **One discrete card:** decode depends on how many experts are in VRAM and how misses are served. The CPU computes
  part of the RAM-tier experts while PCIe carries the rest, and that split is worth ~11 tok/s on two GPUs. On one R9700,
  `STRATA_GLM_RESERVE_MB=1024` (+5.3%) and a 90 GB RAM tier (+14.2%) add expert slots and speed decode (see README).
- **Prompts are compute-bound on both chips.** On Strix, removing all SSD re-reads gave no prefill gain and the copy
  lane stayed under 30% busy. On the R9700 the measured prefill shares are attention ~21%, KDA projections ~19%, MoE
  ~27%, and the remaining dense projections and glue, with the transfers hidden. Prompt work should target math, not
  I/O. A larger prompt chunk (32768 vs 13312 tokens) measured +4.1% at 32K.
- **GEMM headroom is small.** The R9700's hipBLASLt prompt projections average ~101 TFLOPS, about 66% of the chip's
  ~154 TFLOPS FP16 peak at 2350 MHz, but GEMMs are only ~11% of prefill wall time. Better selection is worth an
  estimated 1-4% end-to-end; FP8 GEMMs could be ~2x once a pack exists. GEMM tuning is parked until the compute-bound
  kernels below it are addressed.

## Things that looked like bugs and weren't

- **A decode regression that was a measurement artifact (2026-10-09).** A single-R9700 drop from 28.1 to about 22 tok/s
  between v1.0.16 and later builds did not reproduce. The 28.1 run's replies averaged about 26 tokens, which stay on
  VRAM-resident experts and decode faster. With 256-token replies both builds give 21.2-21.5 tok/s. Lesson: check the
  output length of every benchmark reply before comparing speeds.
- **A correction.** An intermittent GPU memory fault on two GPUs was first put down to another process using the cards.
  That was wrong. The first reproducible fault was in the RAM-shadow option (#15), and on Maya v1.0.14 it reproduced on
  one card within a few requests, always in the prefill of a new request. Lesson: "it went away for 80 rounds" isn't a
  root cause. Keep the failing configuration and bisect it.
- **AMD illegal-memory-access root cause (fixed, v1.0.32).** The highest-priority race was the table
  update described above: boundary A queues an H2D copy, then prompt lending or boundary B rewrites the same pinned
  `upd_key_h`/`upd_val_h` buffers before the earlier DMA has consumed them. Device tables then disagree with the host's
  slot ownership. Resident mode added a second race where a background promotion could publish a slot after lending had
  borrowed it. Corrupted weights can produce NaN router scores; the top-k routers retain `INT_MAX` for an all-NaN row
  and then use it as a table index. The fix waits on the right stream, retires moves before lending, and rejects
  invalid route IDs rather than clamping them into a plausible answer. It merged upstream as
  [#39](https://github.com/mw00/project-maya/pull/39) in v1.0.32.
- **Greedy output that differs between runs on HIP.** Expert placement decides which experts the CPU computes, and that
  changes floating-point rounding. Compare runs by coherence and acceptance, not exact text, unless you pin the tiers.

## Optimization tracking

Optimization leads are tracked with a simple question: what work runs that cannot affect the result? Each lead records
its candidate and effectful work, a negative control, a correctness check, and the cheapest test that would kill it.
Rejected leads are kept too. The "not worth it" table in the [roadmap](ROADMAP.md) is where those landed.
