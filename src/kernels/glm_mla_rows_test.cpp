// The verify window's absorbed MLA projections against one mla() call per row, byte for byte, with padded strides.
// Synthetic BF16 weights and FP16 / INT8 latent caches; no model or oracle fixture needed.
#include "strata/kernels/glm_fast.hpp"
#include <cuda_runtime.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <vector>

namespace gf = strata::kernels::glmf;
namespace {
void check(cudaError_t e) {
    if (e != cudaSuccess) {
        std::fprintf(stderr, "glm_mla_rows_test: %s\n", cudaGetErrorString(e));
        std::exit(1);
    }
}
struct Buffer {
    void* p = nullptr;
    explicit Buffer(size_t bytes) { check(cudaMalloc(&p, bytes)); }
    ~Buffer() { check(cudaFree(p)); }
    void put(const void* h, size_t bytes) { check(cudaMemcpy(p, h, bytes, cudaMemcpyHostToDevice)); }
    void get(void* h, size_t bytes) { check(cudaMemcpy(h, p, bytes, cudaMemcpyDeviceToHost)); }
};
void equal(const void* a, const void* b, size_t bytes, const char* what, int T, bool q8) {
    if (std::memcmp(a, b, bytes) != 0) {
        std::fprintf(stderr, "glm_mla_rows_test: %s differs, T=%d lat_q8=%d\n", what, T, (int) q8);
        std::exit(1);
    }
}
void test(int T, bool lat_q8, int n_sel) {
    constexpr int H = 2, Q = 576, K = 512, V = 256, N = 65, q8_bytes = 36;
    constexpr int ld_q = H * Q + 8, ld_abs = H * K + 8, ld_out = H * V / 32 + 2;
    std::mt19937 rng(617 + T);
    std::uniform_real_distribution<float> dist(-0.5f, 0.5f);
    std::vector<float> q((size_t) T * ld_q), want_abs((size_t) T * ld_abs, -123.0f), got_abs = want_abs;
    for (float& x : q) x = dist(rng);
    for (int i = 0; i < H * Q; ++i) q[i] = 0.0f;   // row 0 also exercises zero dots
    std::vector<uint16_t> wk(H * K * Q), wv(H * V * K);
    for (auto* w : {&wk, &wv})
        for (uint16_t& x : *w) x = (uint16_t) (0x3c00 + rng() % 512 + (rng() % 2 ? 0x8000 : 0));
    const int rec = lat_q8 ? gf::lat8_rec_bytes(K) : K * 2;
    std::vector<uint8_t> lat((size_t) N * rec);
    for (int n = 0; n < N; ++n) {
        if (lat_q8) {
            for (int i = 0; i < K; ++i) lat[(size_t) n * rec + i] = (uint8_t) (int8_t) ((int) (rng() % 255) - 127);
            for (int i = 0; i < K / 32; ++i) {
                const uint16_t scale = 0x2000;
                std::memcpy(lat.data() + (size_t) n * rec + K + i * 2, &scale, 2);
            }
        } else {
            for (int i = 0; i < K; ++i) {
                const uint16_t x = (uint16_t) (0x2c00 + rng() % 512 + (rng() % 2 ? 0x8000 : 0));
                std::memcpy(lat.data() + (size_t) n * rec + i * 2, &x, 2);
            }
        }
    }
    // Different selections per row, including masked cells and a partial final attention chunk.
    std::vector<int> cells((size_t) T * n_sel);
    for (int t = 0; t < T; ++t)
        for (int i = 0; i < n_sel; ++i) cells[t * n_sel + i] = i % 7 == 6 ? -1 : (i * 3 + t) % N;
    std::vector<uint8_t> want((size_t) T * ld_out * q8_bytes, 0xa5), got = want, vb_want = want;
    Buffer dq(q.size() * 4), dwk(wk.size() * 2), dwv(wv.size() * 2), dl(lat.size()), dc(cells.size() * 4);
    Buffer da(got_abs.size() * 4), dar(want_abs.size() * 4), ctx(got_abs.size() * 4);
    Buffer dout(got.size()), dref(want.size()), dvref(want.size());
    dq.put(q.data(), q.size() * 4); dwk.put(wk.data(), wk.size() * 2); dwv.put(wv.data(), wv.size() * 2);
    dl.put(lat.data(), lat.size()); dc.put(cells.data(), cells.size() * 4);
    da.put(got_abs.data(), got_abs.size() * 4); dar.put(want_abs.data(), want_abs.size() * 4);
    dout.put(got.data(), got.size()); dref.put(want.data(), want.size()); dvref.put(vb_want.data(), vb_want.size());
    gf::mla_kb_rows((uint16_t*) dwk.p, (float*) dq.p, ld_q, H, Q, K, T, (float*) da.p, ld_abs, nullptr);
    for (int t = 0; t < T; ++t) {
        gf::mla_kb_rows((uint16_t*) dwk.p, (float*) dq.p + t * ld_q, ld_q, H, Q, K, 1,
                        (float*) dar.p + t * ld_abs, ld_abs, nullptr);
        gf::mla_attention((float*) da.p + t * ld_abs, (uint16_t*) dl.p, (int*) dc.p + t * n_sel,
                          n_sel, H, Q, K, (float*) ctx.p + t * ld_abs, nullptr, lat_q8);
        gf::mla((float*) dq.p + t * ld_q, (uint16_t*) dwk.p, (uint16_t*) dwv.p, (uint16_t*) dl.p,
                (int*) dc.p + t * n_sel, n_sel, H, Q, K, V, (uint8_t*) dref.p + t * ld_out * q8_bytes,
                nullptr, lat_q8);
        gf::mla_vb_rows((uint16_t*) dwv.p, (float*) ctx.p + t * ld_abs, ld_abs, H, K, V, 1,
                        (uint8_t*) dvref.p + t * ld_out * q8_bytes, ld_out, nullptr);
    }
    gf::mla_vb_rows((uint16_t*) dwv.p, (float*) ctx.p, ld_abs, H, K, V, T, dout.p, ld_out, nullptr);
    check(cudaGetLastError());
    check(cudaDeviceSynchronize());
    if (gf::launch_errors() != 0) {
        std::fprintf(stderr, "glm_mla_rows_test: a kernel launch failed\n");
        std::exit(1);
    }
    da.get(got_abs.data(), got_abs.size() * 4); dar.get(want_abs.data(), want_abs.size() * 4);
    dout.get(got.data(), got.size()); dref.get(want.data(), want.size()); dvref.get(vb_want.data(), vb_want.size());
    equal(got_abs.data(), want_abs.data(), got_abs.size() * 4, "k_b rows (including padding)", T, lat_q8);
    equal(got.data(), vb_want.data(), got.size(), "v_b rows (including padding)", T, lat_q8);
    equal(got.data(), want.data(), got.size(), "per-row mla (including padding)", T, lat_q8);
}
}  // namespace

int main() {
    if (std::getenv("STRATA_GLM_MLA1") != nullptr) {
        std::fprintf(stderr, "glm_mla_rows_test: unset STRATA_GLM_MLA1 (the scalar-dot reference path)\n");
        return 1;
    }
    for (int T = 1; T <= gf::kMaxRows; ++T)
        for (bool q8 : {false, true})
            for (int n_sel : {1, 33, 65}) test(T, q8, n_sel);
    std::puts("glm_mla_rows_test: PASS (48 cases, byte-identical)");
}
