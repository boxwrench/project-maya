# Experimental Maya on RX 7900 XT / XTX

This branch adds a Linux HIP build and installer path for Maya's GLM-5.3-Flash
engine on `gfx1100`. It uses one GPU and serves text. Images and HIP multi-GPU
inference are not enabled by this installer.

Use a system ROCm 7 installation with its HIP compiler and hipBLAS, Python
3.10+, CMake 3.24+, and a C++20 compiler. `ROCM_PATH` selects an installation
outside `/opt/rocm`. Everything Maya downloads goes into the project or its
model-data folder; setup does not install system packages.

```sh
./maya.sh --backend hip --gpu 0 --check
./maya.sh --backend hip --gpu 0 --context 8192 --no-vision --yes \
  --download-model --no-start --port 8099 --env STRATA_GLM_RAM_GB=60
./maya.sh --backend hip --port 8099
```

The first setup downloads about 96.5 GB of weights and verifies the SHA-256
hash of each shard. `--gguf-dir DIR` uses existing files instead. GPU numbers
are the kernel KFD topology order shown by `--check`; on the test machine the
7900 XT is GPU 0, an R9700 is GPU 1, and the integrated GPU is GPU 2. This
build accepts only `gfx1100`. Configs are named `maya-<quant>-hip.json` and
select the AMD device through `HIP_VISIBLE_DEVICES`.

The HIP config starts with an 8K context when requested above, a 256-token
prompt chunk, 3 GiB of GPU headroom, and 16 GiB of system-RAM headroom. The
example additionally caps the pinned expert cache at 60 GiB. Change those
settings with `--env KEY=VALUE` during setup. Available RAM, rather than
installed RAM, determines how much can be cached. The engine reduces its
RAM-tier allocation if ROCm cannot pin the requested amount.

## What changed

- Added HIP GPU detection, compilation, rebuild stamps, and serving configs to
  `maya.py`, with separate CUDA and HIP build directories.
- Completed the CUDA-shaped runtime/BLAS mappings needed by GLM, excluded
  NVIDIA's profiling ranges from HIP, and linked the GLM prompt path to HIP MMQ
  and hipBLAS.
- Replaced the GLM hyper-connection kernel's unsupported FP32 `__ldcg` with
  agent-scope acquire loads on HIP. CUDA keeps its existing implementation.
- Kept CUDA WMMA attention guarded to CUDA builds. HIP uses the FP32 attention
  path with a 16-row tile that fits RDNA's 64 KiB workgroup shared-memory limit.
- Fixed the KDA parity fixture to initialize its recurrence state explicitly;
  its numerical tolerance is unchanged.
- A companion handoff-fix contribution backports Strata's HIP post-store
  system fence in both shared doorbell kernels and Maya's fused routing
  signal. The validation below includes that companion change.

Upstream Strata was inspected at `d5ea713` (0.1.40.3). Its AMD backend documents
runtime selection, memory limits, desktop VRAM headroom, and newer packed-byte
optimizations. Maya's GLM engine is separate from Strata's current Qwen engine,
so Strata's model benchmarks do not establish Maya performance.

## Validation

Test host: RX 7900 XT 20 GiB, Ryzen 7 9800X3D, 192 GB installed DDR5, native
Ubuntu Linux, system ROCm 7.2.1 / Clang 22. Maya base revision `70e0746` (1.3.0).

The HIP engine and selected test targets build. All 14 selected GPU checks
pass: device allocation, expert upload staging, packed-byte/shuffle
intrinsics, asynchronous mapped-memory handoff, IQ1_S arithmetic, GLM
hyper-connections, KDA, FFN, DSA, layer arithmetic, synthetic-model logits
against the committed reference, and HIP MMQ. Five isolated installer tests
passed. The fused HC decode kernel also agrees with the independent CPU
reference across eight consecutive 4096-wide steps, including in-place gate
updates, in all three variants (default HC3, forced HC2, and forced HC1).
This is not yet an end-to-end test of the 321B model.

The real batched MLA attention kernel agrees with a double-precision CPU
softmax reference (absolute tolerance `5e-5`), including empty and masked cell
lists, partial and multiple tiles, and multiple head groups. It reads the
FP16 latent cache added in Maya 1.3.0.

The GLM handoff test replays the real routing graph for 100 disk requests
and 100 CPU-lane requests with changing IDs, weights, inputs and answers. The
CPU observes each request without a driver query or stream synchronization.
Both this test and the shared 300-round handoff test also pass on the R9700
using isolated `gfx1201` binaries; the normal Maya installer still targets
`gfx1100` pending full-model validation. See [the shared AMD backport audit](AMD_UPSTREAM.md).

```sh
python3 tools/test_maya_hip.py
LD_LIBRARY_PATH=/opt/rocm/lib HIP_VISIBLE_DEVICES=0 \
  ctest --test-dir build-hip --output-on-failure --timeout 60 \
  -R '^(glm_(hc|kda|ffn|dsa|layer)_parity|glm_model_test|hip_intrinsics|hip_(glm_handoff|glm_prefill_attention|handoff|device_selftest|prefill_mmq_parity|expert_cache_staging)|iq1_s_parity)$'
STRATA_GLM_HC2=1 LD_LIBRARY_PATH=/opt/rocm/lib HIP_VISIBLE_DEVICES=0 \
  build-hip/glm_hc_parity --selftest
STRATA_GLM_HC1=1 LD_LIBRARY_PATH=/opt/rocm/lib HIP_VISIBLE_DEVICES=0 \
  build-hip/glm_hc_parity --selftest
```

The relevant build targets must be built before running CTest. Build and test
logs are kept in `build-hip/`. Full-model startup and response testing are the
next validation step; no Maya throughput number is claimed here.
