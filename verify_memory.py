import numpy as np
from pltm import PLTM_Engine
import time

def test_memory_retention_v2():
    context_size = 2048
    s = 0.5 # Singularity index
    
    print(f"--- Testing PLTM Memory Retention V2 (Overlap-Add) (N={context_size}, s={s}) ---")
    
    # 1. Initialize Engine
    engine = PLTM_Engine(context_size, s)
    
    # 2. Create Inputs
    # Chunk 1: Impulse (Strong signal at valid indices)
    chunk1 = np.zeros(context_size, dtype=np.float32)
    chunk1[0:100] = 1.0 # First 100 items are 1.0
    
    # Chunk 2: Silence (All zeros)
    chunk2 = np.zeros(context_size, dtype=np.float32)
    
    # 3. Process Sequence
    print("Processing Chunk 1 (Impulse)...")
    out1 = engine.process(chunk1)
    
    print("Processing Chunk 2 (Silence)...")
    out2 = engine.process(chunk2)
    
    # 4. Analyze Results
    norm1 = np.linalg.norm(out1)
    norm2 = np.linalg.norm(out2)
    
    print(f"\n[Output 1] Norm: {norm1:.4f}")
    print(f"[Output 2] Norm: {norm2:.4f}")
    
    if norm2 > 0.0001:
        print("\n[SUCCESS] Memory DETECTED in Output 2.")
        print("The engine retained information from Chunk 1 and influenced Chunk 2.")
        print(f"Memory strength (Norm2/Norm1): {norm2/norm1:.4f}")
        
        # Check first few elements of out2
        print(f"First 10 elements of Output 2: {out2[:10]}")
    else:
        print("\n[FAILURE] NO Memory detected in Output 2.")
        print("Output 2 was effectively zero, meaning context was lost between chunks.")

if __name__ == "__main__":
    test_memory_retention_v2()
