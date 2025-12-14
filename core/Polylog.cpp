#include "polylog.h"
#include "kiss_fft/kiss_fft.h"
#include <cmath>
#include <cstring>
#include <iostream>

// --- C++ Class Implementation ---

PolylogEngine::PolylogEngine(int context_size, float s_val) : N(context_size), s(s_val) {
    // 1. Allocate Plans
    cfg_fwd = kiss_fft_alloc(N, 0, NULL, NULL);
    cfg_inv = kiss_fft_alloc(N, 1, NULL, NULL);

    // 2. Resize Buffers
    time_buffer.resize(N);
    freq_buffer.resize(N);
    kernel.resize(N);

    // 3. Precompute properties
    compute_kernel();
}

PolylogEngine::~PolylogEngine() {
    free(cfg_fwd);
    free(cfg_inv);
}

void PolylogEngine::compute_kernel() {
    // k=0 is DC (Preserved)
    kernel[0] = std::complex<float>(1.0f, 0.0f);

    for(int k=1; k < N; k++) {
        // Polylog Decay: k^(-s)
        float w = std::pow((float)k, -s);
        kernel[k] = std::complex<float>(w, 0.0f);
    }
}

void PolylogEngine::set_s(float new_s) {
    if (s != new_s) {
        s = new_s;
        compute_kernel();
    }
}

void PolylogEngine::process(const std::vector<float>& input, std::vector<float>& output) {
    if (input.size() != N || output.size() != N) {
        std::cerr << "[Polylog] Error: Buffer size mismatch." << std::endl;
        return;
    }
    process_pointer(input.data(), output.data());
}

void PolylogEngine::process_pointer(const float* input, float* output) {
    // 1. Copy Input -> Complex Buffer
    for(int i=0; i<N; i++) {
        time_buffer[i] = std::complex<float>(input[i], 0.0f);
    }

    // 2. Forward FFT
    // Cast std::complex* to kiss_fft_cpx* (Safe: both are struct{float, float})
    kiss_fft((kiss_fft_cfg)cfg_fwd, (kiss_fft_cpx*)time_buffer.data(), (kiss_fft_cpx*)freq_buffer.data());

    // 3. Convolution (Kernel Multiply)
    for(int i=0; i<N; i++) {
        // Complex Multiply
        // Since kernel is real-valued in our case (imag=0), we could optimize, but generic replace:
        freq_buffer[i] *= kernel[i];
    }

    // 4. Inverse FFT
    kiss_fft((kiss_fft_cfg)cfg_inv, (kiss_fft_cpx*)freq_buffer.data(), (kiss_fft_cpx*)time_buffer.data());

    // 5. Output Real Part (Normalized)
    float scale = 1.0f / N;
    for(int i=0; i<N; i++) {
        output[i] = time_buffer[i].real() * scale;
    }
}

// --- C API Wrappers ---

EXPORT PolylogHandle ply_create_context(int N, float s) {
    return new PolylogEngine(N, s);
}

EXPORT void ply_process(PolylogHandle ctx, const float* input, float* output) {
    static_cast<PolylogEngine*>(ctx)->process_pointer(input, output);
}

EXPORT void ply_update_s(PolylogHandle ctx, float s) {
    static_cast<PolylogEngine*>(ctx)->set_s(s);
}

EXPORT void ply_destroy(PolylogHandle ctx) {
    delete static_cast<PolylogEngine*>(ctx);
}
