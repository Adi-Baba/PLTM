#ifndef POLYLOG_H
#define POLYLOG_H

#ifdef _WIN32
  #define EXPORT __declspec(dllexport)
#else
  #define EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

// --- C API (Bindings) ---
typedef void* PolylogHandle;

// Create a context for signal length N with singularity index s, gain, and damping
EXPORT PolylogHandle ply_create_context(int N, float s, float gain, float damping);

// Process a signal input -> output (buffers must be size N)
EXPORT void ply_process(PolylogHandle ctx, const float* input, float* output);

// Update parameter s on the fly (Online Learning support)
EXPORT void ply_update_s(PolylogHandle ctx, float s);

// Get direct access to overflow buffer (for resetting/inspection)
EXPORT float* ply_get_overflow_buffer(PolylogHandle ctx);

// Free memory
EXPORT void ply_destroy(PolylogHandle ctx);

#ifdef __cplusplus
}

// --- C++ API (Direct) ---
#include <vector>
#include <complex>

class PolylogEngine {
public:
    PolylogEngine(int context_size, float s_val);
    ~PolylogEngine();

    void process(const std::vector<float>& input, std::vector<float>& output);
    void process_pointer(const float* input, float* output);
    void set_s(float new_s);

private:
    int N;
    float s;
    
    // Opaque pointers to hide KissFFT details
    void* cfg_fwd; 
    void* cfg_inv;
    
    // Internal Buffers - Using std::complex which is layout-compatible with kiss_fft_cpx
    std::vector<std::complex<float>> time_buffer;
    std::vector<std::complex<float>> freq_buffer;
    std::vector<std::complex<float>> kernel;

    void compute_kernel();
};
#endif

#endif
