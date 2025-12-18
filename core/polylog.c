#include "polylog.h"
#include "kiss_fft/kiss_fft.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h> // Debug

typedef struct PolylogContext PolylogContext;

struct PolylogContext {
    int N;
    int fft_size;
    float s;
    
    // KissFFT Configurations
    kiss_fft_cfg cfg_fwd;
    kiss_fft_cfg cfg_inv;
    
    // Buffers (interleaved complex: r, i, r, i...)
    kiss_fft_cpx* time_buffer;
    kiss_fft_cpx* freq_buffer;
    kiss_fft_cpx* kernel;
    
    // Memory State (Overlap-Add)
    float* overflow_buffer;
};

void _compute_kernel(PolylogContext* ctx);

// Precompute the k^-s kernel (Time Domain Construction)
void _compute_kernel(PolylogContext* ctx) {
    // 1. Construct Causal Impulse Response h[t] = (1+t)^-s
    // We limit filter length to N to ensure linear convolution fits in 2N FFT
    int filter_len = ctx->N; 
    
    for(int i=0; i < ctx->fft_size; i++) {
        if (i < filter_len) {
            // Causal decay 
            // Using (1+i) to avoid singularity at 0 if we used i^-s
            float w = powf(1.0f + i, -ctx->s);
            ctx->kernel[i].r = w;
            ctx->kernel[i].i = 0.0f;
        } else {
            // Zero Pad
            ctx->kernel[i].r = 0.0f;
            ctx->kernel[i].i = 0.0f;
        }
    }

    // 2. Transform Kernel to Frequency Domain
    // We use the 'freq_buffer' as temporary scratch space to hold the FFT result
    // Then copy it back to 'kernel' (or just compute inplace if possible, but safe is better)
    
    // Actually, kiss_fft can operate out-of-place. 
    // in: kernel (time), out: kernel (freq) ? 
    // No, pointers must be different usually.
    // Let's use freq_buffer as temp output.
    
    kiss_fft(ctx->cfg_fwd, ctx->kernel, ctx->freq_buffer);
    
    // Copy result back to ctx->kernel (now holding Frequency Weights)
    for(int i=0; i < ctx->fft_size; i++) {
        ctx->kernel[i] = ctx->freq_buffer[i];
    }
}

EXPORT PolylogHandle ply_create_context(int N, float s) {
    PolylogContext* ctx = (PolylogContext*)malloc(sizeof(PolylogContext));
    if (!ctx) return NULL;

    ctx->N = N;
    ctx->fft_size = 2 * N;
    ctx->s = s;
    
    // Allocate Plans (1 = Inverse, 0 = Forward)
    ctx->cfg_fwd = kiss_fft_alloc(ctx->fft_size, 0, NULL, NULL);
    ctx->cfg_inv = kiss_fft_alloc(ctx->fft_size, 1, NULL, NULL);
    
    // Allocate Buffers
    ctx->time_buffer = (kiss_fft_cpx*)malloc(sizeof(kiss_fft_cpx) * ctx->fft_size);
    ctx->freq_buffer = (kiss_fft_cpx*)malloc(sizeof(kiss_fft_cpx) * ctx->fft_size);
    ctx->kernel = (kiss_fft_cpx*)malloc(sizeof(kiss_fft_cpx) * ctx->fft_size);
    ctx->overflow_buffer = (float*)calloc(N, sizeof(float)); // Initialize with zeros!
    
    // Error Handling: Check allocations
    if (!ctx->cfg_fwd || !ctx->cfg_inv || !ctx->time_buffer || 
        !ctx->freq_buffer || !ctx->kernel || !ctx->overflow_buffer) {
        ply_destroy((PolylogHandle)ctx);
        return NULL;
    }

    _compute_kernel(ctx);
    
    return (PolylogHandle)ctx;
}

EXPORT void ply_process(PolylogHandle handle, const float* input, float* output) {
    PolylogContext* ctx = (PolylogContext*)handle;
    if (!ctx || !input || !output) return;

    int N = ctx->N;
    int fft_size = ctx->fft_size;
    
    // 1. Prepare Input (Pad to 2N)
    // First N = Input, Second N = 0 (Zero Padding)
    for(int i=0; i<N; i++) {
        ctx->time_buffer[i].r = input[i];
        ctx->time_buffer[i].i = 0.0f;
    }
    for(int i=N; i<fft_size; i++) {
        ctx->time_buffer[i].r = 0.0f;
        ctx->time_buffer[i].i = 0.0f;
    }
    
    // 2. Forward FFT (Size 2N)
    kiss_fft(ctx->cfg_fwd, ctx->time_buffer, ctx->freq_buffer);
    
    // 3. Convolution (Freq Domain Multiply)
    for(int i=0; i<fft_size; i++) {
        // (a+bi)(c+di) = (ac-bd) + (ad+bc)i
        // Kernel is real-only: r = ar*kr, i = ai*kr
        float kr = ctx->kernel[i].r;
        
        ctx->freq_buffer[i].r *= kr;
        ctx->freq_buffer[i].i *= kr;
    }
    
    // 4. Inverse FFT (Size 2N)
    kiss_fft(ctx->cfg_inv, ctx->freq_buffer, ctx->time_buffer);
    
    // 5. Overlap-Add & Output
    float scale = 1.0f / fft_size;
    
    // 5. Overlap-Add & Output
    // float scale = 1.0f / fft_size; // Already defined above
    
    for(int i=0; i<N; i++) {
        // Output = Current Conv Result (Part 1) + Previous Overflow
        float val = ctx->time_buffer[i].r * scale;
        output[i] = val + ctx->overflow_buffer[i];
    }
    
    // 6. Save Overflow for Next Block
    for(int i=0; i<N; i++) {
        // Overflow = Current Conv Result (Part 2)
        // This 'tail' becomes the start of the next block's addition
        ctx->overflow_buffer[i] = ctx->time_buffer[N + i].r * scale;
    }
}

EXPORT void ply_update_s(PolylogHandle handle, float s) {
    PolylogContext* ctx = (PolylogContext*)handle;
    if (ctx && ctx->s != s) {
        ctx->s = s;
        _compute_kernel(ctx);
    }
}

EXPORT void ply_destroy(PolylogHandle handle) {
    PolylogContext* ctx = (PolylogContext*)handle;
    if(ctx) {
        if (ctx->cfg_fwd) free(ctx->cfg_fwd);
        if (ctx->cfg_inv) free(ctx->cfg_inv);
        if (ctx->time_buffer) free(ctx->time_buffer);
        if (ctx->freq_buffer) free(ctx->freq_buffer);
        if (ctx->kernel) free(ctx->kernel);
        if (ctx->overflow_buffer) free(ctx->overflow_buffer);
        free(ctx);
    }
}
