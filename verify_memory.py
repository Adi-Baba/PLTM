import numpy as np
from pltm import PLTM_Engine
import time

def test_memory_retention_v2():
    context_size = 2048
    s = 0.5 # Singularity index
    
    print(f"--- Testing PLTM Memory Retention V2 (N={context_size}, s={s}) ---")
    
    # 1. Initialize Engine (Normalized)
    # gain=1.0 ensures stable energy
    # damping=0.99 ensures eventual silence
    engine = PLTM_Engine(context_size, s, gain=1.0, damping=0.99)
    
    # 2. Create Inputs
    # Chunk 1: Impulse (Strong signal at start)
    chunk1 = np.zeros(context_size, dtype=np.float32)
    chunk1[0:100] = 1.0 
    
    # Chunk 2+: Silence
    chunk_silence = np.zeros(context_size, dtype=np.float32)
    
    # 3. Process Sequence
    print("Processing Chunk 1 (Impulse)...")
    out1 = engine.process(chunk1)
    
    print("Processing 10 chunks of Silence (Observing Decay)...")
    decay_curve = []
    decay_curve.append(np.max(np.abs(out1)))
    
    for i in range(10):
        out = engine.process(chunk_silence)
        max_val = np.max(np.abs(out))
        decay_curve.append(max_val)
        
        # ASCII Bar plot
        bar_len = int(max_val * 20)
        bar = "#" * bar_len
        print(f"  Chunk {i+2}: Max={max_val:.4f} |{bar}")
        
    # Analyze
    start_val = decay_curve[1] # First tail
    end_val = decay_curve[-1]
    
    if end_val < start_val:
        print("\n[SUCCESS] Memory Decay CONFIRMED.")
        print(f"Signal decayed from {start_val:.4f} to {end_val:.4f} over 10 chunks.")
    else:
        print("\n[WARNING] Signal did not decay (or grew). Check normalization.")

    # Reset Check
    engine.reset()
    out_reset = engine.process(chunk_silence)
    if np.max(np.abs(out_reset)) < 1e-6:
        print("[SUCCESS] Reset verified (Output is ~zero).")
    else:
        print(f"[FAILURE] Reset failed (Max={np.max(np.abs(out_reset))})")

if __name__ == "__main__":
    test_memory_retention_v2()
