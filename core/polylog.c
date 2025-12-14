#include "polylog.h"
#include "kiss_fft/kiss_fft.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

struct PolylogContext {
    int N;
    float s;
    
    // KissFFT Configurations
    kiss_fft_cfg cfg_fwd;
    kiss_fft_cfg cfg_inv;
    
    // Buffers (interleaved complex: r, i, r, i...)
    kiss_fft_cpx* time_buffer;
    kiss_fft_cpx* freq_buffer;
    kiss_fft_cpx* kernel;
};

// Precompute the k^-s kernel
void _compute_kernel(PolylogContext* ctx) {
    // k=0 is DC. Preserved.
    ctx->kernel[0].r = 1.0f;
    ctx->kernel[0].i = 0.0f;
    
    for(int k=1; k < ctx->N; k++) {
        // Polylogarithmic decay weight: k^(-s)
        float w = powf((float)k, -ctx->s);
        ctx->kernel[k].r = w;
        ctx->kernel[k].i = 0.0f;
    }
}

EXPORT PolylogContext* ply_create_context(int N, float s) {
    PolylogContext* ctx = (PolylogContext*)malloc(sizeof(PolylogContext));
    ctx->N = N;
    ctx->s = s;
    
    // Allocate Plans (1 = Inverse, 0 = Forward)
    ctx->cfg_fwd = kiss_fft_alloc(N, 0, NULL, NULL);
    ctx->cfg_inv = kiss_fft_alloc(N, 1, NULL, NULL);
    
    // Allocate Buffers
    ctx->time_buffer = (kiss_fft_cpx*)malloc(sizeof(kiss_fft_cpx) * N);
    ctx->freq_buffer = (kiss_fft_cpx*)malloc(sizeof(kiss_fft_cpx) * N);
    ctx->kernel = (kiss_fft_cpx*)malloc(sizeof(kiss_fft_cpx) * N);
    
    _compute_kernel(ctx);
    
    return ctx;
}

EXPORT void ply_process(PolylogContext* ctx, const float* input, float* output) {
    int N = ctx->N;
    
    // 1. Copy Input -> Complex Buffer
    for(int i=0; i<N; i++) {
        ctx->time_buffer[i].r = input[i];
        ctx->time_buffer[i].i = 0.0f;
    }
    
    // 2. Forward FFT
    kiss_fft(ctx->cfg_fwd, ctx->time_buffer, ctx->freq_buffer);
    
    // 3. Convolution (Freq Domain Multiply)
    for(int i=0; i<N; i++) {
        // (a+bi)(c+di) = (ac-bd) + (ad+bc)i
        // Here kernel is real-only (di=0), simplifies to:
        // r = ar, i = bi
        float ar = ctx->freq_buffer[i].r;
        float ai = ctx->freq_buffer[i].i;
        float kr = ctx->kernel[i].r;
        
        ctx->freq_buffer[i].r = ar * kr;
        ctx->freq_buffer[i].i = ai * kr;
    }
    
    // 4. Inverse FFT
    kiss_fft(ctx->cfg_inv, ctx->freq_buffer, ctx->time_buffer);
    
    // 5. Output Real Part (normalized by N)
    float scale = 1.0f / N;
    for(int i=0; i<N; i++) {
        output[i] = ctx->time_buffer[i].r * scale;
    }
}

EXPORT void ply_update_s(PolylogContext* ctx, float s) {
    if (ctx->s != s) {
        ctx->s = s;
        _compute_kernel(ctx);
    }
}

EXPORT void ply_destroy(PolylogContext* ctx) {
    if(ctx) {
        free(ctx->cfg_fwd); // Note: kiss_fft_free is alias for free
        free(ctx->cfg_inv);
        free(ctx->time_buffer);
        free(ctx->freq_buffer);
        free(ctx->kernel);
        free(ctx);
    }
}
