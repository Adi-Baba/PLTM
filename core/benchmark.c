#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "polylog.h"

// Simple timer
double get_time() {
    return (double)clock() / CLOCKS_PER_SEC;
}

int main() {
    printf("=== PLTM Core Speed Benchmark ===\n");
    
    // Configuration
    int N = 2048;           // Standard LLM Context Window
    int ITERATIONS = 5000;  // Total tokens = N * ITERATIONS = ~10 Million
    float s = 0.1f;
    
    printf("Context Size: %d\n", N);
    printf("Iterations:   %d\n", ITERATIONS);
    printf("Total Tokens: %d\n", N * ITERATIONS);
    
    // Initialize
    printf("[Init] Allocating memory...\n");
    PolylogContext* ctx = ply_create_context(N, s);
    
    float* input = (float*)malloc(N * sizeof(float));
    float* output = (float*)malloc(N * sizeof(float));
    
    // Fill dummy data
    for(int i=0; i<N; i++) input[i] = (float)i / N;
    
    printf("[Run] Starting stress test...\n");
    double start_time = get_time();
    
    for(int i=0; i<ITERATIONS; i++) {
        ply_process(ctx, input, output);
        // Prevent compiler optimization
        if (output[0] == -9999.0f) printf("Impossible"); 
    }
    
    double end_time = get_time();
    double duration = end_time - start_time;
    
    long long total_tokens = (long long)N * ITERATIONS;
    double tokens_per_sec = total_tokens / duration;
    
    printf("\n=== RESULTS ===\n");
    printf("Time:           %.4f seconds\n", duration);
    printf("Throughput:     %.2f Million Tokens/sec\n", tokens_per_sec / 1000000.0);
    
    // Cleanup
    free(input);
    free(output);
    ply_destroy(ctx);
    
    return 0;
}
