import time
import numpy as np
from pltm import PLTM_Engine

def benchmark():
    N = 2048
    s = 0.1
    iterations = 1000
    
    print(f"Initializing PLTM Engine (N={N}, s={s})...")
    engine = PLTM_Engine(N, s)
    
    # Warmup
    dummy = np.random.rand(N).astype(np.float32)
    engine.process(dummy)
    
    print(f"Running {iterations} iterations...")
    start_time = time.time()
    
    for _ in range(iterations):
        # We generate random data outside timing if we want pure processing speed,
        # but typically we want end-to-end latency including data transfer.
        # However, for 'Throughput' of the engine, we often pre-generate or reuse.
        # Let's reuse 'dummy' to measure pure engine speed + python overhead.
        engine.process(dummy)
        
    total_time = time.time() - start_time
    avg_latency = total_time / iterations
    throughput = (N * iterations) / total_time
    
    print("\nResults:")
    print(f"Total Time: {total_time:.4f}s")
    print(f"Average Latency: {avg_latency*1000:.4f} ms per context window")
    print(f"Throughput: {throughput/1e6:.2f} Million Tokens/sec")

if __name__ == "__main__":
    benchmark()
